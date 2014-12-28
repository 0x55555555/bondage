require_relative '../Script/SignatureGenerator'

module Ruby

  class SignatureGenerator < Script::SignatureGenerator

    def staticCallConvention()
      return "::"
    end

    def methodCallConvention()
      return "#"
    end

    # Format [type], a Type instance, in a way lua users can understand
    def scriptType(type)
      # void maps to nil
      if(!type || type.isVoid())
        return "nil"
      end

      # char pointers map to string
      if(type.isStringLiteral())
        return "string"
      end

      # bool is boolean
      if(type.isBoolean())
        return "boolean"
      end

      # all int/float/double types are numbers
      if(type.isInteger() || type.isFloatingPoint())
        return "number"
      end

      return "#{type.name}"
    end
  end
end