#include "platformutils.h"

#include <QApplication>
#include "debug.h"

#if defined(Q_OS_WIN32) && QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include <QtWinExtras/QtWin>
#define DWM_FEATURES
#endif

#ifdef SYMBIAN3_READY
#include <akndiscreetpopup.h>
#endif

#ifdef Q_OS_SYMBIAN
#include <apgcli.h>
#include <apgtask.h>
#include <eikenv.h>
#else
#include <QDesktopServices>
#endif

#include <crypto.h>

PlatformUtils::PlatformUtils(QWidget *parent)
    : QObject((QObject*) parent)
    , window(parent)
#ifndef Q_OS_SYMBIAN
    , trayIcon(this)
    , trayMenu()
#endif
    , unread()
#ifdef SYMBIAN3_READY
    , pigler()
    , piglerId(-1)
#endif
{
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    window->setAttribute(Qt::WA_QuitOnClose, false);

#ifndef Q_OS_SYMBIAN
    connect(&trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(&trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(&trayMenu, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));

    trayMenu.addAction("Open Kutegram");
    trayMenu.addAction("Exit");

    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setIcon(QIcon(":/kutegramquick_small.png"));
    trayIcon.setToolTip("Kutegram");
    trayIcon.show();
#endif

#ifdef SYMBIAN3_READY
    qint32 response = pigler.init("Kutegram"); //TODO think about randomization
    if (response >= 0) {
        if (response > 0)
            piglerHandleTap(response);

        connect(&pigler, SIGNAL(handleTap(qint32)), this, SLOT(piglerHandleTap(qint32)));
        pigler.removeAllNotifications();
        piglerId = 0;
    }
#endif
}

#ifdef SYMBIAN3_READY
void PlatformUtils::piglerHandleTap(qint32 notificationId)
{
    //App should be opened automatically
    unread.clear();
}
#endif

void PlatformUtils::showAndRaise()
{
    //TODO remove notifications
    unread.clear();

    window->show();
    window->activateWindow();
    window->raise();
}

void PlatformUtils::quit()
{
    QApplication::exit();
}

#ifndef Q_OS_SYMBIAN
void PlatformUtils::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason != QSystemTrayIcon::Context) {
        showAndRaise();
    }
}

void PlatformUtils::messageClicked()
{
    showAndRaise();
}

void PlatformUtils::menuTriggered(QAction *action)
{
    if (action->text() == "Exit") {
        quit();
        return;
    }

    showAndRaise();
}
#endif

void PlatformUtils::windowsExtendFrameIntoClientArea(int left, int top, int right, int bottom)
{
#ifdef DWM_FEATURES
    window->setAttribute(Qt::WA_TranslucentBackground, true);
    window->setAttribute(Qt::WA_NoSystemBackground, false);
    window->setStyleSheet("background: transparent");
    QtWin::extendFrameIntoClientArea(window, left, top, right, bottom);
#endif
}

bool PlatformUtils::windowsIsCompositionEnabled()
{
#ifdef DWM_FEATURES
    return QtWin::isCompositionEnabled();
#else
    return false;
#endif
}

QColor PlatformUtils::windowsRealColorizationColor()
{
#ifdef DWM_FEATURES
    return QtWin::realColorizationColor();
#else
    return Qt::white;
#endif
}

bool PlatformUtils::isWindows()
{
#ifdef DWM_FEATURES
    return true;
#else
    return false;
#endif
}

