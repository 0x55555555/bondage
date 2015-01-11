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
  void *getMemory(VALUE t);
  void *getMemory(Box *t);

  template <typename Traits, typename T>
      typename Traits::InitialiseTypes initialise(VALUE *result, const Crate::Type *type, const T *t, Cleanup cleanup)
    {
    if (!Traits::Managed::value)
      {
      createClass<Traits>(result, type, t, cleanup);
      return Traits::Initialised;
      }

    auto foundClass = findClass<Traits>(t, result);
    if (!foundClass)
      {
      createClass<Traits>(result, type, t, cleanup);
      return Traits::Initialised;
      }

    return Traits::AlreadyInitialised;
    }

  template <typename Traits, typename T>
      void createClass(VALUE *result, const Crate::Type *type, const T *t, Cleanup)
    {
    // the full size is the type size, with alignment, without the extra byte stores in Box.
    void *mem = xmalloc(sizeof(Box) + Traits::TypeSize::value + Traits::TypeAlignment::value - 1);

    Box *box = new(mem) Box{ type, { 0 } };

    *result = Data_Wrap_Struct(type->userData().klass, nullptr, free, box);

    if (Traits::Managed::value)
      {
      auto key = Traits::makeObjectKey(t);
      _values[key] = *result;
      _allocations[mem] = key;
      REFLECT_ASSERT(_allocations.size() == _values.size());
      }
    }

  template <typename Traits, typename T> bool findClass(const T *ptr, VALUE *result)
    {
    auto fnd = _values.find(Traits::makeObjectKey(ptr));
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
      REFLECT_ASSERT(instance()->_allocations.size() == instance()->_values.size());
      REFLECT_ASSERT(instance()->_allocations.find(d) != instance()->_allocations.end());
      REFLECT_ASSERT(instance()->_values.find(val->second) != instance()->_values.end());
      instance()->_values.erase(val->second);
      instance()->_allocations.erase(d);
      REFLECT_ASSERT(instance()->_allocations.size() == instance()->_values.size());
      }

    b->type->userData().cleanup(instance(), b);
    }

  int unboxEnum(VALUE v);
  void boxEnum(VALUE *v, int enumVal);

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
