#include "platformutils.h"

#if defined(Q_OS_WIN32) && QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include <QtWinExtras/QtWin>
#define DWM_FEATURES
#endif

PlatformUtils::PlatformUtils(QWidget *parent)
    : QObject((QObject*) parent)
    , window(parent)
{

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
