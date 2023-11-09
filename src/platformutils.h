#ifndef PLATFORMUTILS_H
#define PLATFORMUTILS_H

#include <QObject>
#include <QColor>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QUrl>

#ifdef SYMBIAN3_READY
#include "QPiglerAPI.h"
#endif

class PlatformUtils : public QObject
{
    Q_OBJECT
private:
    QWidget* window;
#ifndef Q_OS_SYMBIAN
    QSystemTrayIcon trayIcon;
    QMenu trayMenu;
#endif
    QHash<qint64, QVariantMap> unread;
#ifdef SYMBIAN3_READY
    QPiglerAPI pigler;
    qint32 piglerId;
#endif

public:
    explicit PlatformUtils(QWidget *parent = 0);

signals:

public slots:
    void showAndRaise();
    void quit();

#ifndef Q_OS_SYMBIAN
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void menuTriggered(QAction* action);
#endif

#ifdef SYMBIAN3_READY
    void piglerHandleTap(qint32 notificationId);
#endif

    void windowsExtendFrameIntoClientArea(int left, int top, int right, int bottom);
    bool windowsIsCompositionEnabled();
    QColor windowsRealColorizationColor();
    bool isWindows();

    void gotNewMessage(qint64 peerId, QString peerName, QString senderName, QString text, bool silent);
};

void openUrl(QUrl url);

#endif // PLATFORMUTILS_H
