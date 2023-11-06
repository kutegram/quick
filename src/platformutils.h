#ifndef PLATFORMUTILS_H
#define PLATFORMUTILS_H

#include <QObject>
#include <QColor>

class PlatformUtils : public QObject
{
    Q_OBJECT
private:
    QWidget* window;

public:
    explicit PlatformUtils(QWidget *parent = 0);

signals:

public slots:
    void windowsExtendFrameIntoClientArea(int left, int top, int right, int bottom);
    bool windowsIsCompositionEnabled();
    QColor windowsRealColorizationColor();
    bool isWindows();
};

#endif // PLATFORMUTILS_H
