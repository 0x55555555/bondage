require_relative "FunctionGenerator.rb"

module CPP

  # Generate exposures for classes
  class ClassGenerator
    def initialize()
      reset()
    end

    attr_reader :interface, :implementation, :wrapperName

    def reset()
      @interface = ""
      @implementation = ""
      @wrapperName = ""

      @fnGen = CPP::FunctionGenerator.new("", "  ")
    end

    def generate(exposer, md, libraryVariable)
      @metaData = md
      @cls = md.parsed

      raise "Unparsed classes can not be exposed #{md.name}" unless @cls && @metaData

      @exposer = exposer

      generateHeader()
      generateSource(libraryVariable)
    end

    def findRootClass(md)
      distance = 0
      cls = md.parentClass
      while(!cls.empty?)
        parentName = @exposer.exposedMetaData.findClass(cls).parentClass
        distance += 1
        if (!parentName)
          return cls, distance
        end
        cls = parentName
      end

      return nil, distance
    end

  private
    def generateHeader()
      clsPath = @cls.fullyQualifiedName
      if(!@metaData.hasParentClass())
        type = classMode()
        derivable = ""
        if (@metaData.isDerivable)
          derivable = "DERIVABLE_"
        end
        @interface = "#{MACRO_PREFIX}EXPOSED_CLASS_#{derivable}#{type}(#{clsPath})"
      else
        parent = @metaData.parentClass
        root, dist = findRootClass(@metaData)
        @interface = "#{MACRO_PREFIX}EXPOSED_DERIVED_CLASS(#{clsPath}, #{parent}, #{root})"
      end
    end

    # Generate binding data for a class
    def generateSource(libraryVariable)
      # find a name that is a valid literal in c++ used for static definitions
      fullyQualified = @cls.fullyQualifiedName()
      @wrapperName = fullyQualified.sub("::", "").gsub("::", "_")

      methods, extraMethods = @fnGen.gatherFunctions(@cls, @exposer)

      methodsLiteral, methodsArray, extraMethodSource = @fnGen.generateFunctionArray(methods, extraMethods, @wrapperName)

      classInfo =
"#{MACRO_PREFIX}IMPLEMENT_EXPOSED_CLASS(
  #{wrapperName},
  #{libraryVariable},
  #{@cls.parent.fullyQualifiedName()},
  #{@cls.name},
  #{methodsLiteral},
  #{methods.length});"


      @implementation =
"// Exposing class #{fullyQualified}#{extraMethodSource}#{methodsArray}
#{classInfo}
"
    end

    CLASS_MODES = [ :copyable, :managed, :unmanaged ]

    def classMode
      cmd = @cls.comment.command("expose")

      mode = :default
      defaultMode = :copyable
      CLASS_MODES.each do |possMode|
        str = possMode.to_s
        if (cmd.hasArg(str))
          raise "'Multiple class management modes specified #{mode.to_s} and '#{str}'." if mode != :default
          mode = possMode
        end
      end

      if (mode == :default)
        if (!@metaData.hasParentClass())
          mode = :copyable
        else
          mode = :managed
        end
      end

      raise "#{@cls.locationString}: A copyable class cannot be derivable." if @metaData.isDerivable && mode == :copyable

      return mode.to_s.upcase
    end
  end
end