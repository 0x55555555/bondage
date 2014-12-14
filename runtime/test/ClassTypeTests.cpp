#include "RuntimeTest.h"
#include "ClassTypeTests.h"
#include "bondage/RuntimeHelpersImpl.h"

Copyable &test_copyable_fn(Copyable &a) { return a; }
Managed &test_managed_fn(Managed &a) { return a; }
Unmanaged &test_unmanaged_fn(Unmanaged &a) { return a; }
Derivable &test_derivable_fn(Derivable &a) { return a; }
Derived &test_derived_fn(Derived &a) { return a; }

struct test_copyable : Reflect::FunctionCall<Reflect::FunctionSignature< Copyable &(*)(Copyable &) >, &test_copyable_fn, bondage::FunctionCaller> { };
struct test_managed : Reflect::FunctionCall<Reflect::FunctionSignature< Managed &(*)(Managed &) >, &test_managed_fn, bondage::FunctionCaller> { };
struct test_unmanaged : Reflect::FunctionCall<Reflect::FunctionSignature< Unmanaged &(*)(Unmanaged &) >, &test_unmanaged_fn, bondage::FunctionCaller> { };
struct test_derivable : Reflect::FunctionCall<Reflect::FunctionSignature< Derivable &(*)(Derivable &) >, &test_derivable_fn, bondage::FunctionCaller> { };
struct test_derived : Reflect::FunctionCall<Reflect::FunctionSignature< Derived &(*)(Derived &) >, &test_derived_fn, bondage::FunctionCaller> { };

const bondage::Function test_functions[] = {
  bondage::FunctionBuilder::build<test_copyable>("test_copyable"),
};

bondage::Library g_test_lib(
  "Test",
  test_functions,
  sizeof(test_functions)/sizeof(test_functions[0]));

namespace Test
{
const bondage::Library &bindings()
{
  return g_test_lib;
}
}

BONDAGE_IMPLEMENT_EXPOSED_CLASS(
  Test_Copyable,
  g_test_lib,
  ,
  Copyable,
  void,
  nullptr,
  0);

BONDAGE_IMPLEMENT_EXPOSED_CLASS(
  Test_Managed,
  g_test_lib,
  ,
  Managed,
  void,
  nullptr,
  0);

BONDAGE_IMPLEMENT_EXPOSED_CLASS(
  Test_Unmanaged,
  g_test_lib,
  ,
  Unmanaged,
  void,
  nullptr,
  0);

BONDAGE_IMPLEMENT_EXPOSED_CLASS(
  Test_Derived,
  g_test_lib,
  ,
  Derived,
  void,
  nullptr,
  0);

BONDAGE_IMPLEMENT_EXPOSED_CLASS(
  Test_Derivable,
  g_test_lib,
  ,
  Derivable,
  void,
  nullptr,
  0);

template <typename T> void testType(T *t)
  {
  bondage::Builder::Boxer boxer;

  auto ctorGen = Helper<T>::create(&boxer, t);
  }

void RuntimeTest::testClassTypes()
  {
  Copyable c;
  testType(&c);

  Managed *m = new Managed(); // this is deleted by boxer
  testType(m);

  Unmanaged u;
  testType(&u);

  Derivable *d = new Derivable(); // this is deleted by boxer
  testType(d);

  Derived *dd = new Derived(); // this is deleted by boxer
  testType(dd);
  }

