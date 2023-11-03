#include <QApplication>
#include "qmlapplicationviewer.h"

#include <QFontDatabase>
#include <QTextCodec>
#include <QtDeclarative>
#include <QFont>
#include <QFontMetrics>
#include "tgclient.h"
#include "dialogsmodel.h"
#include "messagesmodel.h"
#include "systemname.h"
#include "avatardownloader.h"
#include "foldersmodel.h"
#include "currentuserinfo.h"

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

    //TODO: keypad UI navigation
#ifdef Q_OS_SYMBIAN
    QApplication::setAttribute(Qt::AA_S60DisablePartialScreenInputMode, false);
//    QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
#endif

#if QT_VERSION >= 0x050300
    QApplication::setAttribute(Qt::AA_UseOpenGLES, true);
#endif

    QFont font = app.font();

#if defined(Q_OS_SYMBIAN)
    font.setPixelSize(qFloor(((float) font.pointSize()) * 3.0f));
#elif defined(Q_OS_WINDOWS)
    font.setPixelSize(qFloor(((float) font.pixelSize()) * 1.5f));
#endif

    app.setFont(font);
    QFontMetrics fontMetrics(font);

    QApplication::setApplicationVersion(VERSION);
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

    //TODO show status pane without button group on Symbian
    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.rootContext()->setContextProperty("kutegramVersion", QApplication::applicationVersion());
    viewer.rootContext()->setContextProperty("kutegramPlatform", systemName());
    viewer.rootContext()->setContextProperty("kgScaling", ((float) fontMetrics.height()) / 13.0f);
    viewer.rootContext()->setContextProperty("kgCharWidth", ((float) fontMetrics.averageCharWidth()));
    viewer.setMainQmlFile(QLatin1String("qrc:///qml/main.qml"));
    viewer.setWindowTitle("Kutegram");
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
