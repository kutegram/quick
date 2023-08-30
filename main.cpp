#include <QApplication>
#include "qmlapplicationviewer.h"

#include <QFontDatabase>
#include <QTextCodec>
#include <QtDeclarative>
#include "tgclient.h"
#include "dialogsmodel.h"
#include "messagesmodel.h"

int main(int argc, char *argv[])
{
    //TODO OpenGL acceleration
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

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
