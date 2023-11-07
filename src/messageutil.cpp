#include "messageutil.h"

#include "tlschema.h"
#include "tgclient.h"
#include <QDateTime>

using namespace TLType;

bool entitiesSorter(const QVariant &v1, const QVariant &v2)
{
    TgObject o1 = v1.toMap();
    TgObject o2 = v2.toMap();

    return ID(o1) == MessageEntityUrl || o1["offset"].toInt() < o2["offset"].toInt() ||
            (o1["offset"].toInt() == o2["offset"].toInt() && o1["length"].toInt() < o2["length"].toInt());
}

void getTags(TgObject entity, QString textPart, qint32 i, QString &sTag, QString &eTag)
{
    switch (ID(entity)) {
    case MessageEntityUnknown:
        sTag = "";
        eTag = "";
        break;
    case MessageEntityMention:
        sTag = "<a href=\"kutegram://profile/" + textPart + "\">";
        eTag = "</a>";
        break;
    case MessageEntityHashtag:
        sTag = "<a href=\"kutegram://search/" + textPart + "\">";
        eTag = "</a>";
        break;
    case MessageEntityBotCommand:
        sTag = "<a href=\"kutegram://execute/" + textPart + "\">";
        eTag = "</a>";
        break;
    case MessageEntityUrl:
        sTag = "<a href=\"" + textPart + "\">";
        eTag = "</a>";
        break;
    case MessageEntityEmail:
        sTag = "<a href=\"mailto:" + textPart + "\">";
        eTag = "</a>";
        break;
    case MessageEntityBold:
        sTag = "<b>";
        eTag = "</b>";
        break;
    case MessageEntityItalic:
        sTag = "<i>";
        eTag = "</i>";
        break;
    case MessageEntityCode:
        sTag = "<code>";
        eTag = "</code>";
        break;
    case MessageEntityPre: //pre causes some issues with paddings
        sTag = "<code class=\"language-" + entity["language"].toString() + "\">";
        eTag = "</code>";
        break;
    case MessageEntityTextUrl:
        sTag = "<a href=\"" + entity["url"].toString() + "\">";
        eTag = "</a>";
        break;
    case MessageEntityMentionName:
        sTag = "<a href=\"kutegram://profile/" + entity["user_id"].toString() + "\">";
        eTag = "</a>";
        break;
    case InputMessageEntityMentionName:
        sTag = "<a href=\"kutegram://profile/" + TgClient::getPeerId(entity["user_id"].toMap()).toString() + "\">";
        eTag = "</a>";
        break;
    case MessageEntityPhone:
        sTag = "<a href=\"tel:" + textPart + "\">";
        eTag = "</a>";
        break;
    case MessageEntityCashtag:
        sTag = "<a href=\"kutegram://search/" + textPart + "\">";
        eTag = "</a>";
        break;
    case MessageEntityUnderline:
        sTag = "<u>";
        eTag = "</u>";
        break;
    case MessageEntityStrike:
        sTag = "<s>";
        eTag = "</s>";
        break;
    case MessageEntityBlockquote: //blockquote is strange too
        sTag = "<code>";
        eTag = "</code>";
        break;
    case MessageEntityBankCard:
        sTag = "<a href=\"kutegram://card/" + textPart + "\">";
        eTag = "</a>";
        break;
    case MessageEntitySpoiler:
        sTag = "<a class=\"spoiler\" href=\"kutegram://spoiler/" + QString::number(i) + "\">";
        eTag = "</a>";
        break;
    case MessageEntityCustomEmoji: //TODO custom emoji
        sTag = "";
        eTag = "";
        break;
    }
}

QString prepareDialogItemMessage(QString text, TgList entities)
{
    if (text.isEmpty()) {
        return text;
    }

    qSort(entities.begin(), entities.end(), entitiesSorter);

    for (qint32 i = 0; i < text.length(); ++i) {
        QString replace;

        if (text[i] == '&') {
            replace = "&amp;";
        } else if (text[i] == '<') {
            replace = "&lt;";
        } else if (text[i] == '>') {
            replace = "&gt;";
        } else {
            continue;
        }

        text.remove(i, 1);
        text.insert(i, replace);

        for (qint32 j = 0; j < entities.length(); ++j) {
            TgObject entity = entities[j].toMap();
            qint32 offset = entity["offset"].toInt();
            qint32 length = entity["length"].toInt();

            if (i >= offset && i < offset + length) {
                entity["length"] = length + replace.length() - 1;
            } else if (i < offset) {
                entity["offset"] = offset + replace.length() - 1;
            }

            entities[j] = entity;
        }

        i += replace.length() - 1;
    }

    //Just remove spoilers
    for (qint32 i = 0; i < entities.length(); ++i) {
        TgObject entity = entities[i].toMap();
        qint32 offset = entity["offset"].toInt();
        qint32 length = entity["length"].toInt();

        if (length == 0 || ID(entity) != MessageEntitySpoiler) {
            continue;
        }

        text.remove(offset, length);

        for (qint32 j = i + 1; j < entities.length(); ++j) {
            TgObject moving = entities[j].toMap();
            qint32 mOffset = moving["offset"].toInt();

            if (mOffset == offset) {
                moving["length"] = moving["length"].toInt() - length;
            }
            else {
                moving["offset"] = mOffset - length;
            }
            entities[j] = moving;
        }
    }

    text.replace('\n', " ");
    return text;
}

