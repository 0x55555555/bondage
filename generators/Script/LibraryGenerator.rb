require_relative "../../exposer/ParsedLibrary.rb"
require_relative "../GeneratorHelper.rb"
require_relative "RequireHelper.rb"
require_relative "ClassGenerator.rb"

module Script

  # Generate lua exposing code for C++ classes
  class LibraryGenerator
    # create a lua generator for a [library], and a given [exposer].
    def initialize(classPlugins, classifiers, resolver, headerHelper)
      @lineStart = "  "
      @pathResolver = resolver
      @headerHelper = headerHelper
      @classes = { }
      @classifiers = classifiers
    end

    attr_reader :classes, :library

    def write(dir)
      @classes.each do |cls, data|
        path = dir + "/#{cls.name}.#{fileExtension}"
        res = File.open(path, 'w') do |file|
          file.write(@headerHelper.filePrefix(scriptType) + "\n\n")
          file.write(data)
          file.write("\n\n" + classFileReturn(cls))
          file.write(@headerHelper.fileSuffix(scriptType) + "\n")
        end

        raise "Failed to write file #{path}" unless res
      end

      libPath = dir + "/#{@libraryName}Library.#{fileExtension}"
      File.open(libPath, 'w') do |file|
        file.write(@headerHelper.filePrefix(scriptType) + "\n\n")
        file.write(@library)
        file.write(@headerHelper.fileSuffix(scriptType) + "\n")
      end

      raise "Failed to write file #{libPath}" unless libPath
    end

    # Generate lua classes into [dir]
    def generate(visitor, exposer)
      library = visitor.library
      @classes = { }
      @libraryName = library.name


      # for each fully exposed class, we write a file containing the classes methods and data.
      exposer.exposedMetaData.fullTypes.each do |path, cls|
        if(cls.type == :class)
          @classes[cls] = generateClass(library, exposer, cls)
        end
      end

      generateFullLibrary(exposer, library, @classes, visitor.getExposedNamespace())
    end

    def generateFullLibrary(exposer, library, classes, rootNs)
      ls = "#{@lineStart}"

      extraData = []
      requiredClasses = Set.new
      classes = @classes.map{ |cls, idata| generateClassImport(cls) }

      data = []
      appendEnums(library, exposer, rootNs, data)
      appendFunctions(library, exposer, rootNs, data, extraData, requiredClasses)

      extraDatas = ""
      if (extraData.length != 0)
        extraDatas = extraData.join("\n\n") + "\n\n"
      end

      inc = generateRequires(exposer, requiredClasses)

      @library = generateLibrary(inc, extraDatas, @libraryName, classes, data)
    end

    def appendEnums(library, exposer, rootNs, data)
      enums = generateEnums(library, exposer, rootNs)

      enums.each do |enum|
        data << enum
      end
    end

    def appendFunctions(library, exposer, rootNs, data, extraData, requiredClasses)
      functions = exposer.findExposedFunctions(rootNs)

      # for each function, work out how best to call it.
      functions.sort.each do |name, fns|
        definition, preamble = generateFunction(library, rootNs, fns, requiredClasses)

        if (preamble != nil)
          extraData << preamble
        end

        data << definition
      end
    end
  end

end