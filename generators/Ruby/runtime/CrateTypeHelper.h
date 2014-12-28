#pragma once
#include "ruby.h"

namespace Crate
{
class TypeUserData
  {
public:
  template <typename T> void initialise() { }

  bool isInitialised() const { return klass != 0; }

  VALUE klass = 0;
  };
}
