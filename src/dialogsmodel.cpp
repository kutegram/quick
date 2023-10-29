#include "dialogsmodel.h"

#include "tlschema.h"
#include <QMutexLocker>
#include <QColor>
#include <QDateTime>
#include "messageutil.h"

DialogsModel::DialogsModel(QObject *parent)
    : QAbstractListModel(parent)
    , _mutex(QMutex::Recursive)
    , _dialogs()
    , _client(0)
    , _userId(0)
    , _requestId(0)
    , _offsets()
    , _avatarDownloader(0)
    , _elideLength(0)
    , _folders(0)
{
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

void DialogsModel::setElideLength(qint32 length)
{
    QMutexLocker lock(&_mutex);

    _elideLength = length;

    for (qint32 i = 0; i < _dialogs.size(); ++i) {
        TgObject row = _dialogs[i];
        if (row["_message"].toString().isEmpty())
            continue;

        QString messageText = messageToHtml(row["_message"].toString(), row["_entities"].toList(), true, _elideLength);
        row["messageText"] = QString("<html>" + row["messageSenderName"].toString() + messageText + row["afterMessageText"].toString() + "</html>");
        _dialogs[i] = row;

        emit dataChanged(index(i), index(i));
    }
}

qint32 DialogsModel::elideLength() const
{
    return _elideLength;
}

void DialogsModel::setFolders(QObject *model)
{
    QMutexLocker lock(&_mutex);

    if (_folders) {
        _folders->disconnect(this);
    }

    _folders = dynamic_cast<FoldersModel*>(model);
    foldersChanged(_folders ? _folders->folders() : QList<TgObject>());

    if (!_folders) return;

    connect(_folders, SIGNAL(foldersChanged(QList<TgObject>)), this, SLOT(foldersChanged(QList<TgObject>)));
}

QObject* DialogsModel::folders() const
{
    return _folders;
}

void DialogsModel::foldersChanged(QList<TgObject> folders)
{
    QMutexLocker lock(&_mutex);

    for (qint32 i = 0; i < _dialogs.size(); ++i) {
        TgObject row = _dialogs[i];

        TgList dialogFolders;

        for (qint32 j = 0; j < folders.size(); ++j) {
            if (FoldersModel::matchesFilter(folders[j], qDeserialize(row["peerBytes"].toByteArray()).toMap())) {
                dialogFolders << j;
            }
        }

        row["folders"] = dialogFolders;

        _dialogs[i] = row;

        emit dataChanged(index(i), index(i));
    }
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

QHash<int, QByteArray> DialogsModel::roleNames() const
{
    static QHash<int, QByteArray> roles;

    if (!roles.isEmpty())
        return roles;

    roles[TitleRole] = "title";
    roles[ThumbnailColorRole] = "thumbnailColor";
    roles[ThumbnailTextRole] = "thumbnailText";
    roles[AvatarRole] = "avatar";
    roles[MessageTimeRole] = "messageTime";
    roles[MessageTextRole] = "messageText";
    roles[TooltipRole] = "tooltip";
    roles[PeerBytesRole] = "peerBytes";

    return roles;
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
    connect(_client, SIGNAL(messagesDialogsResponse(TgObject,TgLongVariant)), this, SLOT(messagesGetDialogsResponse(TgObject,TgLongVariant)));
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
    if (index.row() < 0) //TODO why this is even calling
        return QVariant();

    return _dialogs[index.row()][roleNames()[role]];
}

bool DialogsModel::canFetchMoreDownwards() const
{
    return _client && _client->isAuthorized() && !_requestId.toLongLong() && !_offsets.isEmpty();
}

void DialogsModel::fetchMoreDownwards()
{
    QMutexLocker lock(&_mutex);

    _requestId = _client->messagesGetDialogsWithOffsets(_offsets, 50);
}

void DialogsModel::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&_mutex);

    if (_userId != userId) {
        resetState();
        _userId = userId;
        fetchMoreDownwards();
    }
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

    QList<TgObject> folders;
    if (_folders)
        folders = _folders->folders();

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

        dialogsRows.append(createRow(lastDialog, lastPeer, lastMessage, messageSender, folders));
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

    if (canFetchMoreDownwards())
        fetchMoreDownwards();
}

TgObject DialogsModel::createRow(TgObject dialog, TgObject peer, TgObject message, TgObject messageSender, QList<TgObject> folders)
{
    TgObject row;

    TgObject inputPeer = peer;
    inputPeer.unite(dialog);
    ID_PROPERTY(inputPeer) = ID_PROPERTY(peer);
    row["peerBytes"] = qSerialize(inputPeer);

    TgList dialogFolders;

    for (qint32 j = 0; j < folders.size(); ++j) {
        if (FoldersModel::matchesFilter(folders[j], inputPeer)) {
            dialogFolders << j;
        }
    }

    row["folders"] = dialogFolders;

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

    //TODO 12-hour format
    row["messageTime"] = QDateTime::fromTime_t(qMax(message["date"].toInt(), message["edit_date"].toInt())).toString("hh:mm");

    QString messageSenderName;

    if (message["out"].toBool()) {
        //messageSenderName = "You";
    } else if (TgClient::isUser(messageSender)) {
        messageSenderName = messageSender["first_name"].toString();
    } else {
        messageSenderName = messageSender["title"].toString();
    }

    if (!messageSenderName.isEmpty()) {
        messageSenderName = "<span style=\"color: "
                + AvatarDownloader::userColor(messageSender["id"]).name()
                + "\">"
                + messageSenderName
                + ": </span>";
    }

    row["_message"] = message["message"];
    row["_entities"] = message["entities"];
    QString messageText = messageToHtml(row["_message"].toString(), row["_entities"].toList(), true, _elideLength);

    QString afterMessageText;
    if (GETID(message["media"].toMap()) != 0) {
        if (!messageText.isEmpty()) {
            afterMessageText += ", ";
        }

        //TODO attachment type
        afterMessageText += "Attachment";
    }

    row["messageSenderName"] = messageSenderName;
    row["afterMessageText"] = afterMessageText;
    row["messageText"] = QString("<html>" + row["messageSenderName"].toString() + messageText + row["afterMessageText"].toString() + "</html>");

    row["thumbnailColor"] = AvatarDownloader::userColor(peer["id"].toLongLong());
    row["thumbnailText"] = AvatarDownloader::getAvatarText(row["title"].toString());
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

        dialog["avatar"] = filePath;
        _dialogs[i] = dialog;

        emit dataChanged(index(i), index(i));
        break;
    }
}

void DialogsModel::refresh()
{
    resetState();
    fetchMoreDownwards();
}

bool DialogsModel::inFolder(qint32 index, qint32 folderIndex)
{
    QMutexLocker lock(&_mutex);

    if (!_folders || index < 0 || folderIndex < 0)
        return true;

    return _dialogs[index]["folders"].toList().contains(folderIndex);
}
