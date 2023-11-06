#include "platformutils.h"

#ifdef Q_OS_WIN32
#include <QtWinExtras/QtWin>
#endif

PlatformUtils::PlatformUtils(QWidget *parent)
    : QObject(parent)
    , window(parent)
{

}

void PlatformUtils::windowsExtendFrameIntoClientArea(int left, int top, int right, int bottom)
{
#ifdef Q_OS_WIN32
    QtWin::extendFrameIntoClientArea(window, left, top, right, bottom);
#endif
}

bool PlatformUtils::windowsIsCompositionEnabled()
{
#ifdef Q_OS_WIN32
    return QtWin::isCompositionEnabled();
#else
    return false;
#endif
}

QColor PlatformUtils::windowsRealColorizationColor()
{
#ifdef Q_OS_WIN32
    return QtWin::realColorizationColor();
#else
    return Qt::white;
#endif
}

bool PlatformUtils::isWindows()
{
#ifdef Q_OS_WIN32
    return true;
#else
    return false;
#endif
}
