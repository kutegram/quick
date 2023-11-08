#ifndef PLATFORMUTILS_H
#define PLATFORMUTILS_H

#include <QObject>
#include <QColor>
#include <QSystemTrayIcon>
#include <QMenu>

class PlatformUtils : public QObject
{
    Q_OBJECT
private:
    QWidget* window;
    QSystemTrayIcon trayIcon;
    QMenu trayMenu;

public:
    explicit PlatformUtils(QWidget *parent = 0);

signals:

public slots:
    void showAndRaise();
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void menuTriggered(QAction* action);

    void windowsExtendFrameIntoClientArea(int left, int top, int right, int bottom);
    bool windowsIsCompositionEnabled();
    QColor windowsRealColorizationColor();
    bool isWindows();
};

#endif // PLATFORMUTILS_H
