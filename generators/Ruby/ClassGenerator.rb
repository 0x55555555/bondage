require_relative '../Script/ClassGenerator'
require_relative '../../exposer/ParsedLibrary'
require_relative '../Script/RequireHelper'
require_relative 'EnumGenerator'
require_relative 'FunctionGenerator'

module Ruby

  # Generate lua exposing code for C++ classes
  class ClassGenerator < Script::ClassGenerator
    def initialize(
        classPlugins,
        classifiers,
        externalLine,
        lineStart,
        resolver)
      super(classPlugins, classifiers, externalLine, lineStart + "  ", resolver)
      @fnGen = FunctionGenerator.new(classifiers, externalLine, @lineStart)
      @enumGen = EnumGenerator.new(@lineStart)
    end

    def setLocalVariableName(var)
      @variableName = var
    end

  private

    def memberSeparator()
      return "\n"
    end

    def generateRequires(exposer, requiredClasses)
      return Script::Helper::generateRequires(@resolver, exposer, requiredClasses)
    end

    def formatClass(includes, preamble, brief, library, cls, parentTag, pluginTag, enumTag, functions)
      clsName = "class"

      # generate class output.
      @classDefinition = "#{includes}#{preamble}module #{library.name}

#{Script::DocumentationGenerator.new.generateClass('#', '', brief)}
#
class #{cls.name}#{parentTag}
#{pluginTag}#{enumTag}#{functions.join("\n\n")}
end

end"
    end

    def generateFunction(library, parsed, name, fns, formattedFunctions, extraData, requiredClasses)
      @fnGen.generate(library, parsed, fns, requiredClasses)

      formattedFunctions << @fnGen.docs
    end

    def formatParentClass(parent)
      return " < #{parent.library.name}::#{parent.name}"
    end
  end

end