#pragma once
#include "ruby.h"
#include "Crate/Type.h"
#include "Crate/Traits.h"
#include <unordered_map>

namespace bondage
{

namespace Ruby
{

class Box
  {
public:
  const Crate::Type *type;
  uint8_t data[1];
  };

class Boxer
  {
public:
  Boxer();
  ~Boxer();

  typedef Crate::TypeUserData::Cleanup Cleanup;
  typedef Box *BoxedData;

  Box *unbox(VALUE t);

  const Crate::Type *getType(VALUE t);
  const Crate::Type *getType(Box *t);
  void *getMemory(VALUE *t);
  void *getMemory(Box *t);

  template <typename Traits, typename T>
      typename Traits::InitialiseTypes initialise(VALUE *result, const Crate::Type *type, const T *t, Cleanup cleanup)
    {
    if (!Traits::Managed::value)
      {
      createClass<Traits>(result, type, t, cleanup);
      return Traits::Initialised;
      }

    auto foundClass = findClass(t, result);
    if (!foundClass)
      {
      createClass<Traits>(result, type, t, cleanup);
      return Traits::Initialised;
      }

    return Traits::AlreadyInitialised;
    }

  template <typename Traits> void createClass(VALUE *result, const Crate::Type *type, const void *t, Cleanup)
    {
    // the full size is the type size, with alignment, without the extra byte stores in Box.
    void *mem = xmalloc(sizeof(Box) + Traits::TypeSize::value + Traits::TypeAlignment::value - 1);

    Box *box = new(mem) Box{ type, { 0 } };

    *result = Data_Wrap_Struct(type->userData().klass, nullptr, free, box);

    if (Traits::Managed::value)
      {
      _values[t] = *result;
      _allocations[mem] = t;
      }
    }

  bool findClass(const void *ptr, VALUE *result)
    {
    auto fnd = _values.find(ptr);
    if (fnd != _values.end())
      {
      *result = fnd->second;
      return true;
      }

    return false;
    }

  static void free(void *d)
    {
    Box* b = static_cast<Box*>(d);

    auto inst = instance();

    auto val = inst->_allocations.find(d);
    if (val != inst->_allocations.end())
      {
      instance()->_allocations.erase(d);
      instance()->_values.erase(val->second);
      }

    b->type->userData().cleanup(instance(), b);
    }

  static Boxer *instance();

private:
  // map from allocated holders to class instances
  std::unordered_map<const void *, const void *> _allocations;
  // map from class instances to values
  std::unordered_map<const void *, VALUE> _values;
  };


template <typename T> class Caster;
}

}

template <typename T> void Crate::TypeUserData::initialise()
  {
  cleanup = Traits<T>::template cleanup<bondage::Ruby::Boxer>;
  }
