#pragma once

#include <QString>
#include <QtTest>

class RuntimeTest : public QObject
  {
  Q_OBJECT

private Q_SLOTS:
  void testTypeExistance();
  void testTypeCasting();
  void testFunctionExistance();
  void testStringLibrary();
  void testPrivateNonCopyableClass();
  };
