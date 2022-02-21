QT       += core gui core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QUAZIP_STATIC
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = myappico.ico

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    resourcebin.cpp \
    resourceentrymodel.cpp \
    resourceentryproxymodel.cpp

HEADERS += \
    Patch.h \
    WorkerThread.h \
    mainwindow.h \
    resourcebin.h \
    resourceentry.h \
    resourceentrymodel.h \
    resourceentryproxymodel.h

FORMS += \
    mainwindow.ui

RESOURCES += qdarkstyle/dark/style.qrc


INCLUDEPATH += "$$PWD/3rdparty/QuaZip/include/QuaZip-Qt6-1.2/quazip"
LIBS += -L"$$PWD/3rdparty/QuaZip/lib" -lquazip1-qt6

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

