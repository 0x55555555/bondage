
module Script

  # Generate lua exposing code for C++ enums
  class EnumGenerator
    def initialize(lineStart)
      @lineStart = lineStart
    end

    attr_reader :enums

    def reset
      @enums = []
    end

    def generate(owner, exposer)
      reset()

      owner.enums.each do |name, enum|
        if (name.empty? || 
          !exposer.allMetaData.isExposedEnum?(enum.fullyQualifiedName))
          next
        end

        str = beginEnum() + "\n"
        enum.members.each do |k, v|
          str << @lineStart + "  " + enumValue(k, v) + "\n"
        end
        str << @lineStart + endEnum()

        @enums << @lineStart + enumDefinition(name, str)
      end
    end

  end

end