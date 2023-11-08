#ifndef MESSAGEUTIL_H
#define MESSAGEUTIL_H

#include "tgstream.h"

TgList& globalUsers();
TgList& globalChats();
QString prepareDialogItemMessage(QString text, TgList entities);
QString messageToHtml(QString text, TgList entities);
void handleMessageAction(TgObject &row, TgObject message, TgObject sender, TgList users, TgList chats);

#endif // MESSAGEUTIL_H
