#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"

#include <QFontDatabase>
#include <QTextCodec>
#include <QtDeclarative>
#include "tgclient.h"
#include "dialogsmodel.h"
#include "messagesmodel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationVersion("1.0.0");
    QApplication::setApplicationName("Kutegram");
    QApplication::setOrganizationName("Kutegram");
    QApplication::setOrganizationDomain("kg.crx.moe");

    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
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
