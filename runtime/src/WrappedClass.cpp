#include "bondage/WrappedClass.h"
#include "bondage/Library.h"

namespace bondage
{

WrappedClass::WrappedClass(Library &owningLib, const Crate::Type *type, const Function *fn, std::size_t count)
    : m_type(type),
      m_functions(fn),
      m_functionCount(count),
      m_next(nullptr)
  {
  REFLECT_ASSERT(type);
  owningLib.registerClass(this);
  }

const Crate::Type &WrappedClass::type() const
  {
  return *m_type;
  }

void WrappedClass::setNext(WrappedClass *cls)
  {
  REFLECT_ASSERT(!m_next);
  m_next = cls;
  }

}
