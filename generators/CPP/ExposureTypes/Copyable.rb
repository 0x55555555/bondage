require_relative 'Basic'

module CPP

module ClassTypes

class Copyable < Basic
  def initialize()
    super("COPYABLE")
  end

  def validate(metadata)
    if (metadata.parsed.hasPureVirtualFunctions())
      return false, "Abstract class #{cls.name} can not be copyable"
    end

    return true
  end
end

end

end
