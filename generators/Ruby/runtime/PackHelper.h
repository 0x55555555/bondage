#pragma once
#include "ruby.h"
#include "Crate/Type.h"
#include "RubyBoxer.h"

#define EXPORT_DEBUGIFY

namespace bondage
{

class Builder
  {
public:
  typedef Ruby::Boxer Boxer;
  struct CallArgs
    {
    size_t thisAdjust;

    VALUE *args;
    size_t argCount;
    VALUE ths;

    enum
      {
      MaxResults = 10
      };
    VALUE results[MaxResults];
    size_t resultCount;

    Boxer *boxer;
    };
  typedef CallArgs *CallData;


  struct Result
    {
    typedef VALUE (*Signature)(int count, VALUE *args, VALUE self);
    Signature fn;
    };

  typedef Result::Signature Call;
  typedef bool (*CanCall)(CallData);

#if REFLECT_DESCRIPTIVE_EXCEPTIONS
  static std::string describeArguments(CallData args)
    {
    auto voidType = Crate::findType<void>();

    std::string argDesc;
    for (size_t i = 0; i < args->argCount; ++i)
      {
      if (i != 0)
        {
        argDesc += ", ";
        }
      argDesc += args->args[i] ? args->boxer->getType(args->args[i])->name() : voidType->name();
      }

    std::string result;
    if (args->ths)
      {
      result = args->boxer->getType(args->ths)->name();
      }
    else
      {
      result = voidType->name();
      }
    result += " ->( " + argDesc + " )";
    return result;
    }

  template <typename Fn> static std::string describeFunction()
    {
    typedef typename Fn::Helper Helper;
    return describeFunction<typename Helper::Class, typename Helper::Arguments>(0);
    }

  template <typename Arguments> class ArgHelper
    {
  public:
    ArgHelper(std::size_t start)
        : m_start(start)
      {
      }

    void append(std::size_t i, const Crate::Type *type)
      {
      if (i > m_start)
        {
        m_result += ", ";
        }

      m_result += type->name();
      }

    template <std::size_t Idx> bool visit()
      {
      typedef typename std::tuple_element<Idx, Arguments>::type Element;
      append(Idx, Crate::findType<Element>());

      return false;
      }

    std::size_t m_start;
    std::string m_result;
    };

  template <typename Class, typename Arguments> static std::string describeFunction(size_t argStart)
    {
    ArgHelper<Arguments> helper(argStart);
    Reflect::tupleEach<Arguments>(helper);

    return Crate::findType<Class>()->name() + " ->( " + helper.m_result + " )";
    }
#endif

  static std::size_t getArgumentCountWithThis(CallData args)
    {;
    return args->argCount + args->thisAdjust;
    }

  template <typename T> static T unpackThis(CallData args)
    {
    try
      {
      return Ruby::Caster<T>::cast(args->boxer, args->ths);
      }
    catch(const Crate::TypeException &type)
      {
      throw Crate::ThisException(type);
      }
    }

  template <typename T> static bool canUnpackThis(CallData args)
    {
    return Ruby::Caster<T>::canCast(args->boxer, args->ths);
    }

  template <typename Arg> struct ReturnType
    {
    typedef typename Ruby::Caster<Arg>::Result Type;
    };

  template <typename Arg>
      static typename Ruby::Caster<Arg>::Result unpackArgument(CallData data, bool, size_t i)
    {
    if (data->argCount <= i)
      {
      throw Reflect::ArgCountException(i, data->argCount);
      }

    try
      {
      return Ruby::Caster<Arg>::cast(data->boxer, data->args[i]);
      }
    catch(const Crate::TypeException &type)
      {
      throw Crate::ArgException(type, i);
      }
    }

  template <typename Arg>
      static bool canUnpackArgument(CallData data, bool, size_t i)
    {
    if (data->argCount <= i)
      {
      return false;
      }
    return Ruby::Caster<Arg>::canCast(data->boxer, data->args[i]);
    }

  template <typename Return, typename T> static void packReturn(CallData data, T &&result)
    {
    VALUE &b = data->results[data->resultCount++];

    Ruby::Caster<Return>::pack(data->boxer, &b, std::move(result));
    }

  template <typename Builder> static Result build()
    {
    Result r = { wrapCall<Builder> };
    return r;
    }

  template <typename Function, typename Builder> static VALUE wrapCall(int count, VALUE *args, VALUE ths)
    {
    const size_t thisAdjust = Builder::template Helper<Function>::Static::value ? 0 : 1; // Ruby always passes some kind of self
    CallArgs data = { thisAdjust, args, (size_t)count, ths, { }, 0, Boxer::instance() };
    try
      {
      Function::template call<Builder>(&data);
      }
    catch (const Reflect::CallException &e)
      {
      rb_raise(rb_eRuntimeError, "%s", e.what());
      }

    if (data.resultCount > 1)
      {
      return rb_ary_new4(data.resultCount, data.results);
      }
    else if(data.resultCount == 0)
      {
      return Qnil;
      }

    return data.results[0];
    }

  template <typename Function, typename Builder> static bool wrapCanCall(int count, VALUE *args, VALUE ths)
    {
    const size_t thisAdjust = Builder::template Static<Function>::value ? 0 : 1; // Ruby always passes some kind of self
    CallArgs data = { thisAdjust, args, (size_t)count, ths, { }, 0, Boxer::instance() };
    return Function::template canCall<Builder>(&data);
    }
  };

}

#include "RubyTypeCasters.h"
