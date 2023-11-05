#include "messageutil.h"

#include "tlschema.h"
#include "tgclient.h"

using namespace TLType;

bool entitiesSorter(const QVariant &v1, const QVariant &v2)
{
    TgObject o1 = v1.toMap();
    TgObject o2 = v2.toMap();

    return o1["offset"].toInt() < o2["offset"].toInt() ||
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
        sTag = "<a href=\"kutegram://spoiler/" + QString::number(i) + "\" style=\"background-color:gray;color:gray;\">";
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
            } else {
                moving["offset"] = mOffset + sTag.length() + eTag.length();
            }
            entities[j] = moving;
        }
    }

    text.replace('\n', "<br />");
    return "<html>" + text + "</html>";
}
