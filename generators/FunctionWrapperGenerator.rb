require_relative "GeneratorHelper.rb"

class FunctionWrapperGenerator
  class WrapperArg
    def initialize(type, source, inoutExtra=nil, accessor="")
      @source = source
      @type = type
      @inoutSource = inoutExtra
      @inputType = accessor
    end

    attr_reader :type, :source, :inoutSource

    def callAccessor
      if (@inputType == :pointer)
        return "&"
      end
      return ""
    end

    def dataAccessor
      if (@inputType == :pointer)
        return "*"
      end
      return ""
    end
  end

  def generateCall(ls, owner, function, functionIndex, argCount, calls, extraFunctions)
    @lineStart = ls
    @constructor = function.isConstructor
    @static = function.static || @constructor
    @needsWrapper = argCount != function.arguments.length || @constructor
    @functionWrapper = nil
    @callArgs = []
    @owner = owner
    @function = function
    @functionIndex = functionIndex
    @name = function.name
    @inputArguments = []
    @outputArguments = []

    if (!@static)
      @inputArguments << "#{owner.fullyQualifiedName} &"
    end

    if (@constructor)
      @outputArguments << "#{owner.fullyQualifiedName} *"
    elsif (function.returnType)
      @outputArguments << function.returnType.name
    end

    # visit arguments of function.
    ArgumentVisitor.visitFunction(owner, function, functionIndex, argCount, self)

    if (@needsWrapper)
      generateWrapper(calls, extraFunctions)
    else
      sig = signature()
      calls << "cobra::function_builder::build_call<#{sig}, &#{@function.fullyQualifiedName}>"
    end
  end

  def generateWrapper(calls, extraFunctions)
    ret = returnType()
    extraFnName = literalName()
    resVar = resultName()

    inArgs = @inputArguments.each_with_index.map{ |arg, i| "#{arg} #{inputArgName(i)}" }.join(', ')

    hasReturnType = @constructor || @function.returnType

    initArgs = []
    if (@outputArguments.length > 1 || (@outputArguments.length > 0 && !hasReturnType))
      initArgs << "#{ret} #{resVar};"
    end

    ls = @lineStart
    olLs = @lineStart + "  "

    call = @callArgs.map do |arg|
      if (arg.type == :input)
        next arg.callAccessor + inputArgPassThrough(arg.source)
      elsif (arg.type == :output)
        next arg.callAccessor + outputArgReference(arg.source)
      elsif (arg.type == :inout)
        input = inputArgPassThrough(arg.inoutSource)
        output = outputArgReference(arg.source)
        initArgs << "#{output} = #{arg.dataAccessor} #{input};"
        next arg.callAccessor + output
      else
        raise "invalid arg type #{arg.type}"
      end
    end

    args = call.join(', ')
    call = ""
    if (@constructor)
      call = "#{TYPE_NAMESPACE}::Type<#{@owner.fullyQualifiedName}>::create(#{args})"
    else
      accessor = functionAccessor()
      call = "#{accessor}(#{args});"
    end

    if (hasReturnType)
      if (@outputArguments.length > 1)
        call = "#{outputArgReference(0)} = #{call}"
      else
        call = "auto &&#{resVar} = #{call}"
      end
    end

    returnExtra = ""
    if (@outputArguments.length > 0)
      returnExtra = "\n#{olLs}return #{resVar};"
    end

    sig = signature()
    callType = @static ? "build_call" : "build_member_standin_call"
    calls << "cobra::function_builder::#{callType}<#{sig}, &#{extraFnName}>"

    extra = ""
    if (initArgs.length != 0)
      extra = initArgs.join("\n") + "\n\n"
    end

    extraFunctions << 
"#{ls}#{ret} #{extraFnName}(#{inArgs})
#{ls}{
#{olLs}#{extra}#{call}#{returnExtra}
#{ls}}"
  end

  def visitInputOutputArgument(fn, idx, cnt, arg)
    outIdx, access = addOutputArgumentHelper(arg)

    inIdx = @inputArguments.length
    @inputArguments << arg.type.name
    @callArgs << WrapperArg.new(:inout, outIdx, inIdx, access)
    @needsWrapper = true
  end
  
  def visitInputArgument(fn, idx, cnt, arg)
    inIdx = @inputArguments.length
    @inputArguments << arg.type.name
    @callArgs << WrapperArg.new(:input, inIdx)
  end

  def visitOutputArgument(fn, idx, cnt, arg)
    outIdx, access = addOutputArgumentHelper(arg)

    @callArgs << WrapperArg.new(:output, outIdx, nil, access)
    @needsWrapper = true
  end

private
  def addOutputArgumentHelper(arg)
    outIdx = @outputArguments.length
    name = arg.type.name
    accessor = ""

    if (arg.type.isPointer)
      accessor = :pointer
      name = arg.type.pointeeType().name
    elsif (arg.type.isLValueReference)
      name = arg.type.pointeeType().name
    elsif (arg.type.isRValueReference)
      raise "R value reference as an output? this needs some thought."
    end
    @outputArguments << name

    return outIdx, accessor
  end

  def signature()
    result = returnType()

    ptrType = "(*)"
    types = nil
    if (@needsWrapper)
      types = @inputArguments.join(", ")
    else
      types = @function.arguments.map{ |arg| arg.type.name }.join(", ")
      if (!@static)
        ptrType = "(#{@owner.fullyQualifiedName}::*)"
      end
    end

    return "#{result}#{ptrType}(#{types})"
  end

  def inputArgName(i)
    return "inputArg#{i}"
  end

  def inputArgPassThrough(i)
    type = @inputArguments[i]
    return "std::forward<#{type}>(#{inputArgName(i)})"
  end

  def outputArgReference(i)
    if (@outputArguments.length > 1)
      return "std::tuple::get<#{i}>(#{resultName()})"
    end

    return resultName()
  end

  def resultName
    return "result"
  end

  def functionAccessor
    if (@constructor)
      return "#{@owner.fullyQualifiedName}"
    end

    if (!@function.static)
      return "#{inputArgName(0)}.#{@function.name}"
    end

    return @function.fullyQualifiedName
  end
  def returnType
    if (@outputArguments.length == 0)
      return "void"
    elsif (@outputArguments.length == 1)
      return @outputArguments[0]
    end

    return "std::tuple<#{@outputArguments.join(', ')}>"
  end

  def literalName
    fullyQualified = @function.fullyQualifiedName()
    literalName = fullyQualified.sub("::", "").gsub("::", "_")
    if (@functionIndex)
      literalName += "_overload#{@functionIndex}"
    end
    return literalName
  end
end