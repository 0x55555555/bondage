#pragma once
#include "ruby.h"

#include <iostream>

namespace Crate
{
class TypeUserData
  {
public:
  template <typename T> void initialise()
    {
    //newFunction = newFunctionTyped<T>;
    }

  bool isInitialised() const { return klass != 0; }

  VALUE (*newFunction)(int count, VALUE *args, VALUE ths);
  VALUE (*constructor)(int count, VALUE *args, VALUE ths) = 0;
  VALUE klass = 0;

  /*template <typename T> static VALUE newFunctionTyped(int count, VALUE *args, VALUE ths)
    {
    //VALUE val = constructor(count, args, ths);
std::cout << "NEW!" << std::endl;
    (void)count;
    (void)args;
    (void)ths;
    //return val;
    }*/
  };
}
