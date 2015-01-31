#pragma once
#include <functional>
#include <array>
#include "Crate/Type.h"

namespace Crate
{

namespace detail
{

template <typename T> struct TypeResolver<std::function<T>>
  {
  static const Type *find()
    {
    static Type t;
    t.initialise<std::function<T>>("function", nullptr);
    return &t;
    }
  };

}

}

namespace bondage
{

namespace Ruby
{

namespace detail
{

template<typename T> struct FunctionTraits;

/// \brief A utility to inform about parameters for std::functions
template<typename R, typename... Args>  struct FunctionTraits<std::function<R(Args...)>>
  {
  static const size_t ArgumentCount = sizeof...(Args);

  typedef R ResultType;

  template <size_t i> struct Arg
    {
    typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
  };

struct GlobalVALUE
  {
  GlobalVALUE()
      : m_val(0)
    {
    }
  GlobalVALUE(const GlobalVALUE &v)
      : GlobalVALUE()
    {
    assign(v.m_val);
    }
  ~GlobalVALUE()
    {
    clear();
    }
  GlobalVALUE &operator=(const GlobalVALUE &v)
    {
    clear();
    assign(v.m_val);
    return *this;
    }

  void assign(VALUE val)
    {
    REFLECT_ASSERT(!m_val);
    m_val = val;
    rb_gc_register_address(&m_val);
    }

  void clear()
    {
    if (m_val)
      {
      rb_gc_unregister_address(&m_val);
      m_val = 0;
      }
    }

  operator VALUE()
    {
    return m_val;
    }

private:
  VALUE m_val;
  };

template <typename Result> struct CallbackReturnHelper
  {
  static Result pack(Boxer *boxer, VALUE result)
    {
    return Caster<Result>::cast(boxer, result);
    }
  };

template <> struct CallbackReturnHelper<void>
  {
  static void pack(Boxer *, VALUE)
    {
    }
  };

/// \brief We wrap this class inside the std::function returned on packing
/// it contains all the required data to call the ruby value.
template <typename Result> class Call
  {
public:
  Call(Boxer *b, VALUE val)
      : boxer(b)
    {
    proc.assign(val);
    }

  struct CallArguments
    {
    CallArguments()
        : threw(false)
      {
      }

    bool threw;
    Reflect::CallbackException exception;

    std::array<detail::GlobalVALUE, FunctionTraits<Result>::ArgumentCount> arguments;
    Call *call;
    };

  template <typename... Args> typename FunctionTraits<Result>::ResultType operator()(Args &&...args)
    {
    typedef VALUE (*RubyFunction)(...);

    CallArguments rubyArgs;
    rubyArgs.call = this;

    extractArguments(rubyArgs.arguments.data(), std::forward<Args&&>(args)...);

    // this is adapted with confusion from https://gist.github.com/ammar/2787174
    // basically were trying to invoke doCall and catch any ruby errors that occur with doRescue.
    // The confusing part comes when casting everything to a VALUE...?
    VALUE result = rb_rescue2((RubyFunction)doCall, (VALUE)&rubyArgs,
                        (RubyFunction)doRescue, (VALUE)&rubyArgs,
                        rb_eException, (VALUE) nullptr);

    if (rubyArgs.threw)
      {
      throw rubyArgs.exception;
      }

    return CallbackReturnHelper<typename FunctionTraits<Result>::ResultType>::pack(boxer, result);
    }

private:
  template <typename Arg, typename...Args> void extractArguments(detail::GlobalVALUE *v, Arg &&a, Args &...args)
    {
    detail::GlobalVALUE &arg = *v++;
    extractArgument(arg, std::forward<Arg&&>(a));
    extractArguments(v, std::forward<Args&&>(args)...);
    }
  void extractArguments(detail::GlobalVALUE *) { }

  template <typename Arg> void extractArgument(detail::GlobalVALUE &gVal, Arg &&arg)
    {
    VALUE val;
    Caster<Arg>::pack(boxer, &val, arg);
    gVal.assign(val);
    }


  static VALUE doRescue(VALUE data, VALUE exception_object)
    {
    CallArguments *args = (CallArguments*)data;

    VALUE problem = rb_funcall2(exception_object, rb_intern("message"), 0, 0);
    VALUE backtrace = rb_funcall2(exception_object, rb_intern("backtrace"), 0, 0);
    std::string problemStr(RSTRING_PTR(problem), RSTRING_LEN(problem));

    std::string backtraceStr;
    for (int i = 0; i < RARRAY_LEN(backtrace); ++i)
      {
      VALUE el = RARRAY_PTR(backtrace)[i];
      std::string frame(RSTRING_PTR(el), RSTRING_LEN(el));
      backtraceStr += frame + "\n";
      }

    args->threw = true;
    args->exception = Reflect::CallbackException(std::string(rb_obj_classname(exception_object)) + ": " + problemStr + "\n" + backtraceStr);
    return Qnil;
    }

  static VALUE doCall(VALUE data)
    {
    CallArguments *args = (CallArguments*)data;

    return rb_funcall2(args->call->proc, rb_intern("call"), args->arguments.size(), (VALUE*)args->arguments.data());
    }

  Boxer *boxer;
  GlobalVALUE proc;
  };
}

/// \brief the bondage caster for a std::function
/// \note this doesn't support packing functions as return arguments, only input arguments
template <typename T> class Caster<std::__1::function<T> &>
  {
public:
  typedef typename std::function<T> Result;

  /// \brief Find if we can cast this [val]
  static bool canCast(Boxer *, VALUE val)
    {
    return TYPE(val) == T_DATA &&
      RTYPEDDATA_P(val) &&
      strcmp(RTYPEDDATA_TYPE(val)->wrap_struct_name, "proc") == 0;
    }

  /// \brief Cast this [val] as a std::function<T>
  static Result cast(Boxer *b, VALUE val)
    {
    if (!canCast(b, val))
      {
      throw Crate::TypeException(Crate::findType<std::__1::function<T>>(), b->getType(val));
      }

    return Result(detail::Call<Result>(b, val));
    }
  };

}

}
