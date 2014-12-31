require_relative 'Base'

module CPP

module ClassTypes

class Basic < Base
  def initialize(macro)
    @macro = macro
  end

  def macroName
    return @macro
  end
end

end

end
