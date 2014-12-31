#pragma once
#include "Crate/Traits.h"
#include "bondage/Library.h"
#include "bondage/CastHelper.h"
#include "bondage/DerivableTraits.h"
#include "bondage/DerivableNonCleanedTraits.h"

namespace bondage
{
class WrappedClass;
class CastHelper;
}

#ifdef __clang__
# define BONDAGE_SHARED_NAME std::__1::shared_ptr
# define BONDAGE_WEAK_SHARED_NAME std::__1::weak_ptr
# define BONDAGE_UNIQUE_NAME std::__1::unique_ptr
#else
# define BONDAGE_SHARED_NAME std::shared_ptr
# define BONDAGE_WEAK_SHARED_NAME std::weak_ptr
# define BONDAGE_UNIQUE_NAME std::unique_ptr
#endif

#define BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  namespace Crate { \
  namespace detail { \
  template <> struct EXP TypeResolver<CLS> { \
  static const Type *find(); }; } }

#define BONDAGE_CLASS_RESOLVER_FORWARD(CLS, FWD) \
  namespace Crate { \
  namespace detail { \
  template <> struct TypeResolver<CLS> : TypeResolver<FWD> { }; } }

#define BONDAGE_CLASS_UNDERIVABLE(EXP, CLS) namespace bondage { \
  template <> class EXP WrappedClassFinder<CLS> { public: \
    static const WrappedClass *findBase(); \
    static CastHelper &castHelper(); \
    static const WrappedClass *find(const void *) { return findBase(); } }; }

#define BONDAGE_CLASS_DERIVABLE(EXP, CLS) namespace bondage { \
  template <> class EXP WrappedClassFinder<CLS> { public: \
    static const WrappedClass *findBase(); \
    static CastHelper &castHelper(); \
    static const WrappedClass *find(const void *d) \
      { auto cls = castHelper().search(d); \
      if (cls) { return cls; } \
      return findBase(); } }; }

#define BONDAGE_CLASS_DERIVED(EXP, CLS, ROOT) namespace bondage { \
  template <> class EXP WrappedClassFinder<CLS> { public: \
    static const WrappedClass *findBase(); \
    static CastHelper &castHelper(); \
    static const WrappedClass *find(const void *p) \
      { return WrappedClassFinder<ROOT>::find(p); } }; }

#define BONDAGE_CLASS_CRATER(CLS, TRAITS) \
  namespace Crate { template <> class Traits<CLS> : public TRAITS<CLS> { }; }

#define BONDAGE_CLASS_SMART_POINTER_CRATER(CLS, POINTER) \
  namespace Crate { template <> class Traits<POINTER<CLS>> : public SmartPointerTraits<POINTER<CLS>> { }; } \
  namespace Crate { template <> class Traits<CLS> : public SmartPointerObjectTraits<CLS, SmartPointerTraits<POINTER<CLS>>> { }; }

#define BONDAGE_CLASS_SMART_WEAK_POINTER_CRATER(CLS, POINTER) \
  namespace Crate { template <> class Traits<POINTER<CLS>> : public WeakSmartPointerTraits<POINTER<CLS>> { }; } \
  namespace Crate { template <> class Traits<CLS> : public WeakSmartPointerObjectTraits<CLS, WeakSmartPointerTraits<POINTER<CLS>>> { }; }


#define BONDAGE_CLASS_CRATER_INVALID(CLS) \
  namespace Crate { template <> class Traits<CLS> { }; }

#define BONDAGE_EXPOSED_CLASS_COPYABLE(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_CRATER(CLS, CopyTraits)

#define BONDAGE_EXPOSED_CLASS_UNIQUE_POINTER(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER_FORWARD(BONDAGE_UNIQUE_NAME<CLS>, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, BONDAGE_UNIQUE_NAME<CLS>) \
  BONDAGE_CLASS_SMART_POINTER_CRATER(CLS, BONDAGE_UNIQUE_NAME)

#define BONDAGE_EXPOSED_CLASS_SHARED_POINTER(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER_FORWARD(BONDAGE_SHARED_NAME<CLS>, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, BONDAGE_SHARED_NAME<CLS>) \
  BONDAGE_CLASS_SMART_POINTER_CRATER(CLS, BONDAGE_SHARED_NAME)

#define BONDAGE_EXPOSED_CLASS_WEAK_POINTER(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER_FORWARD(BONDAGE_WEAK_SHARED_NAME<CLS>, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, BONDAGE_WEAK_SHARED_NAME<CLS>) \
  BONDAGE_CLASS_SMART_WEAK_POINTER_CRATER(CLS, BONDAGE_WEAK_SHARED_NAME)

#define BONDAGE_EXPOSED_CLASS_MANAGED(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_CRATER(CLS, ReferenceTraits)

#define BONDAGE_EXPOSED_CLASS_UNMANAGED(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_CRATER(CLS, ReferenceNonCleanedTraits)

#define BONDAGE_EXPOSED_CLASS_DERIVABLE_COPYABLE(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_UNDERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_CRATER(CLS, CopyTraits)

#define BONDAGE_EXPOSED_CLASS_DERIVABLE_MANAGED(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_DERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_CRATER(CLS, DerivableTraits)

#define BONDAGE_EXPOSED_CLASS_DERIVABLE_UNMANAGED(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_DERIVABLE(EXP, CLS) \
  BONDAGE_CLASS_CRATER(CLS, DerivableNonCleanedTraits)

#define BONDAGE_EXPOSED_DERIVED_CLASS(EXP, CLS, PARENT, ROOT) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  BONDAGE_CLASS_DERIVED(EXP, CLS, ROOT) \
  namespace Crate { template <> class Traits<CLS> : public DerivedTraits<CLS, PARENT, ROOT> { }; }

#define BONDAGE_EXPOSED_CLASS_DERIVED_MANAGED BONDAGE_EXPOSED_DERIVED_CLASS
#define BONDAGE_EXPOSED_CLASS_DERIVED_UNMANAGED BONDAGE_EXPOSED_DERIVED_CLASS
#define BONDAGE_EXPOSED_CLASS_DERIVED_COPYABLE(EXP, CLS, P, R) BONDAGE_EXPOSED_CLASS_COPYABLE(EXP, CLS)

#define BONDAGE_EXPOSED_DERIVED_PARTIAL_CLASS(EXP, CLS, PARENT, ROOT) \
  BONDAGE_CLASS_RESOLVER(EXP, CLS) \
  namespace Crate { template <> class Traits<CLS> : public DerivedTraits<CLS, PARENT, ROOT> { }; }

#define BONDAGE_EXPOSED_ENUM(EXP, CLS) \
  BONDAGE_CLASS_RESOLVER_FORWARD(CLS, unsigned int)\
  namespace Crate { template <> class Traits<CLS> : public EnumTraits<CLS> {}; }
