require_relative '../Script/LibraryGenerator'
require_relative '../../exposer/ParsedLibrary'
require_relative '../GeneratorHelper'
require_relative 'RequireHelper'
require_relative 'ClassGenerator'

module Lua

  # Generate lua exposing code for C++ classes
  class LibraryGenerator < Script::LibraryGenerator
    # create a lua generator for a [library], and a given [exposer].
    def initialize(classPlugins, classifiers, getter, resolver, headerHelper)
      super(classPlugins, classifiers, resolver, headerHelper)

      @getter = getter
      @clsGen = ClassGenerator.new(classPlugins, classifiers, "", @lineStart, getter, resolver)
      @fnGen = Function::Generator.new(@classifiers, "", @lineStart, @getter)
    end

    def fileExtension
      return "lua"
    end

    def scriptType
      return :lua
    end

    def localName(cls)
      return "#{cls.name}_cls"
    end

    def classFileReturn(cls)
      return "return #{localName(cls)}"
    end

    def generateRequires(exposer, requiredClasses)
      return Helper::generateRequires(@pathResolver, exposer, requiredClasses, "class")
    end

    def generateLibrary(includes, preamble, libraryName, classes, data)
      contents = (classes + data).join(",\n\n")

      @library = "#{includes}#{preamble}local #{@libraryName} = {\n#{contents}\n}\n\nreturn #{@libraryName}"
    end

    def generateClass(library, exposer, cls)
      @clsGen.setLocalVariableName(localName(cls))
      @clsGen.generate(library, exposer, cls)

      return @clsGen.classDefinition
    end

    def generateFunction(library, rootNs, fns, requiredClasses)
      @fnGen.generate(library, rootNs, fns, requiredClasses)

      extraData = nil
      if (@fnGen.wrapper.length > 0)
        extraData = @fnGen.wrapper
      end

      data = "#{@fnGen.docs}\n#{@lineStart}#{@fnGen.name} = #{@fnGen.bind}"

      return data, extraData
    end

    def generateEnums(library, exposer, rootNs)
      enumGen = EnumGenerator.new(@lineStart)

      enumGen.generate(rootNs, exposer)
      return enumGen.enums
    end

    def generateClassImport(cls)
      "#{@lineStart}#{cls.name} = require(\"#{@pathResolver.pathFor(cls)}\")"
    end
  end

end