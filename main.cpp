#include <QApplication>
#include "qmlapplicationviewer.h"

#include <QFontDatabase>
#include <QTextCodec>
#include <QtDeclarative>
#include "tgclient.h"
#include "dialogsmodel.h"
#include "messagesmodel.h"
#include "messageeditor.h"
#include "systemname.h"
#include "avatardownloader.h"

int main(int argc, char *argv[])
{
    //TODO OpenGL acceleration
    //Causes some crashes on Windows, Symbian 9.2-9.3?, research it
    //Requires custom fonts, but they can't be install on iOS
#if QT_VERSION < 0x050000
    //QApplication::setGraphicsSystem("opengl");
#endif

    QApplication app(argc, argv);

    QApplication::setApplicationVersion("1.0.0");
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
    qmlRegisterType<MessageEditor>("Kutegram", 1, 0, "MessageEditor");
    qmlRegisterType<AvatarDownloader>("Kutegram", 1, 0, "AvatarDownloader");

    //TODO show status pane without button group on Symbian
    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.rootContext()->setContextProperty("kutegramVersion", QApplication::applicationVersion());
    viewer.rootContext()->setContextProperty("kutegramPlatform", systemName());
    viewer.setMainQmlFile(QLatin1String("qrc:/qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
