
module Script

  class SignatureGenerator

    def generate(cls, fn, args, returnTypes)
      # Find the list of arguments with type then name, comma separated
      argString = formatArguments(args)

      # Find a list of return types, comma separated
      retString = formatReturnArguments(returnTypes)

      if (retString.length == 0)
        retString = formatType(nil)
      end

      # Extract signature
      callConv = (fn.static || fn.isConstructor) ? staticCallConvention() : methodCallConvention()

      return "#{retString} #{cls.name}#{callConv}#{fn.name}(#{argString})"
    end

    def formatArguments(args)
      return args.length.times.map{ |i|
        arg = args[i]
        argName = arg.name.length != 0 ? arg.name : "arg#{i+1}"

        next "#{formatType(arg.type)} #{argName}"
      }.join(", ")
    end

    def formatReturnArguments(types)
      return types.map{ |t| formatType(t) }.join(", ")
    end

    # Format [type], a Type instance, in a way lua users can understand
    def formatType(type)
      # pointers and references are stripped (after strings!)
      if(type != nil && !type.isStringLiteral() && (type.isPointer() || type.isLValueReference() || type.isLValueReference()))
        return formatType(type.pointeeType())
      end

      return scriptType(type)
    end
  end
end