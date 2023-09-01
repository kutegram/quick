QT += core declarative network xml
DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS
VERSION = 1.0.0
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

# TODO Migrate to QRC
folder_01.source = qml
folder_01.target = .
folder_02.source = img
folder_02.target = .
DEPLOYMENTFOLDERS = folder_01 folder_02

QML_IMPORT_PATH =

symbian:TARGET.UID3 = 0xE607720E

symbian:TARGET.CAPABILITY += NetworkServices ReadUserData WriteUserData

SOURCES += main.cpp \
    dialogsmodel.cpp \
    messagesmodel.cpp \
    messageeditor.cpp

HEADERS += \
    dialogsmodel.h \
    messagesmodel.h \
    messageeditor.h

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

include(libkg/libkg.pri)

include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()
