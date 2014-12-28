#include "ModuleBuilder.h"

typedef VALUE (*RubyFunction)(...);

void buildRubyClass(VALUE &rbLib, const bondage::WrappedClass *cls)
  {
  // We may have initialised this as a parent class earlier.
  auto &userData = const_cast<Crate::TypeUserData&>(cls->type().userData());
  if (userData.isInitialised())
    {
    return;
    }

  VALUE superRbCls = rb_cObject;
  auto parent = cls->type().parent();
  if (parent != Crate::findType<void>())
    {
    auto &rbData = parent->userData();
    if (!rbData.isInitialised())
      {
      buildRubyClass(rbLib, cls);
      }
    superRbCls = rbData.klass;
    assert(superRbCls);
    }

  VALUE rbCls = rb_define_class_under(rbLib, cls->type().name().data(), superRbCls);

  userData.klass = rbCls;

  const bondage::Function *constructor = nullptr;

  for (size_t i = 0; i < cls->functionCount(); ++i)
    {
    auto &fn = cls->function(i);

    const char *name = fn.name().data();
    if (fn.name() == cls->type().name())
      {
      constructor = &fn;
      }
    else
      {
      rb_define_method(rbCls, name, (RubyFunction)fn.getCallFunction(), -1);
      }
    }

  if (constructor)
    {
    rb_define_module_function(rbCls, "new", (RubyFunction)constructor->getCallFunction(), -1);
    }
  }

void buildRubyModule(VALUE &rbLib, const bondage::Library &lib)
  {
  rbLib = rb_define_module(lib.name().data());

  for (size_t i = 0; i < lib.functionCount(); ++i)
    {
    auto &fn = lib.function(i);

    rb_define_module_function(rbLib, fn.name().data(), (RubyFunction)fn.getCallFunction(), -1);
    }

  for (auto cls = lib.firstClass(); cls; cls = cls->next())
    {
    buildRubyClass(rbLib, cls);
    }
  }
