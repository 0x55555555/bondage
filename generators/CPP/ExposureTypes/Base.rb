
module CPP

module ClassTypes

class Base

  def validate(cls)
    return true
  end

  def generateHeader(type, metaData, root)
    clsPath = metaData.parsed.fullyQualifiedName
    exportMacro = metaData.library.exportMacro

    if (type == :partial)
      parent = metaData.parentClass
      return "#{MACRO_PREFIX}EXPOSED_DERIVED_PARTIAL_CLASS(#{exportMacro}, #{clsPath}, #{parent}, #{root})"
    end

    if(!metaData.hasParentClass())
      derivable = ""
      if (metaData.isDerivable)
        derivable = "DERIVABLE_"
      end
      return "#{MACRO_PREFIX}EXPOSED_CLASS_#{derivable}#{macroName}(#{exportMacro}, #{clsPath})"
    else
      parent = metaData.parentClass
      return "#{MACRO_PREFIX}EXPOSED_CLASS_DERIVED_#{macroName}(#{exportMacro}, #{clsPath}, #{parent}, #{root})"
    end
  end

  def generateSource(wrapperName,
      libraryVariable,
      cls,
      parent,
      methodsLiteral,
      methods)

    return "#{MACRO_PREFIX}IMPLEMENT_EXPOSED_CLASS(
  #{wrapperName},
  #{libraryVariable},
  #{cls.parent.fullyQualifiedName()},
  #{cls.name},
  #{parent ? parent : "void"},
  #{methodsLiteral},
  #{methods.length});"
  end
end

end

end
