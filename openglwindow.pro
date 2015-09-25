include(openglwindow.pri)

SOURCES += \
    main.cpp \
    trianglewindow.cpp \
    terrain.cpp

target.path = $$[QT_INSTALL_EXAMPLES]/gui/openglwindow
INSTALLS += target

HEADERS += \
    trianglewindow.h \
    terrain.h

QT += opengl
