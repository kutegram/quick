#include "dialogsmodel.h"

#include "tlschema.h"
#include <QMutexLocker>
#include <QColor>
#include <QDateTime>

DialogsModel::DialogsModel(QObject *parent)
    : QAbstractListModel(parent)
    , mutex(QMutex::Recursive)
    , _dialogs()
    , _client(0)
    , _userId(0)
    , _requestId(0)
    , _offsets()
    , _loadingAvatars()
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[ThumbnailColorRole] = "thumbnailColor";
    roles[ThumbnailTextRole] = "thumbnailText";
    roles[AvatarRole] = "avatar";
    roles[MessageTimeRole] = "messageTime";
    roles[MessageTextRole] = "messageText";
    roles[AvatarLoadedRole] = "avatarLoaded";
    roles[InputPeerRole] = "inputPeer";
    roles[PeerBytesRole] = "peerBytes";
    roles[TooltipRole] = "tooltip";
    setRoleNames(roles);
}

void DialogsModel::resetState()
{
    if (!_dialogs.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, _dialogs.size() - 1);
        _dialogs.clear();
        endRemoveRows();
    }

    _requestId = 0;
    _offsets = TgObject();
    _offsets["_start"] = true;
    _loadingAvatars.clear();
}

void DialogsModel::setClient(QObject *client)
{
    QMutexLocker lock(&mutex);

    if (_client) {
        _client->disconnect(this);
    }

    _client = dynamic_cast<TgClient*>(client);
    _userId = 0;

    resetState();

    if (!_client) return;

    _client->cacheDirectory().mkdir("avatars");

    connect(_client, SIGNAL(authorized(TgLongVariant)), this, SLOT(authorized(TgLongVariant)));
    connect(_client, SIGNAL(messagesGetDialogsResponse(TgObject,TgLongVariant)), this, SLOT(messagesGetDialogsResponse(TgObject,TgLongVariant)));
    connect(_client, SIGNAL(fileDownloaded(TgLongVariant,QString)), this, SLOT(fileDownloaded(TgLongVariant,QString)));
    connect(_client, SIGNAL(fileDownloadCanceled(TgLongVariant,QString)), this, SLOT(fileDownloadCanceled(TgLongVariant,QString)));
}

QObject* DialogsModel::client() const
{
    return _client;
}

int DialogsModel::rowCount(const QModelIndex &parent) const
{
    return _dialogs.size();
}

QVariant DialogsModel::data(const QModelIndex &index, int role) const
{
    return _dialogs[index.row()][roleNames()[role]];
}

bool DialogsModel::canFetchMore(const QModelIndex &parent) const
{
    return _client && _client->isAuthorized() && !_requestId.toLongLong() && !_offsets.isEmpty();
}

void DialogsModel::fetchMore(const QModelIndex &parent)
{
    QMutexLocker lock(&mutex);

    _requestId = _client->messagesGetDialogsWithOffsets(_offsets, 20);
}

void DialogsModel::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&mutex);

    if (_userId != userId) {
        resetState();
        _userId = userId;
        fetchMore(QModelIndex());
        return;
    }

    _userId = userId;
}

void DialogsModel::messagesGetDialogsResponse(TgObject data, TgLongVariant messageId)
{
    QMutexLocker lock(&mutex);

    if (_requestId != messageId) {
        return;
    }

    _requestId = 0;

    switch (GETID(data)) {
    case TLType::MessagesDialogs:
    case TLType::MessagesDialogsNotModified:
        _offsets = TgObject();
        break;
    case TLType::MessagesDialogsSlice:
        _offsets = TgClient::getDialogsOffsets(data);
        break;
    }

    TgList dialogsList = data["dialogs"].toList();
    TgList messagesList = data["messages"].toList();
    TgList usersList = data["users"].toList();
    TgList chatsList = data["chats"].toList();

    if (dialogsList.isEmpty()) {
        _offsets = TgObject();
        return;
    }

    QList<TgObject> dialogsRows;
    dialogsRows.reserve(dialogsList.size());

    for (qint32 i = 0; i < dialogsList.size(); ++i) {
        TgObject lastDialog = dialogsList[i].toMap();
        TgObject lastDialogPeer = lastDialog["peer"].toMap();
        TgInt lastMessageId = lastDialog["top_message"].toInt();

        TgObject lastMessage;
        TgObject lastPeer;

        for (qint32 j = messagesList.size() - 1; j >= 0; --j) {
            TgObject message = messagesList[j].toMap();
            if (TgClient::peersEqual(message["peer_id"].toMap(), lastDialogPeer)
                    && message["id"].toInt() == lastMessageId) {
                lastMessage = message;
                break;
            }
        }

        if (TgClient::isUser(lastDialogPeer)) for (qint32 j = 0; j < usersList.size(); ++j) {
            TgObject peer = usersList[j].toMap();
            if (TgClient::peersEqual(peer, lastDialogPeer)) {
                lastPeer = peer;
                break;
            }
        }
        if (TgClient::isChat(lastDialogPeer)) for (qint32 j = 0; j < chatsList.size(); ++j) {
            TgObject peer = chatsList[j].toMap();
            if (TgClient::peersEqual(peer, lastDialogPeer)) {
                lastPeer = peer;
                break;
            }
        }

        TgObject fromId = lastMessage["from_id"].toMap();
        TgObject messageSender;

        if (TgClient::isUser(fromId)) for (qint32 j = 0; j < usersList.size(); ++j) {
            TgObject peer = usersList[j].toMap();
            if (TgClient::peersEqual(peer, fromId)) {
                messageSender = peer;
                break;
            }
        }
        if (TgClient::isChat(fromId)) for (qint32 j = 0; j < chatsList.size(); ++j) {
            TgObject peer = chatsList[j].toMap();
            if (TgClient::peersEqual(peer, fromId)) {
                messageSender = peer;
                break;
            }
        }

        dialogsRows.append(createRow(lastDialog, lastPeer, lastMessage, messageSender));
    }

    beginInsertRows(QModelIndex(), _dialogs.size(), _dialogs.size() + dialogsRows.size() - 1);
    _dialogs.append(dialogsRows);
    endInsertRows();
}

