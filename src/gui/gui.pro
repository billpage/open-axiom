## Copyright (C) 2011-2013 Gabriel Dos Reis.
## All rights reserved.
## Autoconf template file for the benefit of QMake 
## Written by Gabriel Dos Reis.

oa_targetdir = ../../x86_64-unknown-linux-gnu

OA_INC = $${oa_targetdir}/include
OA_LIB = -L../../x86_64-unknown-linux-gnu/lib -lOpenAxiom

KLF = /home/wspage/Downloads/bruceoutdoors-tiny-tex-bed47a5e2a7a

## We build in release mode.
CONFIG += release
## But, don't think we want an app bundle, yet.
macx {
  CONFIG -= app_bundle
}

macx-g++ {
  QMAKE_CFLAGS -= -Xarch_x86_64
  QMAKE_CFLAGS_X86_64 -= -Xarch_x86_64

  QMAKE_CXXFLAGS -= -Xarch_x86_64
  QMAKE_CXXFLAGS += -stdlib=libc++
  QMAKE_CXXFLAGS_X86_64 -= -Xarch_x86_64
  QMAKE_CXXFLAGS_X86_64 += -stdlib=libc++

  QMAKE_LFLAGS -= -Xarch_x86_64
  QMAKE_LFLAGS += -stdlib=libc++
  QMAKE_LFLAGS_X86_64 -= -Xarch_x86_64
  QMAKE_LFLAGS_X86_64 += -stdlib=libc++
}
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

## We are building a GUI application.
TEMPLATE = app

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

## Put the executable directly in the staged bin directory.
DESTDIR = ../../x86_64-unknown-linux-gnu/bin
TARGET = open-axiom

## Tell QT that sources are not in the build directory
VPATH += ../../../open-axiom/src/gui

## Our headers
HEADERS += server.h conversation.h main-window.h debate.h
INCLUDEPATH += $$OA_INC
INCLUDEPATH +=  ../../../open-axiom/src/gui
INCLUDEPATH += ../../../open-axiom/src/include
INCLUDEPATH += ../../config
DEPENDPATH += ../../../open-axiom/src/gui
INCLUDEPATH += $$KLF/klfbackend

## Source files
SOURCES += server.cc conversation.cc main-window.cc debate.cc main.cc

## Additional support libraries
LIBS += $$OA_LIB
LIBS += -L$$KLF/build-KLFBackend -lKLFBackend

## C++ compiler
QMAKE_CXX = g++
QMAKE_CXXFLAGS += -std=c++11  -Wno-mismatched-tags -Wno-string-plus-int
QMAKE_LINK = g++
