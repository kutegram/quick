#include <QtGui/QApplication>
#include "qmlapplicationviewer.h"

#include <QDebug>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //TODO: set app properties and textcodecs
    qDebug() << QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont("fonts/OpenSans/OpenSans-Regular.ttf"));
    qDebug() << QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont("fonts/OpenSans/OpenSans-SemiBold.ttf"));

    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
    viewer.setMainQmlFile(QLatin1String("qml/main.qml"));

#ifdef Q_OS_SYMBIAN
    viewer.showMaximized();
#elif defined(Q_WS_MAEMO_5)
    viewer.showMaximized();
#else
    viewer.show();
#endif

    return app.exec();
}
