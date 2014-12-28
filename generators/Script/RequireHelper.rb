
module Script

  module Helper

    def self.generateRequires(resolver, exposer, clss, coreName=nil)
      core = ""
      if (coreName)
        core = yield(coreName, resolver.coreClassPath())
      end

      reqs = resolver.coreRequires()
      if (reqs.length != 0)
        core += "\n" + reqs.join("\n")
      end

      if (clss.length == 0)
        if (core.length > 0)
          return core + "\n\n"
        else
          return ""
        end
      end

      classes = clss.map{ |clsName|
        cls = exposer.allMetaData.findClass(clsName)
        yield(cls.name, resolver.pathFor(cls))
        }.join("\n")

      if (classes.length > 0)
        classes += "\n"
      end

      classes += core

      if (classes.length > 0)
        classes += "\n\n"
      end
    end

  end
end