QString messageToHtml(QString text, TgList entities)
{
    //TODO unite neighbour spoilers
    if (text.isEmpty()) {
        return text;
    }

    qSort(entities.begin(), entities.end(), entitiesSorter);

    for (qint32 i = 0; i < text.length(); ++i) {
        QString replace;

        if (text[i] == '&') {
            replace = "&amp;";
        } else if (text[i] == '<') {
            replace = "&lt;";
        } else if (text[i] == '>') {
            replace = "&gt;";
        } else {
            continue;
        }

        text.remove(i, 1);
        text.insert(i, replace);

        for (qint32 j = 0; j < entities.length(); ++j) {
            TgObject entity = entities[j].toMap();
            qint32 offset = entity["offset"].toInt();
            qint32 length = entity["length"].toInt();

            if (i >= offset && i < offset + length) {
                entity["length"] = length + replace.length() - 1;
            } else if (i < offset) {
                entity["offset"] = offset + replace.length() - 1;
            }

            entities[j] = entity;
        }

        i += replace.length() - 1;
    }

    for (qint32 i = 0; i < entities.length(); ++i) {
        TgObject entity = entities[i].toMap();
        qint32 offset = entity["offset"].toInt();
        qint32 length = entity["length"].toInt();

        if (length == 0) {
            continue;
        }

        QString textPart = text.mid(offset, length);
        QString sTag, eTag;
        getTags(entity, textPart, i, sTag, eTag);

        text.insert(offset, sTag);
        text.insert(offset + sTag.length() + length, eTag);

        for (qint32 j = i + 1; j < entities.length(); ++j) {
            TgObject moving = entities[j].toMap();
            qint32 mOffset = moving["offset"].toInt();

            if (mOffset == offset) {
                moving["length"] = moving["length"].toInt() + sTag.length() + eTag.length();
            } else if (mOffset > offset && mOffset < offset + entity["length"].toInt()) {
                moving["offset"] = mOffset + sTag.length();
            } else if (mOffset > offset) {
                moving["offset"] = mOffset + sTag.length() + eTag.length();
            }
            entities[j] = moving;
        }
    }

    text.replace('\n', "<br />");
    return "<html><style>.spoiler, .spoiler > *, .spoiler > * > a:link { background-color: gray; color: gray; }</style>" + text + "</html>";
}

