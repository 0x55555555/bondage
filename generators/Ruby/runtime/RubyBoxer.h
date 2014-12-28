#pragma once
#include "ruby.h"
#include "Crate/Type.h"
#include "Crate/Traits.h"

namespace bondage
{

namespace Ruby
{

class Boxer
  {
public:
  Boxer();

  class Box;
  typedef void (*Cleanup)(Boxer *ifc, Box *data);
  class Box
    {
  public:
    const Crate::Type *type;
    Cleanup cleanup;
    uint8_t data[1];
    };
  typedef Box *BoxedData;

  Box *unbox(VALUE t);

  const Crate::Type *getType(VALUE t);
  const Crate::Type *getType(Box *t);
  void *getMemory(VALUE *t);
  void *getMemory(Box *t);

  template <typename Traits, typename T>
      typename Traits::InitialiseTypes initialise(VALUE *result, const Crate::Type *type, const T *, Cleanup cleanup)
    {
    // the full size is the type size, with alignment, without the extra byte stores in Box.
    void *mem = xmalloc(Traits::TypeSize::value + Traits::TypeAlignment::value - 1);
    Box *box = new(mem) Box{ type, cleanup, { 0 } };

    *result = Data_Wrap_Struct(type->userData().klass, nullptr, free, box);

    return Traits::Initialised;
    //return Traits::AlreadyInitialised;
    }

  static void free(void *d)
    {
    Box* b = static_cast<Box*>(d);
    b->cleanup(instance(), b);
    }

  static Boxer *instance();
  };

template <typename T> class Caster;
}

}
