require_relative 'IndexClassifier'
require_relative 'NamedClassifier'

module Lua

  DEFAULT_CLASSIFIERS = {
    :index => IndexClassifier.new,
    :named => NamedClassifier.new,
  }

end
