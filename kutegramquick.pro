TARGET = Kutegram
APPNAME = Kutegram
VERSION = 1.1.2
PKG_VERSION = 1,1,2
DEFINES += VERSION=\"\\\"$$VERSION\\\"\"
#DATE = $$system(date /t)
#DEFINES += BUILDDATE=\"\\\"$$DATE\\\"\"
#COMMIT_SHA = $$system(git log --pretty=format:%h -n 1);
#DEFINES += COMMIT_SHA=\"\\\"$$COMMIT_SHA\\\"\"

QT += core declarative network xml
DEFINES += QT_USE_FAST_CONCATENATION QT_USE_FAST_OPERATOR_PLUS
CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT KG_NO_DEBUG KG_NO_INFO

QML_IMPORT_PATH =

win32:RC_FILE = kutegramquick.rc
macx:ICON = kutegramquick.icns

symbian {
    ICON = kutegramquick.svg
    TARGET.UID3 = 0xE0713D51
    DEFINES += SYMBIAN_UID=$$TARGET.UID3

    TARGET.CAPABILITY += ReadUserData WriteUserData UserEnvironment NetworkServices LocalServices SwEvent
    #TARGET.EPOCHEAPSIZE = 0x400000 0x4000000
    #TARGET.EPOCSTACKSIZE = 0x14000

    supported_platforms = \
            "[0x1028315F],0,0,0,{\"S60ProductID\"}" \ # Symbian^1
            "[0x20022E6D],0,0,0,{\"S60ProductID\"}" \ # Symbian^3
            "[0x102032BE],0,0,0,{\"S60ProductID\"}" \ # Symbian 9.2
            "[0x102752AE],0,0,0,{\"S60ProductID\"}" \ # Symbian 9.3
            "[0x2003A678],0,0,0,{\"S60ProductID\"}"   # Symbian Belle

    default_deployment.pkg_prerules -= pkg_platform_dependencies
    supported_platforms_deployment.pkg_prerules += supported_platforms
    DEPLOYMENT += supported_platforms_deployment

    vendor_info = \
        " " \
        "; Localised Vendor name" \
        "%{\"curoviyxru\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"curoviyxru\"" \
        " "
    package.pkg_prerules += vendor_info

    header = "$${LITERAL_HASH}{\"Kutegram\"},(0xE0713D51),$$PKG_VERSION,TYPE=SA,RU"
    package.pkg_prerules += header

    DEPLOYMENT += package
    DEPLOYMENT.installer_header = "$${LITERAL_HASH}{\"Kutegram Installer\"},(0xE5E0AFB2),$$PKG_VERSION"
}

INCLUDEPATH += src

SOURCES +=  \
	src/main.cpp \
    src/dialogsmodel.cpp \
    src/messagesmodel.cpp \
    src/avatardownloader.cpp \
    src/foldersmodel.cpp \
    src/currentuserinfo.cpp \
    src/messageutil.cpp

HEADERS += \
    src/dialogsmodel.h \
    src/messagesmodel.h \
    src/avatardownloader.h \
    src/foldersmodel.h \
    src/currentuserinfo.h \
    src/messageutil.h

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

RESOURCES += \
    resources.qrc

include(libkg/libkg.pri)

include(pigler/qt-library/library.pri)

include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
