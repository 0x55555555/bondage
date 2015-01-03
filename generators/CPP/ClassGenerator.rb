require_relative 'FunctionGenerator'
require_relative 'ExposureTypes/Types'

module CPP

  # Generate exposures for classes
  class ClassGenerator
    def initialize(modes = DefaultClassTypes)
      reset()
      @modes = modes
    end

    attr_reader :interface, :implementation, :wrapperName

    def reset()
      @interface = ""
      @implementation = ""
      @wrapperName = ""

      @fnGen = CPP::FunctionGenerator.new("", "  ")
    end

    def generate(exposer, md, libraryVariable, files)
      @metaData = md
      @cls = md.parsed

      raise "Unparsed classes can not be exposed #{md.name}" unless @cls && @metaData

      @exposer = exposer

      type = classMode()
      validation, err = type.validate(@metaData)
      if (!validation)
        raise "Invalid class mode: #{err}"
      end

      if md.fullyExposed
        generateHeader(type)
        generateSource(type, libraryVariable, files)
      else
        generatePartial(type)
      end
    end

    def findRootClass(md)
      distance = 0
      cls = md.parentClass
      while(!cls.empty?)
        foundClass = @exposer.allMetaData.findClass(cls)
        raise "Failed to locate exposed class #{cls} in exposer #{@exposer.allMetaData.debugTypes}" unless foundClass

        parentName = foundClass.parentClass
        distance += 1
        if (!parentName)
          return cls, distance
        end
        cls = parentName
      end

      return nil, distance
    end

  private
    def generatePartial(type)
      raise "partial class without parent #{clsPath}" unless @metaData.hasParentClass()

      root, dist = findRootClass(@metaData)
      dist = nil
      @interface = type.generateHeader(:partial, @metaData, root)
    end

    def generateHeader(type)
      root = nil
      if(@metaData.hasParentClass())
        root, dist = findRootClass(@metaData)
      end
      @interface = type.generateHeader(:full, @metaData, root)
    end

    # Generate binding data for a class
    def generateSource(type, libraryVariable, files)
      # find a name that is a valid literal in c++ used for static definitions
      fullyQualified = @cls.fullyQualifiedName()
      @wrapperName = fullyQualified.sub("::", "").gsub("::", "_")

      methods, extraMethods, typedefs = @fnGen.gatherFunctions(@cls, @exposer, files)

      methodsLiteral, methodsArray, extraMethodSource = @fnGen.generateFunctionArray(typedefs, methods, extraMethods, @wrapperName)

      parent = @metaData.parentClass

      classInfo = type.generateSource(
        @wrapperName,
        libraryVariable,
        @cls,
        parent,
        methodsLiteral,
        methods)

      @implementation =
"// Exposing class #{fullyQualified}#{extraMethodSource}#{methodsArray}
#{classInfo}
"
    end

    def classMode
      cmd = @cls.comment.command("expose")

      mode = :default
      @modes.each do |possMode, factory|
        str = possMode.to_s
        if (cmd != nil && cmd.hasArg(str))
          raise "'Multiple class management modes specified #{mode.to_s} and '#{str}'." if mode != :default
          mode = possMode
        end
      end

      return @modes[mode]
    end
  end
end