QString getAvatarText(QString title)
{
    QStringList split = title.split(" ", QString::SkipEmptyParts);
    QString result;

    for (qint32 i = 0; i < split.size(); ++i) {
        if (result.size() > 1) break;
        QString item = split[i];
        if (item.isEmpty()) continue;

        for (qint32 j = 0; j < item.length(); ++j) {
            if (item[j].isPrint()) {
                result += item[j].toUpper();
                break;
            }
        }
    }

    if (result.isEmpty() && !title.isEmpty())
        result += title[0].toUpper();

    return result;
}

QColor userColor(qint64 id)
{
   return QColor::fromHsl(id % 360, 160, 160);
}

TgObject DialogsModel::createRow(TgObject dialog, TgObject peer, TgObject message, TgObject messageSender)
{
    TgObject row;

    TgObject inputPeer = peer;
    row["inputPeer"] = inputPeer;

    inputPeer["read_inbox_max_id"] = dialog["read_inbox_max_id"];
    inputPeer["read_outbox_max_id"] = dialog["read_outbox_max_id"];
    inputPeer["draft"] = dialog["draft"];

    QByteArray array;
    QDataStream peerStream(&array, QIODevice::WriteOnly);
    peerStream << inputPeer;
    row["peerBytes"] = array;

    //TODO typing status
    if (TgClient::isUser(peer)) {
        row["title"] = QString(peer["first_name"].toString() + " " + peer["last_name"].toString());
        row["tooltip"] = "user"; //TODO last seen and online
    } else {
        row["title"] = peer["title"].toString();

        QString tooltip = TgClient::isChannel(peer) ? "channel" : "chat";
        if (!peer["participants_count"].isNull()) {
            tooltip = peer["participants_count"].toString();
            //TODO localization support
            tooltip += TgClient::isChannel(peer) ? " subscribers" : " members";
        }

        row["tooltip"] = tooltip;
    }

    row["thumbnailColor"] = userColor(peer["id"].toLongLong());
    row["thumbnailText"] = getAvatarText(row["title"].toString());
    row["avatarLoaded"] = false;

    row["messageTime"] = QDateTime::fromTime_t(qMax(message["date"].toInt(), message["edit_date"].toInt())).toString("hh:mm");

    QString messageSenderName;

    if (TgClient::commonPeerType(messageSender) == 0) {
        //This means that it is a channel feed or personal messages.
        //Authorized user isn't returned by API, so we have to deal with it.
        if (message["out"].toBool()) {
            messageSenderName = "You";
        }
        //else messageSender = peer;
    }

    if (TgClient::isUser(messageSender)) {
        messageSenderName = messageSender["first_name"].toString();
    } else {
        messageSenderName = messageSender["title"].toString();
    }

    if (!messageSenderName.isEmpty()) {
        messageSenderName += ": ";
    }

    //TODO markdown / styled entities
    QString messageText = message["message"].toString().replace('\n', " ");

    if (GETID(message["media"].toMap()) != 0) {
        if (!messageText.isEmpty()) {
            messageText += ", ";
        }

        //TODO attachment type
        messageText += "Attachment";
    }

    messageSenderName += messageText;

    row["messageText"] = messageSenderName;

    TgObject photo = peer["photo"].toMap();
    if (GETID(photo)) {
        qint64 photoId = photo["photo_id"].toLongLong();
        QString relativePath = "avatars/" + QString::number(photoId) + ".jpg";
        QString avatarFilePath = _client->cacheDirectory().absoluteFilePath(relativePath);
        QFile avatarFile(avatarFilePath);

        row["avatar"] = avatarFilePath;

        if (!avatarFile.exists() || avatarFile.size() == 0) {
            qint64 loadingId = _client->downloadFile(avatarFilePath, peer).toLongLong();
            _loadingAvatars[loadingId] = row["inputPeer"].toMap();
        } else {
            row["avatarLoaded"] = true;
        }
    }

    return row;
}

void DialogsModel::fileDownloadCanceled(TgLongVariant fileId, QString filePath)
{
    QMutexLocker lock(&mutex);

    _loadingAvatars.remove(fileId.toLongLong());
}

void DialogsModel::fileDownloaded(TgLongVariant fileId, QString filePath)
{
    QMutexLocker lock(&mutex);

    TgObject inputPeer = _loadingAvatars.take(fileId.toLongLong());

    for (qint32 i = 0; i < _dialogs.size(); ++i) {
        TgObject dialog = _dialogs[i];
        if (TgClient::peersEqual(dialog["inputPeer"].toMap(), inputPeer)) {
            dialog["avatarLoaded"] = true;
            _dialogs[i] = dialog;
            emit dataChanged(index(i), index(i));
            break;
        }
    }
}

void DialogsModel::refresh()
{
    resetState();
    fetchMore(QModelIndex());
}
