#include "platformutils.h"

#include <QApplication>

#if defined(Q_OS_WIN32) && QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include <QtWinExtras/QtWin>
#define DWM_FEATURES
#endif

PlatformUtils::PlatformUtils(QWidget *parent)
    : QObject((QObject*) parent)
    , window(parent)
    , trayIcon(this)
    , trayMenu()
{
    window->setAttribute(Qt::WA_DeleteOnClose, false);
    window->setAttribute(Qt::WA_QuitOnClose, false);

    connect(&trayIcon, SIGNAL(messageClicked()), this, SLOT(messageClicked()));
    connect(&trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
    connect(&trayMenu, SIGNAL(triggered(QAction*)), this, SLOT(menuTriggered(QAction*)));

    trayMenu.addAction("Open Kutegram");
    trayMenu.addAction("Exit");

    trayIcon.setContextMenu(&trayMenu);
    trayIcon.setIcon(QIcon(":/kutegramquick_small.png"));
    trayIcon.setToolTip("Kutegram");
    trayIcon.show();
}

void PlatformUtils::showAndRaise()
{
    window->show();
    window->activateWindow();
    window->raise();
}

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
        QApplication::exit();
        return;
    }

    showAndRaise();
}

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
