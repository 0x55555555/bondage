#pragma once
#include "ruby.h"

namespace bondage
{
namespace Ruby
{
class Box;
class Boxer;
}
}

namespace Crate
{
class TypeUserData
  {
public:
  template <typename T> void initialise();

  bool isInitialised() const { return klass != 0; }

  typedef void (*Cleanup)(bondage::Ruby::Boxer *ifc, bondage::Ruby::Box *data);
  Cleanup cleanup;
  VALUE klass = 0;
  };
}
