require_relative "../../exposer/ParsedLibrary.rb"
require_relative "../GeneratorHelper.rb"
require_relative "../../exposer/FunctionVisitor.rb"

require_relative "FunctionGenerator.rb"
require_relative "ClassGenerator.rb"

require 'pathname'

module CPP

  class LibraryGenerator
    
    class DerivedClass
      def initialize(literalName, path, rootPath, distance)
        @literalName = literalName
        @path = path
        @rootPath = rootPath
        @distance = distance
      end

      attr_reader :literalName, :path, :rootPath, :distance
    end

    def initialize(headerHelper)
      @header = ""
      @source = ""
      @headerHelper = headerHelper
    end

    attr_reader :header, :source

    def headerPath(library)
      return "#{library.autogenPath}/#{library.name}.h"
    end

    def sourcePath(library)
      return "#{library.autogenPath}/#{library.name}.cpp"
    end

    def generate(visitor, exposer)
      library = visitor.library
      rootNs = visitor.getExposedNamespace()
      setLibrary(library)


      files = Set.new
      derivedClasses = Set.new
      libraryName = "g_bondage_library_#{library.name}"

      classHeaders, classSources, clsGen = generateClasses(
        exposer,
        files,
        derivedClasses,
        libraryName)

      generateFiles(
        libraryName,
        library,
        exposer,
        rootNs,
        files,
        classHeaders,
        classSources,
        clsGen,
        derivedClasses)
    end

  private
    def generateFiles(
        libraryName,
        library,
        exposer,
        rootNs,
        files,
        clsHead,
        clsSrc,
        clsGen,
        derivedClasses)
      @header = @headerHelper.filePreamble("//") + "\n\n" +
        generateLibraryHeader(libraryName, library, exposer, rootNs, files) +
        "\n\n" + clsHead.join("\n") + "\n"

      @source = @headerHelper.filePreamble("//") + "\n" +
        includes(library) + 
        generateLibrarySource(libraryName, library, exposer, rootNs, files) +
        "\n\n\n" + clsSrc.join("\n\n\n") +
        "\n\n" + generateDerivedCasts(clsGen, derivedClasses)
    end

    def generateIncludePath(libraryfile)
      return Pathname.new(libraryfile).relative_path_from(@libraryIncludePath).cleanpath
    end

    def generateInclude(libraryfile)
      return "#include \"#{generateIncludePath(libraryfile)}\""
    end

    def coreIncludeFiles(library)
      sourcefiles = [ TYPE_NAMESPACE + "/RuntimeHelpersImpl.h", "utility", "tuple" ]

      library.dependencies.each{ |l| sourcefiles << generateIncludePath(headerPath(l)) }
      return sourcefiles
    end

    def includes(library)
      return generateInclude(headerPath(library)) + "\n" +
       coreIncludeFiles(library).map{ |f| "#include \"#{f}\"" }.join("\n") + "\n\n\n"
    end

    def generateClasses(exposer, files, derivedClasses, libraryName)
      clsGen = ClassGenerator.new

      classHeaders = []
      classSources = []

      exposer.exposedMetaData.types.each do |path, cls|
        if (cls.type == :class && cls.fullyExposed)

          generateClass(
            clsGen, 
            path, 
            cls, 
            classHeaders, 
            classSources, 
            exposer, 
            files, 
            derivedClasses, 
            libraryName)
        end
      end

      return classHeaders, classSources, clsGen
    end

    def generateClass(
        clsGen, 
        path, 
        cls, 
        classHeaders, 
        classSources, 
        exposer, 
        files, 
        derivedClasses, 
        libraryName)

      clsGen.reset()
      clsGen.generate(exposer, cls, libraryName)
      classHeaders << clsGen.interface
      classSources << clsGen.implementation

      files << cls.parsed.primaryFile

      if (cls.parentClass)
        rootPath, distance = clsGen.findRootClass(cls)

        derivedClasses << DerivedClass.new(clsGen.wrapperName, path, rootPath, distance)
      end
    end

    def generateLibraryHeader(libraryName, library, exposer, rootNs, files)
      raise "Invalid root namespace for #{library.name}." unless rootNs

      files = files | Set.new(@headerHelper.requiredIncludes(library))

      includes = files.map{ |path| generateInclude(path) }.join("\n")

      return "#{includes}\n#include \"#{TYPE_NAMESPACE}/RuntimeHelpers.h\"

namespace #{library.name}
{
#{library.exportMacro} const bondage::Library &bindings();
}"
    end

    def generateLibrarySource(libraryName, library, exposer, rootNs, files)
      fnGen = CPP::FunctionGenerator.new("", "  ")
      methods, extraMethods = fnGen.gatherFunctions(rootNs, exposer)

      methodsLiteral, methodsArray, extraMethodSource = fnGen.generateFunctionArray(methods, extraMethods, libraryName)

      return "using namespace #{library.namespaceName};\n\n#{extraMethodSource}#{methodsArray}
bondage::Library #{libraryName}(
  \"#{library.name}\",
  #{methodsLiteral},
  #{methods.length});
namespace #{library.name}
{
const bondage::Library &bindings()
{
  return #{libraryName};
}
}"
    end


    def generateDerivedCasts(clsGen, clss)
      byRoots = { }

      clss.each do |classData|
        arr = byRoots[classData.rootPath]
        if (!arr)
          arr = []
          byRoots[classData.rootPath] = arr
        end

        arr << classData
      end

      output = ""
      byRoots.each do |root, classes|
        output += generateDerivedCast(root, classes) + "\n\n"
      end

      return output
    end

    def generateDerivedCast(root, classes)
      rootNiceName = root.sub("::", "").gsub("::", "_")
      functionName = "#{@library.name}_#{rootNiceName}_caster"
      output = "#include \"CastHelper.#{rootNiceName}.h\"\n\n"
      output += "const bondage::WrappedClass *#{functionName}(const void *vPtr)\n{\n"

      output += "  auto ptr = static_cast<const #{root}*>(vPtr);\n\n"

      classes.sort_by{ |a| a.distance }.reverse.each do |derived|
        output += "  if (Crate::CastHelper< #{root}, #{derived.path} >::canCast(ptr))\n  {\n    return &#{derived.literalName};\n  }\n"
      end

      output += "  return nullptr;"

      output += "\n}\n\n"

      output += "bondage::CastHelperLibrary g_#{functionName}(bondage::WrappedClassFinder< #{root} >::castHelper(), #{functionName});"
      return output
    end

    def setLibrary(lib)
      @library = lib

      rootIncludePath = lib.root
      if (lib.includePaths.length > 0)
        rootIncludePath = lib.includePaths[0]
      end

      @libraryIncludePath = Pathname.new(rootIncludePath)
    end
  end
end
