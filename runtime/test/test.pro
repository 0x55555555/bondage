#-------------------------------------------------
#
# Project created by QtCreator 2014-03-19T13:34:15
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = RuntimeTest
CONFIG   += console testcase
CONFIG   -= app_bundle

TEMPLATE = app

win32-msvc2012 {
  # needs enabling for msvc2013, which has no mkspec yet...
  # QMAKE_CXXFLAGS += /FS
  DEFINES += REFLECT_MACRO_IMPL
}

SOURCES += \
  RuntimeTest.cpp \
    ../src/Library.cpp \
    ../src/WrappedClass.cpp \
    ../src/CastHelper.cpp \
    ClassTypeTests.cpp \
    ../../test/testData/StringLibrary/autogen_baked/String.cpp \
    ../../test/testData/Generator/autogen_baked/Gen.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../include/bondage/RuntimeHelpers.h \
    ../include/bondage/RuntimeHelpersImpl.h \
    ../include/bondage/Function.h \
    ../include/bondage/FunctionBuilder.h \
    ../include/bondage/Library.h \
    ../include/bondage/WrappedClass.h \
    ../include/bondage/Bondage.h \
    ../include/bondage/CastHelper.h \
    PackHelper.h \
    CastHelper.Gen_GenCls.h \
    CrateTypeHelper.h \
    ../include/bondage/DerivableNonCleanedTraits.h \
    ../include/bondage/DerivableTraits.h \
    RuntimeTest.h \
    ../../test/testData/StringLibrary/autogen_baked/String.h \
    ../../test/testData/Generator/autogen_baked/Gen.h \
    autogen_String/String.h \
    autogen_Gen/Gen.h \
    ClassTypeTests.h

INCLUDEPATH += \
  ../include/ \
  ../Reflect/include/ \
  ../../test/testData/ \
  ../../test/testData/StringLibrary/ \
  ../../test/testData/Generator/ \
  .

macx-clang {
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.9
  QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
}

linux-clang {
  QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
  INCLUDEPATH += /usr/include/c++/4.6/ /usr/include/c++/4.6/x86_64-linux-gnu/32/
}

linux-g++ {
  QMAKE_CXXFLAGS += -std=c++0x
}

linux-g++-64 {
  QMAKE_CXXFLAGS += -std=c++0x
}
