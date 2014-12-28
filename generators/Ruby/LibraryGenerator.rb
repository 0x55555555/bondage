require_relative '../Script/LibraryGenerator'
require_relative '../../exposer/ParsedLibrary'
require_relative '../GeneratorHelper'
require_relative '../Script/RequireHelper'
require_relative 'ClassGenerator'

module Ruby

  # Generate lua exposing code for C++ classes
  class LibraryGenerator < Script::LibraryGenerator
    # create a lua generator for a [library], and a given [exposer].
    def initialize(classPlugins, classifiers, resolver, headerHelper)
      super(classPlugins, classifiers, resolver, headerHelper)
      
      @lineStart = ""

      @clsGen = ClassGenerator.new(classPlugins, classifiers, "", @lineStart, resolver)
      @fnGen = FunctionGenerator.new(@classifiers, "", @lineStart)
    end

    def fileExtension
      return "rb"
    end

    def scriptType
      return :ruby
    end

    def classFileReturn(cls)
      return ""
    end

    def generateRequires(exposer, requiredClasses)
      return Script::Helper::generateRequires(@pathResolver, exposer, requiredClasses) do |cls, path|
        next 'require \'#path\''
      end
    end

    def generateLibrary(includes, preamble, libraryName, classes, data)
      classContents = classes.join("\n\n")
      contents = data.join("\n\n")

      @library = "#{includes}#{preamble}#{classContents}\n\nmodule #{@libraryName}\n#{contents}\n\nend\n"
    end

    def generateClass(library, exposer, cls)
      @clsGen.generate(library, exposer, cls)

      return @clsGen.classDefinition
    end

    def generateFunction(library, rootNs, fns, requiredClasses)
      @fnGen.generate(library, rootNs, fns, requiredClasses)

      return @fnGen.docs, nil
    end

    def generateEnums(library, exposer, rootNs)
      enumGen = EnumGenerator.new(@lineStart)

      enumGen.generate(rootNs, exposer)
      return enumGen.enums
    end

    def generateClassImport(cls)
      "#{@lineStart}require_relative '#{@pathResolver.pathFor(cls)}'"
    end
  end

end