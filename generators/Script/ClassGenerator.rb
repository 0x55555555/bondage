require_relative '../../exposer/ParsedLibrary'

module Script

  # Generate lua exposing code for C++ classes
  class ClassGenerator
    def initialize(
        classPlugins,
        classifiers,
        externalLine,
        lineStart,
        resolver)
      @lineStart = lineStart
      @plugins = classPlugins
      @resolver = resolver
    end

    attr_reader :classDefinition

    def reset
      @classDefinition = ""
    end


    # Generate the lua class data for [cls]
    def generate(library, exposer, cls)
      parsed = cls.parsed

      @plugins.each { |n, plugin| plugin.beginClass(library, parsed) }

      requiredClasses = Set.new
      formattedFunctions, extraData = generateFunctions(library, exposer, parsed, requiredClasses)


      # if [cls] has a parent class, find its data and require path.
      parentInsert = generateClassParentData(exposer, cls)

      enumInsert = generateEnums(parsed, exposer)

      # find a brief comment for [cls]
      brief = parsed.comment.commandText("brief")

      extraDatas = ""
      if (extraData.length != 0)
        extraDatas = extraData.join("\n\n") + "\n\n"
      end


      pluginInsert = generatePluginData(requiredClasses)

      inc = generateRequires(exposer, requiredClasses)

      return formatClass(inc, extraDatas, brief, library, cls, parentInsert, pluginInsert, enumInsert, formattedFunctions)
    end

  private
    def generatePluginData(requiredClasses)
      pluginInsertData = @plugins.map { |n, plugin|
        plugin.endClass(@lineStart, requiredClasses)
      }.select{ |r|
        r != nil && !r.empty?
      }

      if (pluginInsertData.length == 0)
        return ""
      end

      return "\n" + pluginInsertData.join(",\n\n") + ",\n"
    end

    def generateEnums(parsed, exposer)
      @enumGen.generate(parsed, exposer)

      if (@enumGen.enums.length == 0)
        return ""
      end

      out = @enumGen.enums.map { |enum|
         "#{enum}"
      }.join("\n" + memberSeparator())

      return "\n#{out}" + memberSeparator()
    end

    def isPluginInterested(name, fns)
      interested = []
      @plugins.each do |pluginName, plugin|
        if (plugin.interestedInFunctions?(name, fns))
          interested << plugin
        end
      end

      return interested.length > 0 ? interested : nil
    end

    def generateFunctions(library, exposer, parsed, requiredClasses)
      operatorMatch = /\A[a-zA-Z_0-9]+\z/
      functions = exposer.findExposedFunctions(parsed).select do |name, fns|
        operatorMatch.match(name) != nil
      end 

      extraData = []
      formattedFunctions = []

      # for each function, work out how best to call it.
      functions.sort.each do |name, fns|
        generateFunction(library, parsed, name, fns, formattedFunctions, extraData, requiredClasses)
      end

      return formattedFunctions, extraData
    end

    def generateClassParentData(exposer, cls)
      # if [cls] has a parent class, find its data and require path.
      parentInsert = ""
      if(cls.parentClass)
        parent = exposer.allMetaData.findClass(cls.parentClass)
        raise "Missing parent dependency '#{ls.parentClass}'" unless parent

        parentInsert = formatParentClass(parent)
      end

      return parentInsert
    end
  end

end