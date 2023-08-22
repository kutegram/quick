#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"

#include <QFontDatabase>
#include <QTextCodec>
#include <QtDeclarative>
#include "tgclient.h"

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

    QFontDatabase::addApplicationFont("fonts/OpenSans/OpenSans-Regular.ttf");
    QFontDatabase::addApplicationFont("fonts/OpenSans/OpenSans-SemiBold.ttf");

    qmlRegisterType<TgClient>("Kutegram", 1, 0, "TgClient");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
