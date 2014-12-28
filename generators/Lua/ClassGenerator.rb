require_relative '../Script/ClassGenerator'
require_relative '../../exposer/ParsedLibrary'
require_relative 'Function/Generator'
require_relative 'RequireHelper'
require_relative 'EnumGenerator'

module Lua

  # Generate lua exposing code for C++ classes
  class ClassGenerator < Script::ClassGenerator
    def initialize(
        classPlugins,
        classifiers,
        externalLine,
        lineStart,
        getter,
        resolver)
      super(classPlugins, classifiers, externalLine, lineStart, getter, resolver)
      @fnGen = Function::Generator.new(classifiers, externalLine, @lineStart, getter)
      @enumGen = Lua::EnumGenerator.new(@lineStart)
    end

    def setLocalVariableName(var)
      @variableName = var
    end

  private

    def memberSeparator()
      return ",\n"
    end

    def generateRequires(exposer, requiredClasses)
      return Helper::generateRequires(@resolver, exposer, requiredClasses, 'class')
    end

    def formatClass(includes, preamble, brief, library, cls, parentTag, pluginTag, enumTag, functions)
      clsName = "class"

      # generate class output.
      @classDefinition = "#{includes}#{preamble}#{Script::DocumentationGenerator.new.generateClass('--', '', brief)}
--
local #{@variableName} = #{clsName} \"#{cls.name}\" {
  library = \"#{library.name}\",
#{parentTag}#{pluginTag}#{enumTag}
#{functions.join(",\n\n")}
}"
    end

    def generateFunction(library, parsed, name, fns, formattedFunctions, extraData, requiredClasses)
      @fnGen.generate(library, parsed, fns, requiredClasses)

      bind = @fnGen.bind
      name = @fnGen.name

      # Visit the plugins for our class, they may choose
      # to do something with this function later.
      pluginInterested = isPluginInterested(name, fns)

      if (@fnGen.wrapper.length != 0)
        extraData << @fnGen.wrapper
      end

      if (pluginInterested)
        varName = ""
        if (@fnGen.bindIsForwarder)
          varName = bind
        else
          # someone wants a reference to this function other than the def - store it in a local for passing on
          varName = "#{parsed.name}_#{name}_fwd"
          extraData << "local #{varName} = #{bind}"
        end

        bind = varName
        
        pluginInterested.each{ |p| p.addFunctions(name, fns, bind) }
      end

      formattedFunctions << "#{@fnGen.docs}\n#{@lineStart}#{name} = #{bind}"
    end

    def formatParentClass(parent)
      parentRequirePath = @resolver.pathFor(parent)

      return "  super = require \"#{parentRequirePath}\"" + memberSeparator
    end
  end

end