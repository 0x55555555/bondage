// Copyright me, fool. No, copying and stuff.
//
// This file is auto generated, do not change it!
//
#include "autogen_String/String.h"
#include "bondage/RuntimeHelpersImpl.h"
#include "utility"
#include "tuple"
#include "StringLibrary.h"


using namespace String;


struct String_quote_overload0_t : Reflect::FunctionCall<Reflect::FunctionSignature< void(*)(String::StringCls &) >, &::String::quote, bondage::FunctionCaller> { };

const bondage::Function g_bondage_library_String_methods[] = {
  bondage::FunctionBuilder::build<
    String_quote_overload0_t
    >("quote")
};


bondage::Library g_bondage_library_String(
  "String",
  g_bondage_library_String_methods,
  1);
namespace String
{
const bondage::Library &bindings()
{
  return g_bondage_library_String;
}
}


// Exposing class ::String::StringCls
struct String_StringCls_append_overload0_t : Reflect::FunctionCall<Reflect::FunctionSignature< void(::String::StringCls::*)(const char *) >, &::String::StringCls::append, bondage::FunctionCaller> { };
struct String_StringCls_append_overload1_t : Reflect::FunctionCall<Reflect::FunctionSignature< void(::String::StringCls::*)(int) >, &::String::StringCls::append, bondage::FunctionCaller> { };
struct String_StringCls_append_overload3_t : Reflect::FunctionCall<Reflect::FunctionSignature< void(::String::StringCls::*)(int, int) >, &::String::StringCls::append, bondage::FunctionCaller> { };
struct StringCls_append_overload_2 : Reflect::FunctionArgCountSelectorBlock<2, Reflect::FunctionArgumentTypeSelector<
      String_StringCls_append_overload0_t,
      String_StringCls_append_overload1_t
      > > { };
struct StringCls_append_overload_3 : Reflect::FunctionArgCountSelectorBlock<3,
      String_StringCls_append_overload3_t
      > { };
struct StringCls_append_overload : Reflect::FunctionArgumentCountSelector<
    StringCls_append_overload_2,
    StringCls_append_overload_3
    > { };
struct String_StringCls_create_overload0_t : Reflect::FunctionCall<Reflect::FunctionSignature< String::StringCls(*)(const char *) >, &::String::StringCls::create, bondage::FunctionCaller> { };
struct String_StringCls_toUpper_overload0_t : Reflect::FunctionCall<Reflect::FunctionSignature< String::StringCls(::String::StringCls::*)() >, &::String::StringCls::toUpper, bondage::FunctionCaller> { };

const bondage::Function String_StringCls_methods[] = {
  bondage::FunctionBuilder::buildOverload< StringCls_append_overload >("append"),
  bondage::FunctionBuilder::build<
    String_StringCls_create_overload0_t
    >("create"),
  bondage::FunctionBuilder::build<
    String_StringCls_toUpper_overload0_t
    >("toUpper")
};


BONDAGE_IMPLEMENT_EXPOSED_CLASS(
  String_StringCls,
  g_bondage_library_String,
  ::String,
  StringCls,
  void,
  String_StringCls_methods,
  3);



