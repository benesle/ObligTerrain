#-------------------------------------------------
#
# Project created by QtCreator 2019-10-15T04:01:33
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = ObligTerrain

INCLUDEPATH += ./GSL
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
        GSL/gsl_math.cpp \
        GSL/matrix2x2.cpp \
        GSL/matrix3x3.cpp \
        GSL/matrix4x4.cpp \
        GSL/vector2d.cpp \
        GSL/vector3d.cpp \
        GSL/vector4d.cpp \
        LAS/las2txt_normalized.cpp \
        camera.cpp \
        input.cpp \
        mainwindow.cpp \
        octahedronball.cpp \
        renderwindow.cpp \
        shader.cpp \
        texture.cpp \
        triangle.cpp \
        trianglesurface.cpp \
        vertex.cpp \
        visualobject.cpp \
        xyz.cpp

HEADERS += \
        GSL/gsl_math.h \
        GSL/math_constants.h \
        GSL/matrix2x2.h \
        GSL/matrix3x3.h \
        GSL/matrix4x4.h \
        GSL/scene.h \
        GSL/vector2d.h \
        GSL/vector3d.h \
        GSL/vector4d.h \
        LAS/lasloader.h \
        camera.h \
        gltypes.h \
        innpch.h \
        input.h \
        mainwindow.h \
        octahedronball.h \
        renderwindow.cpp.autosave \
        renderwindow.h \
        shader.h \
        texture.h \
        triangle.h \
        trianglesurface.h \
        ui_mainwindow.h \
        vertex.h \
        visualobject.h \
        xyz.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Shaders/phongshader.frag \
    Shaders/phongshader.vert \
    Shaders/plainshader.frag \
    Shaders/plainshader.vert \
    Shaders/textureshader.frag \
    Shaders/textureshader.vert \
    plainfragment.frag \
    plainvertex.vert \
    texturefragmet.frag \
    texturevertex.vert
