#ifndef MESSAGEUTIL_H
#define MESSAGEUTIL_H

#include "tgstream.h"

QString prepareDialogItemMessage(QString text, TgList entities);
QString messageToHtml(QString text, TgList entities);

#endif // MESSAGEUTIL_H
