// Copyright me, fool. No, copying and stuff.
//
// This file is auto generated, do not change it!
//
#include "autogen_Enum/Enum.h"
#include "bondage/RuntimeHelpersImpl.h"
#include "utility"
#include "tuple"
#include "Enum.h"


using namespace Enum;


bondage::Library g_bondage_library_Enum(
  "Enum",
  nullptr,
  0);
namespace Enum
{
const bondage::Library &bindings()
{
  return g_bondage_library_Enum;
}
}


// Exposing class ::Enum::ExposedClass
struct Enum_ExposedClass_fn1_overload0_t : Reflect::FunctionCall<Reflect::FunctionSignature< void(::Enum::ExposedClass::*)(Enum::ExposedEnumStatic) >, &::Enum::ExposedClass::fn1, bondage::FunctionCaller> { };
struct Enum_ExposedClass_fn3_overload0_t : Reflect::FunctionCall<Reflect::FunctionSignature< void(::Enum::ExposedClass::*)(Enum::ExposedClass::ExposedEnum) >, &::Enum::ExposedClass::fn3, bondage::FunctionCaller> { };

const bondage::Function Enum_ExposedClass_methods[] = {
  bondage::FunctionBuilder::build<
    Enum_ExposedClass_fn1_overload0_t
    >("fn1"),
  bondage::FunctionBuilder::build<
    Enum_ExposedClass_fn3_overload0_t
    >("fn3")
};


BONDAGE_IMPLEMENT_EXPOSED_CLASS(
  Enum_ExposedClass,
  g_bondage_library_Enum,
  ::Enum,
  ExposedClass,
  void,
  Enum_ExposedClass_methods,
  2);



