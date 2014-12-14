#pragma once
#include <type_traits>

// Basic class definition - should be pod
struct POD
  {
  int a;
  };
static_assert(std::is_pod<POD>::value, "POD should be pod.");

class Copyable
  {
public:
  Copyable() { }
  Copyable(const Copyable &)  { }
  Copyable &operator=(const Copyable &) { return *this; }
  ~Copyable() { }

  friend class RuntimeTest;
  };
static_assert(std::is_pod<POD>::value, "POD should be pod.");
static_assert(std::is_constructible<POD>::value, "Should be constructible");
static_assert(std::is_copy_assignable<POD>::value, "Should be copyable");
static_assert(std::is_copy_constructible<POD>::value, "Should be copyable");

class Managed
  {
public:
  ~Managed() { }

private:
  Managed() { }
  Managed(const Managed &)  { }
  Managed &operator=(const Managed &) { return *this; }

  friend class RuntimeTest;
  };
static_assert(std::is_destructible<Managed>::value, "Should be destructible");
static_assert(!std::is_constructible<Managed>::value, "Should not be constructible");
static_assert(!std::is_copy_assignable<Managed>::value, "Should be copyable");
static_assert(!std::is_copy_constructible<Managed>::value, "Should be copyable");

class Unmanaged
  {
private:
  Unmanaged() { }
  Unmanaged(const Unmanaged &)  { }
  Unmanaged &operator=(const Unmanaged &) { return *this; }
  ~Unmanaged() { }

  friend class RuntimeTest;
  };
static_assert(!std::is_destructible<Unmanaged>::value, "Should not be destructible");
static_assert(!std::is_constructible<Unmanaged>::value, "Should not be constructible");
static_assert(!std::is_copy_assignable<Unmanaged>::value, "Should be copyable");
static_assert(!std::is_copy_constructible<Unmanaged>::value, "Should be copyable");

class Derivable
  {
public:
  ~Derivable() { }

protected:
  Derivable() { }
  Derivable(const Derivable &)  { }
  Derivable &operator=(const Derivable &) { return *this; }

  friend class RuntimeTest;
  };

class Derived : public Derivable
  {
public:
  ~Derived() { }

protected:
  Derived() { }
  Derived(const Derived &d) : Derivable(d) { }
  Derived &operator=(const Derived &) { return *this; }

  friend class RuntimeTest;
  };

static_assert(std::is_destructible<Managed>::value, "Should be destructible");
static_assert(!std::is_constructible<Managed>::value, "Should not be constructible");
static_assert(!std::is_copy_assignable<Managed>::value, "Should be copyable");
static_assert(!std::is_copy_constructible<Managed>::value, "Should be copyable");

#include "bondage/RuntimeHelpers.h"

namespace Test
{
const bondage::Library &bindings();
}

#define DUMMY_EXPORT

BONDAGE_EXPOSED_CLASS_COPYABLE(DUMMY_EXPORT, Copyable)
BONDAGE_EXPOSED_CLASS_MANAGED(DUMMY_EXPORT, Managed)
BONDAGE_EXPOSED_CLASS_UNMANAGED(DUMMY_EXPORT, Unmanaged)


BONDAGE_EXPOSED_CLASS_DERIVABLE_MANAGED(DUMMY_EXPORT, Derivable)
BONDAGE_EXPOSED_CLASS_DERIVED_MANAGED(DUMMY_EXPORT, Derived, Derivable, Derivable)
