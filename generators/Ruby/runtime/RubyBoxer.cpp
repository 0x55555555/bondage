#include "RubyBoxer.h"
#include "Crate/EmbeddedTypes.h"
#include "assert.h"

#define DUMMY_TYPE(type, name) \
  template <> struct Crate::detail::TypeResolver<type> { \
  static const Crate::Type *find() { static Crate::Type t; t.initialise<type>(name, nullptr); return &t; } };

class DummyModule {};
DUMMY_TYPE(DummyModule, "Module");

class DummyUnknown {};
DUMMY_TYPE(DummyUnknown, "Unknown");

#undef DUMMY_TYPE

namespace bondage
{

namespace Ruby
{

Boxer::Boxer()
  {
  }

const Crate::Type *Boxer::getType(VALUE t)
  {
  auto type = TYPE(t);

  if (type == T_NONE || type == T_NIL)
    {
    return Crate::findType<void>();
    }
  else if (type == T_FLOAT)
    {
    return Crate::findType<float>();
    }
  else if (type == T_FIXNUM)
    {
    return Crate::findType<int>();
    }
  else if (type == T_STRING || type == T_SYMBOL)
    {
    return Crate::findType<const char *>();
    }
  else if (type == T_TRUE || type == T_FALSE)
    {
    return Crate::findType<bool>();
    }
  else if(type == T_DATA)
    {
    Box *b = unbox(t);
    return getType(b);
    }
  else if(type == T_MODULE)
    {
    return Crate::findType<DummyModule>();
    }
  else
    {
    return Crate::findType<DummyUnknown>();
    }

#define T_OBJECT RUBY_T_OBJECT
#define T_CLASS  RUBY_T_CLASS
#define T_ICLASS RUBY_T_ICLASS
#define T_REGEXP RUBY_T_REGEXP
#define T_ARRAY  RUBY_T_ARRAY
#define T_HASH   RUBY_T_HASH
#define T_STRUCT RUBY_T_STRUCT
#define T_BIGNUM RUBY_T_BIGNUM
#define T_FILE   RUBY_T_FILE
#define T_MATCH  RUBY_T_MATCH
#define T_RATIONAL RUBY_T_RATIONAL
#define T_COMPLEX RUBY_T_COMPLEX
#define T_UNDEF  RUBY_T_UNDEF
#define T_NODE   RUBY_T_NODE
#define T_ZOMBIE RUBY_T_ZOMBIE
#define T_MASK   RUBY_T_MASK

  return nullptr;
  }

const Crate::Type *Boxer::getType(Box *t)
  {
  assert(t);
  return t->type;
  }

void *Boxer::getMemory(VALUE *t)
  {
  return getMemory(unbox(*t));
  }

void *Boxer::getMemory(Box *t)
  {
  return t->data;
  }

Boxer::Box *Boxer::unbox(VALUE t)
  {
  Box *b = nullptr;
  Data_Get_Struct(t, Box, b);
  return b;
  }

Boxer *Boxer::instance()
  {
  static Boxer b;

  return &b;
  }

}
}
