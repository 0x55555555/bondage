require_relative '../Script/RequireHelper'


module Lua

  module Helper

    def self.generateRequires(resolver, exposer, clss, coreName)

      var = Script::Helper.generateRequires(resolver, exposer, clss, coreName) do |name, path|
        next "local #{name} = require \"#{path}\""
      end

    end
  end
end