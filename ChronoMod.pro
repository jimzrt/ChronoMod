QT       += core gui core5compat

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

standalone {
    DEFINES += QUAZIP_STATIC
    INCLUDEPATH += "$$PWD/3rdparty/QuaZip/include/QuaZip-Qt6-1.2/quazip"
    LIBS += -L"$$PWD/3rdparty/QuaZip/lib"
} else {
    INCLUDEPATH += "/usr/include/QuaZip-Qt6-1.4/quazip"
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = myappico.ico

SOURCES += \
    ChronoCrypto.cpp \
    main.cpp \
    mainwindow.cpp \
    resourcebin.cpp \
    resourceentrymodel.cpp \
    resourceentryproxymodel.cpp

HEADERS += \
    ChronoCrypto.h \
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

LIBS += -lquazip1-qt6

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

