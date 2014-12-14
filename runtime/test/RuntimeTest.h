#pragma once

#include <QString>
#include <QtTest>
#include "bondage/Library.h"
#include "bondage/WrappedClass.h"

class RuntimeTest : public QObject
  {
  Q_OBJECT

private Q_SLOTS:
  void testTypeExistance();
  void testTypeCasting();
  void testFunctionExistance();
  void testStringLibrary();
  void testClassTypes();
  };

template <typename T> struct Helper
  {
  typedef Crate::Traits<T> Traits;

  static std::unique_ptr<Reflect::example::Object> create(
      bondage::Builder::Boxer *boxer,
      T *data)
    {
    auto box = boxer->create<Traits>();
    Traits::box(boxer, box.get(), data);

    return box;
    }
  };
