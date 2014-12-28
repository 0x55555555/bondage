
module Script

  module Helper

    def self.generateRequires(resolver, exposer, clss, coreName=nil)
      if (coreName)
        core = yield(coreName, resolver.coreClassPath())
      end

      reqs = resolver.coreRequires()
      if (reqs.length != 0)
        core += "\n" + reqs.join("\n")
      end

      if (clss.length == 0)
        return core + "\n\n"
      end

      return clss.map{ |clsName|
        cls = exposer.allMetaData.findClass(clsName)
        yield(cls.name, resolver.pathFor(cls))
        }.join("\n") + "\n" + core + "\n\n"
    end

  end
end