#pragma once
#include "bondage/Bondage.h"
#include "PackHelper.h"
#include "Crate/Traits.h"

namespace bondage
{

namespace Ruby
{

template <typename T, bool CanCast(VALUE), T Cast(VALUE), VALUE Pack(T)> class SimpleCaster
  {
public:
  typedef T Result;

  static bool canCast(Boxer *, VALUE val)
    {
    return CanCast(val);
    }

  static Result cast(Boxer *b, VALUE val)
    {
    if (!canCast(b, val))
      {
      throw Crate::TypeException(b->getType(val), Crate::findType<T>());
      }

    return (Result)Cast(val);
    }

  static void pack(Boxer *, VALUE *v, T result)
    {
    *v = Pack(result);
    }
  };

inline bool canCastNumber(VALUE v) { return RB_TYPE_P(v, T_FIXNUM) || RB_TYPE_P(v, T_FLOAT); }

inline VALUE fromFloat(float i) { return DBL2NUM(i); }
inline VALUE fromDouble(double i) { return DBL2NUM(i); }
inline double toDouble(VALUE v) { if (RB_TYPE_P(v, T_FIXNUM)) { return (double)FIX2LONG(v); } return DBL2NUM(v); }
inline float toFloat(VALUE v) { return (float)toDouble(v); }

inline VALUE fromLong(long i) { return LONG2FIX(i); }
inline VALUE fromInt(int i) { return INT2FIX(i); }
inline VALUE fromSizeT(size_t i) { return ULL2NUM(i); }
inline long toLong(VALUE v) { if (RB_TYPE_P(v, T_FIXNUM)) { return FIX2LONG(v); } return (long)DBL2NUM(v); }
inline int toInt(VALUE v) { return (int)toLong(v); }
inline size_t toSizeT(VALUE v) { return (size_t)toLong(v); }

inline bool canCastBool(VALUE v) { auto type = TYPE(v); return type == T_TRUE || type == T_FALSE; }
inline VALUE fromBool(bool i) { return i ? Qtrue : Qfalse; }
inline bool toBool(VALUE v) { return RTEST(v); }


template <> class Caster<int> : public SimpleCaster<int, canCastNumber, toInt, fromInt> { };
template <> class Caster<long> : public SimpleCaster<long, canCastNumber, toLong, fromLong> { };
template <> class Caster<size_t> : public SimpleCaster<size_t, canCastNumber, toSizeT, fromSizeT> { };
template <> class Caster<float> : public SimpleCaster<float, canCastNumber, toFloat, fromFloat> { };
template <> class Caster<double> : public SimpleCaster<double, canCastNumber, toDouble, fromDouble> { };
template <> class Caster<bool> : public SimpleCaster<bool, canCastBool, toBool, fromBool> { };

template <typename T> class PointerCaster
  {
public:
  typedef typename Crate::Traits<T>::UnboxResult Result;

  static bool canCast(Boxer *b, VALUE val)
    {
    return TYPE(val) == T_NIL || Crate::Traits<T>::canUnbox(b, val);
    }

  static Result cast(Boxer *b, VALUE val)
    {
    if (!canCast(b, val))
      {
      throw Crate::TypeException(Crate::findType<T>(), b->getType(val));
      }

    if (TYPE(val) == T_NIL)
      {
      return nullptr;
      }

    return Crate::Traits<T>::unbox(b, val);
    }

  static void pack(Boxer *box, VALUE *v, T *result)
    {
    if (!result)
      {
      *v = Qnil;
      return;
      }

    Crate::Traits<T>::box(box, v, result);
    }

  static void pack(Boxer *box, VALUE *v, const T *result)
    {
    if (!result)
      {
      *v = Qnil;
      return;
      }

    Crate::Traits<T>::box(box, v, result);
    }
  };

template <typename T> class Caster<T*> : public PointerCaster<T> { };


template <typename T> class ReferenceCaster : public Caster<T*>
  {
public:
  typedef decltype(*std::declval<typename Crate::Traits<T>::UnboxResult>()) Result;

  static Result cast(Boxer *b, VALUE val)
    {
    auto t = Caster<T*>::cast(b, val);
    if (!t)
      {
      throw Crate::TypeException(b->getType(val), Crate::findType<T>());
      }

    return *t;
    }
    
  static void pack(Boxer *box, VALUE *v, T &&result)
    {
    Caster<T*>::pack(box, v, &result);
    }

  static void pack(Boxer *box, VALUE *v, T &result)
    {
    Caster<T*>::pack(box, v, &result);
    }

  static void pack(Boxer *box, VALUE *v, const T &result)
    {
    Caster<T*>::pack(box, v, &result);
    }
  };

template <typename T> class Caster<T&> : public ReferenceCaster<T>
  {
  };

template <typename T> class Caster<std::shared_ptr<T>&> : public ReferenceCaster<std::shared_ptr<T>>
  {
public:
  typedef typename std::shared_ptr<T> Result;

  static Result cast(Boxer *b, VALUE val)
    {
    auto t = Caster<std::shared_ptr<T>*>::cast(b, val);
    if (!t)
      {
      return Result();
      }

    return *t;
    }
  };

template <typename T> class Caster<const T *> : public Caster<T *> { };
template <typename T> class Caster<const T &> : public Caster<T &> { };
template <typename T> class Caster : public Caster<const T &> { };

template <typename T> class Caster<std::shared_ptr<T>*> : public PointerCaster<std::shared_ptr<T>>
  {
public:
  static void pack(Boxer *box, VALUE *v, const std::shared_ptr<T> *result)
    {
    if (!result || !result->get())
      {
      *v = Qnil;
      return; 
      }

    Crate::Traits<std::shared_ptr<T>>::box(box, v, result);
    }
  };

template <typename T> class Caster<std::unique_ptr<T>> : public PointerCaster<std::unique_ptr<T>>
  {
public:
  static void pack(Boxer *box, VALUE *v, std::unique_ptr<T> *result)
    {
    if (!result || !result->get())
      {
      *v = Qnil;
      return;
      }

    Crate::Traits<std::unique_ptr<T>>::box(box, v, result);
    }
  };

inline void Boxer::boxEnum(VALUE *v, int enumVal)
  {
  Ruby::Caster<int>::pack(bondage::Builder::Boxer::instance(), v, enumVal);
  }

inline int Boxer::unboxEnum(VALUE v)
  {
  return Ruby::Caster<int>::cast(bondage::Builder::Boxer::instance(), v);
  }

inline int Boxer::canUnboxEnum(VALUE v)
  {
  return canCastNumber(v);
  }

}

}
