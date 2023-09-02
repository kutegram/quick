#include "dialogsmodel.h"

#include "tlschema.h"
#include <QMutexLocker>
#include <QColor>
#include <QDateTime>

DialogsModel::DialogsModel(QObject *parent)
    : QAbstractListModel(parent)
    , _mutex(QMutex::Recursive)
    , _dialogs()
    , _client(0)
    , _userId(0)
    , _requestId(0)
    , _offsets()
    , _avatarDownloader(0)
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
}

void DialogsModel::setClient(QObject *client)
{
    QMutexLocker lock(&_mutex);

    if (_client) {
        _client->disconnect(this);
    }

    _client = dynamic_cast<TgClient*>(client);
    _userId = 0;

    resetState();

    if (!_client) return;

    connect(_client, SIGNAL(authorized(TgLongVariant)), this, SLOT(authorized(TgLongVariant)));
    connect(_client, SIGNAL(messagesGetDialogsResponse(TgObject,TgLongVariant)), this, SLOT(messagesGetDialogsResponse(TgObject,TgLongVariant)));
}

QObject* DialogsModel::client() const
{
    return _client;
}

void DialogsModel::setAvatarDownloader(QObject *avatarDownloader)
{
    QMutexLocker lock(&_mutex);

    if (_avatarDownloader) {
        _avatarDownloader->disconnect(this);
    }

    _avatarDownloader = dynamic_cast<AvatarDownloader*>(avatarDownloader);

    if (!_avatarDownloader) return;

    connect(_avatarDownloader, SIGNAL(avatarDownloaded(TgLongVariant,QString)), this, SLOT(avatarDownloaded(TgLongVariant,QString)));
}

QObject* DialogsModel::avatarDownloader() const
{
    return _avatarDownloader;
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
    QMutexLocker lock(&_mutex);

    _requestId = _client->messagesGetDialogsWithOffsets(_offsets, 20);
}

void DialogsModel::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&_mutex);

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
    QMutexLocker lock(&_mutex);

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

    if (_avatarDownloader) {
        for (qint32 i = 0; i < usersList.size(); ++i) {
            _avatarDownloader->downloadAvatar(usersList[i].toMap());
        }
        for (qint32 i = 0; i < chatsList.size(); ++i) {
            _avatarDownloader->downloadAvatar(chatsList[i].toMap());
        }
    }
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

    row["thumbnailColor"] = AvatarDownloader::userColor(peer["id"].toLongLong());
    row["thumbnailText"] = AvatarDownloader::getAvatarText(row["title"].toString());
    row["avatarLoaded"] = false;
    row["avatar"] = "";
    row["photoId"] = peer["photo"].toMap()["photo_id"];

    return row;
}

void DialogsModel::avatarDownloaded(TgLongVariant photoId, QString filePath)
{
    QMutexLocker lock(&_mutex);

    for (qint32 i = 0; i < _dialogs.size(); ++i) {
        TgObject dialog = _dialogs[i];

        if (dialog["photoId"] != photoId) {
            continue;
        }

        dialog["avatarLoaded"] = true;
        dialog["avatar"] = filePath;
        _dialogs[i] = dialog;

        emit dataChanged(index(i), index(i));
        break;
    }
}

void DialogsModel::refresh()
{
    resetState();
    fetchMore(QModelIndex());
}
