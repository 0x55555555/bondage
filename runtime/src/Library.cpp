#include "bondage/Library.h"

namespace bondage
{

Library::Library(const char *name, const Function *fn, std::size_t functionCount)
    : m_name(name),
      m_functions(fn),
      m_functionCount(functionCount)
  {
  }

void Library::registerClass(WrappedClass *cls)
  {
  if (!m_first)
    {
    m_first = m_last = cls;
    return;
    }

  auto lst = m_last;
  m_last = cls;

  REFLECT_ASSERT(lst);
  lst->setNext(m_last);
  }


ClassWalker::ClassWalker(const Library &l)
    : m_library(&l)
  {
  }

ClassWalker::iterator::iterator(const WrappedClass *cls)
    : m_cls(cls)
  {
  }

const WrappedClass *ClassWalker::iterator::operator*() const
  {
  return m_cls;
  }

bool ClassWalker::iterator::operator!=(const iterator &i) const
  {
  return m_cls != i.m_cls;
  }

ClassWalker::iterator& ClassWalker::iterator::operator++()
  {
  REFLECT_ASSERT(m_cls);
  m_cls = m_cls->next();
  return *this;
  }

ClassWalker::iterator ClassWalker::begin()
  {
  return iterator(m_library->firstClass());
  }

ClassWalker::iterator ClassWalker::end()
  {
  return iterator(nullptr);
  }

}
