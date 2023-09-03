QT += core declarative network xml
DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT KG_NO_DEBUG KG_NO_INFO

QML_IMPORT_PATH =

win32:RC_FILE = kutegramquick.rc
macx:ICON = kutegramquick.icns
symbian:ICON = kutegramquick.svg

symbian:TARGET.UID3 = 0xE607720E
symbian:TARGET.CAPABILITY += NetworkServices ReadUserData WriteUserData

SOURCES += main.cpp \
    dialogsmodel.cpp \
    messagesmodel.cpp \
    messageeditor.cpp \
    avatardownloader.cpp

HEADERS += \
    dialogsmodel.h \
    messagesmodel.h \
    messageeditor.h \
    avatardownloader.h

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    qml/main.qml \
    qml/auth/Button.qml \
    qml/auth/CodePage.qml \
    qml/auth/IntroPage.qml \
    qml/auth/PhonePage.qml \
    qml/control/Drawer.qml \
    qml/control/DrawerButton.qml \
    qml/control/LineEdit.qml \
    qml/control/Spinner.qml \
    qml/control/Stack.qml \
    qml/control/TopBar.qml \
    qml/dialog/DialogItem.qml \
    qml/dialog/DialogPage.qml \
    qml/dialog/FolderItem.qml \
    qml/message/MessageDocument.qml \
    qml/message/MessageEdit.qml \
    qml/message/MessageImage.qml \
    qml/message/MessageItem.qml \
    qml/message/MessagePage.qml \
    qml/control/SnackBar.qml

RESOURCES += \
    resources.qrc

include(libkg/libkg.pri)

include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()