void PlatformUtils::gotNewMessage(qint64 peerId, QString peerName, QString senderName, QString text, bool silent)
{
    if (window->hasFocus()) {
        unread.clear();
        return;
    }

    QVariantMap info;
    info["id"] = peerId;
    info["peerName"] = peerName;
    info["senderName"] = senderName;
    info["text"] = text;

    unread.insert(peerId, info);

    QString title;
    QString message;

    title = peerName;
    message = senderName;
    message += text;

#ifndef Q_OS_SYMBIAN
    if (!silent) {
        kgDebug() << "Sending Windows notification";
        trayIcon.showMessage(title, message);
    }
#endif

    if (unread.size() != 1) {
        title = "New messages from " + QString::number(unread.size()) + " chats";
        foreach (qint32 pid, unread.keys()) {
            if (!message.isEmpty()) {
                message += ", ";
            }
            message += unread[pid]["peerName"].toString();
        }
    }

    title = title.left(63);
    message = message.left(63);

#ifdef SYMBIAN3_READY
    kgDebug() << "Sending Symbian notification";
    TUid symbianUid = {SYMBIAN_UID};
    //TODO: icon
    TRAP_IGNORE(CAknDiscreetPopup::ShowGlobalPopupL(TPtrC16(title.utf16()), TPtrC16(message.utf16()), KAknsIIDNone, KNullDesC, 0, 0, 1, 0, 0, symbianUid));
#endif

#ifdef SYMBIAN3_READY
    kgDebug() << "Sending Pigler notification";
    if (piglerId == 0) {
        piglerId = pigler.createNotification(title, message);
    } else if (piglerId > 0) {
        pigler.updateNotification(piglerId, title, message);
    } else {
        kgDebug() << "Pigler is not initialized";
    }

    if (piglerId > 0) {
        static QImage piglerImage(":/kutegramquick_pigler.png");
        pigler.setNotificationIcon(piglerId, piglerImage);
    }
#endif

    //TODO: notify only when unfocused?
    //TODO: custom notification popup for Windows/legacy Symbian
    //TODO: android
    //TODO: vibrate
    //TODO: sound
    //TODO: blink
}

void openUrl(QUrl url)
{
#ifdef Q_OS_SYMBIAN
    static TUid KUidBrowser = {0x10008D39};
    _LIT(KBrowserPrefix, "4 ");

    // convert url to encoded version of QString
    QString encUrl(QString::fromUtf8(url.toEncoded()));
    // using qt_QString2TPtrC() based on
    // <http://qt.gitorious.org/qt/qt/blobs/4.7/src/corelib/kernel/qcore_symbian_p.h#line102>
    TPtrC tUrl(TPtrC16(static_cast<const TUint16*>(encUrl.utf16()), encUrl.length()));

    // Following code based on
    // <http://www.developer.nokia.com/Community/Wiki/Launch_default_web_browser_using_Symbian_C%2B%2B>

    // create a session with apparc server
    RApaLsSession appArcSession;
    User::LeaveIfError(appArcSession.Connect());
    CleanupClosePushL<RApaLsSession>(appArcSession);

    // get the default application uid for application/x-web-browse
    TDataType mimeDatatype(_L8("application/x-web-browse"));
    TUid handlerUID;
    appArcSession.AppForDataType(mimeDatatype, handlerUID);

    // if UiD not found, use the native browser
    if (handlerUID.iUid == 0 || handlerUID.iUid == -1)
        handlerUID = KUidBrowser;

    // Following code based on
    // <http://qt.gitorious.org/qt/qt/blobs/4.7/src/gui/util/qdesktopservices_s60.cpp#line213>

    HBufC* buf16 = HBufC::NewLC(tUrl.Length() + KBrowserPrefix.iTypeLength);
    buf16->Des().Copy(KBrowserPrefix); // Prefix used to launch correct browser view
    buf16->Des().Append(tUrl);

    TApaTaskList taskList(CCoeEnv::Static()->WsSession());
    TApaTask task = taskList.FindApp(handlerUID);
    if (task.Exists()) {
        // Switch to existing browser instance
        task.BringToForeground();
        HBufC8* param8 = HBufC8::NewLC(buf16->Length());
        param8->Des().Append(buf16->Des());
        task.SendMessage(TUid::Uid(0), *param8); // Uid is not used
        CleanupStack::PopAndDestroy(param8);
    } else {
        // Start a new browser instance
        TThreadId id;
        appArcSession.StartDocument(*buf16, handlerUID, id);
    }

    CleanupStack::PopAndDestroy(buf16);
    CleanupStack::PopAndDestroy(&appArcSession);
#else
    QDesktopServices::openUrl(url);
#endif
}
