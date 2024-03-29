#include <QApplication>
#include "qmlapplicationviewer.h"

#include <QFontDatabase>
#include <QTextCodec>
#include <QFont>
#include <QFontMetrics>
#include "tgclient.h"
#include "dialogsmodel.h"
#include "messagesmodel.h"
#include "systemname.h"
#include "avatardownloader.h"
#include "foldersmodel.h"
#include "currentuserinfo.h"
#include "platformutils.h"
#include <QSystemSemaphore>
#include <QSharedMemory>

#if QT_VERSION >= 0x050000
#include <QQmlContext>
#include <QQmlEngine>
#else
#include <QtDeclarative>
#endif

#if QT_VERSION >= 0x040702
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#endif

int main(int argc, char *argv[])
{
    //TODO OpenGL acceleration
    //Causes some crashes on Windows, Symbian 9.2-9.3?, research it
    //Requires custom fonts, but they can't be install on iOS
#if QT_VERSION < 0x050000
    //QApplication::setGraphicsSystem("opengl");
#endif

    QApplication app(argc, argv);

    QSystemSemaphore sema("Kutegram_semaphone", 1);
    bool isRunning;
    sema.acquire();

    {
        QSharedMemory shmem("Kutegram_shared");
        shmem.attach();
    }

    QSharedMemory shmem("Kutegram_shared");
    if (shmem.attach())
    {
        isRunning = true;
    }
    else
    {
        shmem.create(1);
        isRunning = false;
    }

    sema.release();
    if (isRunning) {
        //TODO raise Kutegram window
        return 1;
    }

    //TODO: keypad UI navigation
#ifdef Q_OS_SYMBIAN
    QApplication::setAttribute(Qt::AA_S60DisablePartialScreenInputMode, false);
//    QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
#endif

#if QT_VERSION >= 0x050300
    QApplication::setAttribute(Qt::AA_UseOpenGLES, true);
#endif

    QApplication::setApplicationVersion(QString(VERSION).replace("\"", ""));
    QApplication::setApplicationName("Kutegram");
    QApplication::setOrganizationName("Kutegram");
    QApplication::setOrganizationDomain("kg.crx.moe");

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
#if QT_VERSION < 0x050000
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
#endif
    QTextCodec::setCodecForLocale(codec);

    TgClient::registerQML();
    qmlRegisterType<DialogsModel>("Kutegram", 1, 0, "DialogsModel");
    qmlRegisterType<MessagesModel>("Kutegram", 1, 0, "MessagesModel");
    qmlRegisterType<AvatarDownloader>("Kutegram", 1, 0, "AvatarDownloader");
    qmlRegisterType<FoldersModel>("Kutegram", 1, 0, "FoldersModel");
    qmlRegisterType<CurrentUserInfo>("Kutegram", 1, 0, "CurrentUserInfo");
    qmlRegisterUncreatableType<PlatformUtils>("Kutegram", 1, 0, "PlatformUtils", "PlatformUtils is uncreatable. Use platformUtils root property.");

    //TODO show status pane without button group on Symbian
    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.rootContext()->setContextProperty("kutegramVersion", QApplication::applicationVersion());
    viewer.rootContext()->setContextProperty("kutegramPlatform", systemName());
    viewer.rootContext()->setContextProperty("platformUtils", new PlatformUtils(&viewer));
    viewer.rootContext()->setContextProperty("kgScaling", QFontMetrics(app.font()).height() / 14.0f);
    viewer.setMainQmlFile(QLatin1String("qrc:///qml/Main.qml"));
#if QT_VERSION >= 0x050000
    viewer.setTitle("Kutegram");
#else
    viewer.setWindowTitle("Kutegram");
#endif
    viewer.showExpanded();

#if QT_VERSION >= 0x040702
    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        //TODO save network selection
        QNetworkConfiguration config = manager.defaultConfiguration();
        QNetworkSession* networkSession = new QNetworkSession(config);
        networkSession->open(); //TODO reset network selection
    }
#endif

    return app.exec();
}
