require_relative '../Script/EnumGenerator'

module Lua

  # Generate lua exposing code for C++ enums
  class EnumGenerator < Script::EnumGenerator

    def beginEnum()
      return "{"
    end

    def endEnum()
      return "}"
    end

    def enumValue(k, v)
      return "#{k} = #{v},"
    end

    def enumDefinition(name, enum)
      return "#{name} = #{enum}"
    end
  end

end