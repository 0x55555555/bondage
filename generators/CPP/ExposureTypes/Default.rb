require_relative 'Base'
require_relative 'Copyable'
require_relative 'Basic'

module CPP

module ClassTypes

class Default < Base
  def initialize
    @copyable = Copyable.new
    @managed = Basic.new("MANAGED")
  end

  def validate(metaData)
    return findForwarder(metaData).validate(metaData)
  end

  def generateHeader(type, metaData, root)
    return findForwarder(metaData).generateHeader(type, metaData, root)
  end

private
  def findForwarder(metaData)
    fwd = @copyable
    if (metaData.hasParentClass())
      fwd = @managed
    end

    return fwd
  end
end

end

end