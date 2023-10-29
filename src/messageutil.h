#ifndef MESSAGEUTIL_H
#define MESSAGEUTIL_H

#include "tgstream.h"

QString messageToHtml(QString text, TgList entities, bool inlineBreaks, qint32 limitWidth);

#endif // MESSAGEUTIL_H
