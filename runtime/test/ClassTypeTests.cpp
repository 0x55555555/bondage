#include "RuntimeTest.h"
#include "ClassTypeTests.h"
#include "bondage/RuntimeHelpersImpl.h"

bondage::Library g_test_lib(
  "Test",
  nullptr,
  0);

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

