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
    qmlRegisterType<TgStream>("Kutegram", 1, 0, "TgStream");
    qmlRegisterType<TgPacket>("Kutegram", 1, 0, "TgPacket");
    qRegisterMetaType<TelegramObject>("TelegramObject");
    qRegisterMetaType<TgVariant>("TgVariant");
    qRegisterMetaType<TgObject>("TgObject");
    qRegisterMetaType<TgMap>("TgMap");
    qRegisterMetaType<TgVector>("TgVector");
    qRegisterMetaType<TgList>("TgList");
    qRegisterMetaType<TgArray>("TgArray");
    qRegisterMetaType<TgInt128>("TgInt128");
    qRegisterMetaType<TgInt256>("TgInt256");
    qRegisterMetaType<TgInt>("TgInt");
    qRegisterMetaType<TgInteger>("TgInteger");
    qRegisterMetaType<TgLong>("TgLong");
    qRegisterMetaType<TgDouble>("TgDouble");
    qRegisterMetaType<TgString>("TgString");
    qRegisterMetaType<TgBool>("TgBool");
    qRegisterMetaType<TgByteArray>("TgByteArray");

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