//TODO move message row generation methods to separate file
void handleMessageAction(TgObject &row, TgObject message, TgObject sender, TgList users, TgList chats)
{
    TgObject action = message["action"].toMap();
    switch (ID(action)) {
    case TLType::Unknown:
    case TLType::MessageActionEmpty:
        return;
    case TLType::MessageActionChatCreate:
        row["messageText"] = QString("created the group " + action["title"].toString());
        break;
    case TLType::MessageActionChatEditTitle:
        row["messageText"] = QString("changed group name to " + action["title"].toString());
        break;
    case TLType::MessageActionChatEditPhoto:
        row["messageText"] = QString("updated group photo");

        //copypaste from parent method
        row["hasMedia"] = false;
        row["photoFile"] = "";
        row["_photoToDownload"] = action["photo"].toMap();
        row["photoFileId"] = action["photo"].toMap()["id"].toLongLong();
        row["hasPhoto"] = row["photoFileId"].toLongLong() != 0;
        row["photoSpoiler"] = false;
        break;
    case TLType::MessageActionChatDeletePhoto:
        row["messageText"] = QString("removed group photo");
        break;
    case TLType::MessageActionChatAddUser:
    {
        QString message = "added";
        TgList usersIds = action["users"].toList();

        for (qint32 i = 0; i < usersIds.size(); ++i) {
            for (qint32 j = 0; j < users.size(); ++j) {
                if (TgClient::getPeerId(users[j].toMap()) == usersIds[i]) {
                    if (i == 0) {
                        message += " ";
                    } else {
                        message += ", ";
                    }
                    message += users[j].toMap()["first_name"].toString()
                            + " "
                            + users[j].toMap()["last_name"].toString();
                    break;
                }
            }
        }

        row["messageText"] = message;
        break;
    }
    case TLType::MessageActionChatDeleteUser:
    {
        QString message = "removed ";
        TgLong userId = action["user_id"].toLongLong();

        for (qint32 j = 0; j < users.size(); ++j) {
            if (TgClient::getPeerId(users[j].toMap()) == userId) {
                message += users[j].toMap()["first_name"].toString()
                        + " "
                        + users[j].toMap()["last_name"].toString();
                break;
            }
        }

        row["messageText"] = message;
        break;
    }
    case TLType::MessageActionChatJoinedByLink:
        row["messageText"] = QString("joined the group via invite link"); //TODO inviter_id?
        break;
    case TLType::MessageActionChannelCreate:
        row["messageText"] = QString("created the channel " + action["title"].toString());
        break;
    case TLType::MessageActionChatMigrateTo:
        row["messageText"] = QString("migrated this chat to supergroup");
        break;
    case TLType::MessageActionChannelMigrateFrom:
        row["messageText"] = QString("migrated this channel from group");
        break;
    case TLType::MessageActionPinMessage:
        row["messageText"] = QString("pinned a message"); //TODO how to find out what message?
        break;
    case TLType::MessageActionHistoryClear:
        row["messageText"] = QString("cleared history");
        break;
    case TLType::MessageActionGameScore:
        row["messageText"] = QString("scored " + action["score"].toString() + " in the game"); //TODO find out what game? (game_id:long)
        break;
    case TLType::MessageActionPaymentSentMe:
        row["messageText"] = QString("sent money to you"); //TODO currencies
        break;
    case TLType::MessageActionPaymentSent:
        row["messageText"] = QString("sent money"); //TODO currencies
        break;
    case TLType::MessageActionPhoneCall:
        row["messageText"] = QString("made a call"); //TODO custom message bubble (like TDesktop)
        break;
    case TLType::MessageActionScreenshotTaken:
        row["messageText"] = QString("took a screenshot");
        break;
    case TLType::MessageActionCustomAction:
        row["messageText"] = action["message"].toString(); //TODO what is this?
        break;
    case TLType::MessageActionBotAllowed:
        row["messageText"] = QString("gave bot permission to send messages");
        break;
    case TLType::MessageActionSecureValuesSentMe:
        row["messageText"] = QString("sent Telegram Passport secure values");
        break;
    case TLType::MessageActionSecureValuesSent:
        row["messageText"] = QString("requested Telegram Passport secure values");
        break;
    case TLType::MessageActionContactSignUp:
        row["messageText"] = QString("has registered in Telegram being your contact");
        break;
    case TLType::MessageActionGeoProximityReached:
    {
        QString message;

        TgObject fromId = action["from_id"].toMap();
        TgObject sender;

        if (TgClient::isUser(fromId)) for (qint32 j = 0; j < users.size(); ++j) {
            TgObject peer = users[j].toMap();
            if (TgClient::peersEqual(peer, fromId)) {
                sender = peer;
                break;
            }
        }
        if (TgClient::isChat(fromId)) for (qint32 j = 0; j < chats.size(); ++j) {
            TgObject peer = chats[j].toMap();
            if (TgClient::peersEqual(peer, fromId)) {
                sender = peer;
                break;
            }
        }

        message += sender["first_name"].toString()
                + " "
                + sender["last_name"].toString();

        message += " is now within ";
        qint32 distance = action["distance"].toInt();

        if (distance > 1000) {
            message += QString::number(distance / 1000.0f);
            message += " km from ";
        } else {
            message += QString::number(distance);
            message += " m from ";
        }

        fromId = action["to_id"].toMap();
        if (TgClient::isUser(fromId)) for (qint32 j = 0; j < users.size(); ++j) {
            TgObject peer = users[j].toMap();
            if (TgClient::peersEqual(peer, fromId)) {
                sender = peer;
                break;
            }
        }
        if (TgClient::isChat(fromId)) for (qint32 j = 0; j < chats.size(); ++j) {
            TgObject peer = chats[j].toMap();
            if (TgClient::peersEqual(peer, fromId)) {
                sender = peer;
                break;
            }
        }

        message += sender["first_name"].toString()
                + " "
                + sender["last_name"].toString();

        row["messageText"] = message;
        break;
    }
    case MessageActionGroupCall:
        row["messageText"] = "made a group call"; //TODO custom message bubble (like TDesktop)
        break;
    case MessageActionInviteToGroupCall:
    {
        QString message = "invited to group call";
        TgList usersIds = action["users"].toList();

        for (qint32 i = 0; i < usersIds.size(); ++i) {
            for (qint32 j = 0; j < users.size(); ++j) {
                if (TgClient::getPeerId(users[j].toMap()) == usersIds[i]) {
                    if (i == 0) {
                        message += " ";
                    } else {
                        message += ", ";
                    }
                    message += users[j].toMap()["first_name"].toString()
                            + " "
                            + users[j].toMap()["last_name"].toString();
                    break;
                }
            }
        }

        row["messageText"] = message;
        break;
    }
    case MessageActionSetMessagesTTL:
    {
        qint32 period = action["period"].toInt();
        if (period) {
            QString message = "set messages to auto-delete in ";
            if (period >= 30 * 24 * 60 * 60) {
                message += QString::number(period / 30 / 24 / 60 / 60);
                message += " months";
            } else if (period >= 24 * 60 * 60) {
                message += QString::number(period / 24 / 60 / 60);
                message += " days";
            } else if (period >= 60 * 60) {
                message += QString::number(period / 60 / 60);
                message += " hours";
            } else if (period >= 60) {
                message += QString::number(period / 60);
                message += " minutes";
            } else {
                message += QString::number(period);
                message += " seconds";
            }

            row["messageText"] = message;
        } else {
            row["messageText"] = QString("disabled the auto-delete timer");
        }
        break;
    }
    case MessageActionGroupCallScheduled:
        row["messageText"] = QString("scheduled a group call for " + QDateTime::fromTime_t(action["schedule_date"].toInt()).toString("MMMM d hh:mm"));
        break;
    case MessageActionSetChatTheme:
        row["messageText"] = QString("changed the chat theme");
        break;
    case MessageActionChatJoinedByRequest:
        row["messageText"] = QString("was accepted to the chat");
        break;
    case MessageActionWebViewDataSentMe:
        row["messageText"] = QString("transferred data from the \"" + action["text"].toString() + "\" button to the bot");
        break;
    case MessageActionWebViewDataSent:
        row["messageText"] = QString("transferred data from the \"" + action["text"].toString() + "\" button to the bot");
        break;
    case MessageActionGiftPremium:
        row["messageText"] = QString("sent you a Telegram Premium gift for " + action["months"].toString() + " months"); //TODO currencies
        break;
    case MessageActionTopicCreate:
        row["messageText"] = QString("created the topic " + action["title"].toString());
        break;
    case MessageActionTopicEdit:
        if (!action["title"].toString().isEmpty()) {
            row["messageText"] = QString("changed topic title to " + action["title"].toString());
        } else if (action["icon_emoji_id"].toLongLong()) {
            row["messageText"] = QString("changed topic icon");
        } else if (!action["closed"].isNull()) {
            if (action["closed"].toBool()) {
                row["messageText"] = QString("closed topic");
            } else {
                row["messageText"] = QString("reopened topic");
            }
        } else if (!action["hidden"].isNull()) {
            if (action["hidden"].toBool()) {
                row["messageText"] = QString("hid topic");
            } else {
                row["messageText"] = QString("showed topic");
            }
        } else{
            row["messageText"] = "edited the topic";
        }
        break;
    case MessageActionSuggestProfilePhoto:
        row["messageText"] = QString("suggests photo for your profile");

        //copypaste from parent method
        row["hasMedia"] = false;
        row["photoFile"] = "";
        row["_photoToDownload"] = action["photo"].toMap();
        row["photoFileId"] = action["photo"].toMap()["id"].toLongLong();
        row["hasPhoto"] = row["photoFileId"].toLongLong() != 0;
        row["photoSpoiler"] = false;
        break;
    case MessageActionRequestedPeer:
        row["messageText"] = QString("shared a peer to the bot");
        break;
    case MessageActionSetChatWallPaper:
        row["messageText"] = QString("changed the chat wallpaper");
        break;
    case MessageActionSetSameChatWallPaper:
        row["messageText"] = QString("changed the same chat wallpaper");
        break;
    case MessageActionGiftCode:
        row["messageText"] = QString("sent you a gift code");
        break;
    case MessageActionGiveawayLaunch:
        row["messageText"] = QString("just started a giveaway of Telegram Premium subscriptions for its followers");
        break;
    default:
        row["messageText"] = "unsupported service message";
        break;
    }
}
