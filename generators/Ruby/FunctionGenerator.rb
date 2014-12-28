require_relative 'SignatureGenerator'
require_relative '../Script/DocumentationGenerator'

module Ruby

  class FunctionGenerator
    def initialize(classifiers, externalLine, line)
      @lineStart = line
      @sigGenerator = Ruby::SignatureGenerator.new
      reset()
    end

    attr_reader :docs

    def reset()
      @brief = ""
      @signatures = Set.new()
      @returnComments = { }
      @namedArgs = { }
      @argumentClassifiers = []
      @returnClassifiers = []
      @arguments = []
      @returnTypes = []
    end

    def generate(library, cls, fns, requiredClasses)
      
      FunctionVisitor.visit(cls, fns, self)

      @docs = Script::DocumentationGenerator.new.generateFunction('#', @lineStart, @signatures, @brief, @returnComments, @namedArgs)
    end

    def visitFunction(owner, function, functionIndex, argCount)
      if (@brief.empty?)
        @brief = function.comment.commandText("brief")
      end

      @arguments = []
      @returnTypes = []

      extractReturnData(function)

      ArgumentVisitor.visitFunction(owner, function, functionIndex, argCount, self)

      @signatures << @sigGenerator.generate(owner, function, @arguments, @returnTypes)
    end

    def extractReturnData(function)
      if (function.returnType)
        type, brief = extractArgumentClassifier(function.returnBrief, @returnClassifiers, 0)
        type = nil

        if (!@returnComments[:result] || @returnComments[:result].empty?)
          @returnComments[:result] = brief.strip
        end

        @returnTypes << function.returnType
      end
    end

    def returnClassifier(i)
      type = @returnClassifiers[i]

      if (!type)
        return :none
      end

      return type
    end

    def visitInputArgument(fn, n, argCount, arg)
      i = @arguments.length
      type, brief = extractArgumentClassifier(arg.brief, @argumentClassifiers, i)
      type = nil

      @arguments << arg

      if (!arg.name.empty? && !@namedArgs.include?(arg.name))
        @namedArgs[arg.name] = brief
      end
    end

    def visitOutputArgument(fn, n, argCount, arg)
      i = @returnTypes.length

      type, brief = extractArgumentClassifier(arg.brief, @returnClassifiers, i)
      type = nil

      @returnTypes << arg.type

      if (!arg.name.empty? && !@namedArgs.include?(arg.name))
        @returnComments["#{i} #{arg.name}"] = brief
      end
    end

    def visitInputOutputArgument(fn, n, argCount, arg)
      visitInputArgument(fn, n, argCount, arg)
      visitOutputArgument(fn, n, argCount, arg)
    end

  private
    ARGUMENT_TYPE_REGEX = /\[([a-z]+)\]\s*(.*)/

    # Strip out an argument type from the brief text
    def extractArgumentClassifier(brief, arr, i)
      cutBrief = brief
      if (brief.strip.length == 0)
        return nil, ""
      end

      type = :none
      match = ARGUMENT_TYPE_REGEX.match(brief)
      if (match)
        type = match.captures[0].to_sym
        cutBrief = match.captures[1]
      end

      if (arr[i] && arr[i] != type)
        raise "Conflicting argument types #{type} and #{arr[i]} in #{brief}"
      end
      arr[i] = type

      @anyClassifiersUsed = @anyClassifiersUsed || type != :none

      return type, cutBrief
    end
  end

end