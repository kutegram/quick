#include "messagesmodel.h"

#include "tlschema.h"
#include <QMutexLocker>
#include <QColor>
#include <QDateTime>
#include <QUrl>
#include <QDomDocument>
#include "avatardownloader.h"
#include <QFileDialog>
#include "messageutil.h"
#include <QDesktopServices>

#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#endif

using namespace TLType;

//TODO use SQLite
TgList _globalUsers;
TgList _globalChats;

#define BATCH_SIZE 40

MessagesModel::MessagesModel(QObject *parent)
    : QAbstractListModel(parent)
    , _mutex(QMutex::Recursive)
    , _history()
    , _client(0)
    , _userId(0)
    , _peer()
    , _inputPeer()
    , _upRequestId(0)
    , _downRequestId(0)
    , _upOffset(0)
    , _downOffset(0)
    , _avatarDownloader(0)
    , _downloadRequests()
    , _uploadId(0)
    , _sentMessages()
    , _media()
{
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

QHash<int, QByteArray> MessagesModel::roleNames() const
{
    static QHash<int, QByteArray> roles;

    if (!roles.isEmpty())
        return roles;

    roles[PeerNameRole] = "peerName";
    roles[MessageTextRole] = "messageText";
    roles[MergeMessageRole] = "mergeMessage";
    roles[MessageTimeRole] = "messageTime";
    roles[SenderNameRole] = "senderName";
    roles[IsChannelRole] = "isChannel";
    roles[ThumbnailColorRole] = "thumbnailColor";
    roles[ThumbnailTextRole] = "thumbnailText";
    roles[AvatarRole] = "avatar";
    roles[HasMediaRole] = "hasMedia";
    roles[MediaImageRole] = "mediaImage";
    roles[MediaTitleRole] = "mediaTitle";
    roles[MediaTextRole] = "mediaText";
    roles[MediaDownloadableRole] = "mediaDownloadable";
    roles[MessageIdRole] = "messageId";
    roles[ForwardedFromRole] = "forwardedFrom";
    roles[MediaUrlRole] = "mediaUrl";
    roles[PhotoFileRole] = "photoFile";
    roles[HasPhotoRole] = "hasPhoto";

    return roles;
}

void MessagesModel::resetState()
{
    if (!_history.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, _history.size() - 1);
        _history.clear();
        endRemoveRows();
    }

    _peer = TgObject();
    _inputPeer = TgObject();
    _upRequestId = 0;
    _downRequestId = 0;
    _upOffset = 0;
    _downOffset = 0;
}

void MessagesModel::setClient(QObject *client)
{
    QMutexLocker lock(&_mutex);

    if (_client) {
        _client->disconnect(this);
    }

    _client = dynamic_cast<TgClient*>(client);
    _userId = 0;

    resetState();
    cancelUpload();

    if (!_client) return;

    connect(_client, SIGNAL(authorized(TgLongVariant)), this, SLOT(authorized(TgLongVariant)));
    connect(_client, SIGNAL(messagesMessagesResponse(TgObject,TgLongVariant)), this, SLOT(messagesGetHistoryResponse(TgObject,TgLongVariant)));
    connect(_client, SIGNAL(fileDownloaded(TgLongVariant,QString)), this, SLOT(fileDownloaded(TgLongVariant,QString)));
    connect(_client, SIGNAL(fileDownloadCanceled(TgLongVariant,QString)), this, SLOT(fileDownloadCanceled(TgLongVariant,QString)));
    connect(_client, SIGNAL(gotMessageUpdate(TgObject,TgLongVariant)), this, SLOT(gotMessageUpdate(TgObject,TgLongVariant)));
    connect(_client, SIGNAL(gotUpdate(TgObject,TgLongVariant,TgList,TgList,qint32,qint32,qint32)), this, SLOT(gotUpdate(TgObject,TgLongVariant,TgList,TgList,qint32,qint32,qint32)));
    connect(_client, SIGNAL(fileUploading(TgLongVariant,TgLongVariant,TgLongVariant,qint32)), this, SLOT(fileUploading(TgLongVariant,TgLongVariant,TgLongVariant,qint32)));
    connect(_client, SIGNAL(fileUploaded(TgLongVariant,TgObject)), this, SLOT(fileUploaded(TgLongVariant,TgObject)));
    connect(_client, SIGNAL(fileUploadCanceled(TgLongVariant)), this, SLOT(fileUploadCanceled(TgLongVariant)));
}

QObject* MessagesModel::client() const
{
    return _client;
}

void MessagesModel::setAvatarDownloader(QObject *avatarDownloader)
{
    QMutexLocker lock(&_mutex);

    if (_avatarDownloader) {
        _avatarDownloader->disconnect(this);
    }

    _avatarDownloader = dynamic_cast<AvatarDownloader*>(avatarDownloader);

    if (!_avatarDownloader) return;

    connect(_avatarDownloader, SIGNAL(avatarDownloaded(TgLongVariant,QString)), this, SLOT(avatarDownloaded(TgLongVariant,QString)));
    connect(_avatarDownloader, SIGNAL(photoDownloaded(TgLongVariant,QString)), this, SLOT(photoDownloaded(TgLongVariant,QString)));
}

QObject* MessagesModel::avatarDownloader() const
{
    return _avatarDownloader;
}

void MessagesModel::setPeer(QByteArray bytes)
{
    QMutexLocker lock(&_mutex);

    resetState();
    _downloadRequests.clear();

    _peer = qDeserialize(bytes).toMap();
    _inputPeer = TgClient::toInputPeer(_peer);
    cancelUpload();

    _upOffset = _downOffset = qMax(_peer["read_inbox_max_id"].toInt(), _peer["read_outbox_max_id"].toInt());
    fetchMoreUpwards();
    fetchMoreDownwards();
}

QByteArray MessagesModel::peer() const
{
    return qSerialize(_peer);
}

int MessagesModel::rowCount(const QModelIndex &parent) const
{
    return _history.size();
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if (role == IsChannelRole) {
        return TgClient::isChannel(_peer);
    }

    if (role == MergeMessageRole) {
        if (index.row() < 1) {
            return false;
        }

        TgObject curr = _history[index.row()];
        TgObject prev = _history[index.row() - 1];

        if (!TgClient::peersEqual(curr["sender"].toMap(), prev["sender"].toMap())) {
            return false;
        }

        if (!curr["grouped_id"].isNull() && !prev["grouped_id"].isNull()
                && curr["grouped_id"].toLongLong() == prev["grouped_id"].toLongLong()) {
            return true;
        }

        if (!TgClient::isChannel(_peer) && curr["date"].toInt() - prev["date"].toInt() < 300) {
            return true;
        }

        return false;
    }

    return _history[index.row()][roleNames()[role]];
}

bool MessagesModel::canFetchMoreDownwards() const
{
    return _client && _client->isAuthorized() && TgClient::commonPeerType(_inputPeer) != 0 && !_downRequestId.toLongLong() && _downOffset != -1;
}

void MessagesModel::fetchMoreDownwards()
{
    QMutexLocker lock(&_mutex);

    _downRequestId = _client->messagesGetHistory(_inputPeer, _downOffset, 0, -BATCH_SIZE, BATCH_SIZE);
}

bool MessagesModel::canFetchMoreUpwards() const
{
    return _client && _client->isAuthorized() && TgClient::commonPeerType(_inputPeer) != 0 && !_upRequestId.toLongLong() && _upOffset != -1;
}

void MessagesModel::fetchMoreUpwards()
{
    QMutexLocker lock(&_mutex);

    _upRequestId = _client->messagesGetHistory(_inputPeer, _upOffset, 0, 0, BATCH_SIZE);
}

void MessagesModel::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&_mutex);

    if (_userId != userId) {
        resetState();
        cancelUpload();
        _downloadRequests.clear();
        _userId = userId;
    }
}

void MessagesModel::messagesGetHistoryResponse(TgObject data, TgLongVariant messageId)
{
    QMutexLocker lock(&_mutex);

    if (messageId == _downRequestId) {
        handleHistoryResponse(data, messageId);
        _downRequestId = 0;
        return;
    }

    if (messageId == _upRequestId) {
        handleHistoryResponseUpwards(data, messageId);
        _upRequestId = 0;
        return;
    }
}

void MessagesModel::handleHistoryResponse(TgObject data, TgLongVariant messageId)
{
    TgList messages = data["messages"].toList();
    TgList chats = data["chats"].toList();
    TgList users = data["users"].toList();

    _globalUsers.append(users);
    _globalChats.append(chats);

    if (messages.isEmpty()) {
        _downOffset = -1;
        return;
    }

    QList<TgObject> messagesRows;
    messagesRows.reserve(messages.size());

    for (qint32 i = messages.size() - 1; i >= 0; --i) {
        TgObject message = messages[i].toMap();
        TgObject fromId = message["from_id"].toMap();
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
        if (TgClient::commonPeerType(fromId) == 0) {
            sender = _peer;
        }

        messagesRows.append(createRow(message, sender, users, chats));
    }

    qint32 oldOffset = _downOffset;
    qint32 newOffset = messages.first().toMap()["id"].toInt();
    if (_downOffset != newOffset && messages.size() == BATCH_SIZE) {
        _downOffset = newOffset;
    } else {
        _downOffset = -1;
    }

    qint32 oldSize = _history.size();

    beginInsertRows(QModelIndex(), _history.size(), _history.size() + messagesRows.size() - 1);
    _history.append(messagesRows);
    endInsertRows();

    if (oldSize > 0) {
        emit dataChanged(index(oldSize - 1), index(oldSize - 1));
    }

    // aka it is the first time when history is loaded in chat
    if (qMax(_peer["read_inbox_max_id"].toInt(), _peer["read_outbox_max_id"].toInt()) == oldOffset) {
        emit scrollTo(_history.size() - 1);
    }


    if (_avatarDownloader) {
        for (qint32 i = 0; i < users.size(); ++i) {
            _avatarDownloader->downloadAvatar(users[i].toMap());
        }
        for (qint32 i = 0; i < chats.size(); ++i) {
            _avatarDownloader->downloadAvatar(chats[i].toMap());
        }
    }
}

void MessagesModel::handleHistoryResponseUpwards(TgObject data, TgLongVariant messageId)
{
    TgList messages = data["messages"].toList();
    TgList chats = data["chats"].toList();
    TgList users = data["users"].toList();

    _globalUsers.append(users);
    _globalChats.append(chats);

    if (messages.isEmpty()) {
        _upOffset = -1;
        return;
    }

    QList<TgObject> messagesRows;
    messagesRows.reserve(messages.size());

    for (qint32 i = messages.size() - 1; i >= 0; --i) {
        TgObject message = messages[i].toMap();
        TgObject fromId = message["from_id"].toMap();
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
        if (TgClient::commonPeerType(fromId) == 0) {
            //This means that it is a channel feed or personal messages.
            //Authorized user is returned by API, so we don't need to put it manually.
            sender = _peer;
        }

        messagesRows.append(createRow(message, sender, users, chats));
    }

    qint32 oldOffset = _upOffset;
    qint32 newOffset = messages.last().toMap()["id"].toInt();
    if (_upOffset != newOffset && messages.size() == BATCH_SIZE) {
        _upOffset = newOffset;
    } else {
        _upOffset = -1;
    }

    qint32 oldSize = _history.size();

    beginInsertRows(QModelIndex(), 0, messagesRows.size() - 1);
    _history = messagesRows + _history;
    endInsertRows();

    if (oldSize > 0) {
        emit dataChanged(index(messagesRows.size()), index(messagesRows.size()));
    }

    // aka it is the first time when history is loaded in chat
    if (qMax(_peer["read_inbox_max_id"].toInt(), _peer["read_outbox_max_id"].toInt()) == oldOffset) {
        emit scrollTo(_history.size() - 1);
    } else {
        emit scrollTo(messagesRows.size());
    }

    if (_avatarDownloader) {
        for (qint32 i = 0; i < users.size(); ++i) {
            _avatarDownloader->downloadAvatar(users[i].toMap());
        }
        for (qint32 i = 0; i < chats.size(); ++i) {
            _avatarDownloader->downloadAvatar(chats[i].toMap());
        }
        for (qint32 i = 0; i < messages.size(); ++i) {
            TgObject photo = messages[i].toMap()["media"].toMap()["photo"].toMap();
            if (photo["id"].toLongLong() != 0)
                _avatarDownloader->downloadPhoto(photo);
        }
    }
}

TgObject MessagesModel::createRow(TgObject message, TgObject sender, TgList users, TgList chats)
{
    TgObject row;
    row["messageId"] = message["id"];

    if (TgClient::isUser(sender)) {
        row["senderName"] = QString(sender["first_name"].toString() + " " + sender["last_name"].toString());
    } else {
        row["senderName"] = sender["title"].toString();
    }

    //TODO post author

    row["thumbnailColor"] = AvatarDownloader::userColor(sender["id"].toLongLong());
    row["thumbnailText"] = AvatarDownloader::getAvatarText(row["senderName"].toString());
    row["avatar"] = "";
    row["photoId"] = sender["photo"].toMap()["photo_id"];

    row["senderName"] = QString("<html><span style=\"color: "
            + AvatarDownloader::userColor(sender["id"]).name()
            + "\">"
            + row["senderName"].toString()
            + "</span></html>");

    row["date"] = message["date"];
    row["grouped_id"] = message["grouped_id"];
    //TODO 12-hour format
    row["messageTime"] = QDateTime::fromTime_t(qMax(message["date"].toInt(), message["edit_date"].toInt())).toString("hh:mm");
    //TODO replies support
    row["messageText"] = messageToHtml(message["message"].toString(), message["entities"].toList());
    if (GETID(message) == MessageService) {
        //TODO service messages
        row["messageText"] = "<html><i>service messages are not supported yet</i></html>";
    }
    row["sender"] = TgClient::toInputPeer(sender);

    TgObject fwdFrom = message["fwd_from"].toMap();
    row["forwardedFrom"] = "";
    if (EXISTS(fwdFrom)) {
        QString forwardedFrom = fwdFrom["from_name"].toString();

        if (forwardedFrom.isEmpty()) {
            TgObject fwdPeer = fwdFrom["from_id"].toMap();

            if (TgClient::isUser(fwdPeer)) for (qint32 i = 0; i < users.size(); ++i) {
                TgObject realPeer = users[i].toMap();
                if (TgClient::peersEqual(fwdPeer, realPeer)) {
                    forwardedFrom = QString(realPeer["first_name"].toString() + " " + realPeer["last_name"].toString());
                    break;
                }
            }
            if (TgClient::isChat(fwdPeer)) for (qint32 i = 0; i < chats.size(); ++i) {
                TgObject realPeer = chats[i].toMap();
                if (TgClient::peersEqual(fwdPeer, realPeer)) {
                    forwardedFrom = realPeer["title"].toString();
                    break;
                }
            }
        }

        row["forwardedFrom"] = forwardedFrom;
    }

    TgObject media = message["media"].toMap();
    row["hasMedia"] = GETID(media) != 0;
    row["mediaDownloadable"] = false;

    row["mediaUrl"] = "";
    switch (GETID(media)) {
    case MessageMediaPhoto:
    {
        row["hasMedia"] = false;
        row["photoFile"] = "";
        row["photoFileId"] = media["photo"].toMap()["id"].toLongLong();
        row["hasPhoto"] = row["photoFileId"].toLongLong() != 0;
        break;
    }
    case MessageMediaContact:
    {
        row["mediaImage"] = "../../img/media/account.png";
        QString contactName;

        contactName += media["first_name"].toString();
        contactName += " ";
        contactName += media["last_name"].toString();

        row["mediaTitle"] = contactName;
        row["mediaText"] = media["phone_number"].toString();
        break;
    }
    case MessageMediaUnsupported:
        row["mediaImage"] = "../../img/media/file.png";
        row["mediaTitle"] = "Unsupported media";
        row["mediaText"] = "update your app";
        break;
    case MessageMediaDocument:
    {
        row["mediaImage"] = "../../img/media/file.png";
        row["mediaDownloadable"] = true;
        row["mediaDownload"] = media;

        TgObject document = media["document"].toMap();
        QString documentName = "Unknown file";

        TgList attributes = document["attributes"].toList();
        for (qint32 i = 0; i < attributes.size(); ++i) {
            TgObject attribute = attributes[i].toMap();
            if (GETID(attribute) == DocumentAttributeFilename) {
                documentName = attribute["file_name"].toString();
                break;
            }
        }

        qint64 size = document["size"].toLongLong();
        QString sizeString;

        if (size > 1073741824) {
            sizeString = QString::number((long double) size / 1073741824L, 'f', 2);
            sizeString += " GB";
        } else if (size > 1048576) {
            sizeString = QString::number((long double) size / 1048576L, 'f', 2);
            sizeString += " MB";
        } else if (size > 1024) {
            sizeString = QString::number((long double) size / 1024L, 'f', 2);
            sizeString += " KB";
        } else {
            sizeString = QString::number((long double) size, 'f', 2);
            sizeString += " B";
        }

        row["mediaTitle"] = documentName;
        row["mediaFileName"] = documentName;
        row["mediaText"] = sizeString;
        break;
    }
    case MessageMediaWebPage:
        row["mediaImage"] = "../../img/media/web.png";
        row["mediaTitle"] = "Webpage";
        row["mediaText"] = media["webpage"].toMap()["title"].toString();
        if (row["mediaText"].toString().isEmpty()) row["mediaText"] = "unknown link";
        row["mediaUrl"] = media["webpage"].toMap()["url"].toString();
        break;
    case MessageMediaVenue:
        row["mediaImage"] = "../../img/media/map-marker.png";
        row["mediaTitle"] = "Venue";
        row["mediaText"] = media["title"].toString();
        break;
    case MessageMediaGame:
        row["mediaImage"] = "../../img/media/gamepad-square.png";
        row["mediaTitle"] = "Game";
        row["mediaText"] = media["game"].toMap()["title"].toString();
        break;
    case MessageMediaInvoice:
        row["mediaImage"] = "../../img/media/receipt-text.png";
        row["mediaTitle"] = media["title"].toString();
        row["mediaText"] = media["description"].toString();
        break;
    case MessageMediaGeo:
    case MessageMediaGeoLive:
    {
        row["mediaImage"] = "../../img/media/map-marker.png";
        row["mediaTitle"] = GETID(media) == MessageMediaGeoLive ? "Live geolocation" : "Geolocation";

        TgObject geo = media["geo"].toMap();
        QString geoText;
        geoText += geo["long"].toString();
        geoText += ", ";
        geoText += geo["lat"].toString();

        row["mediaText"] = geoText;
        break;
    }
    case MessageMediaPoll:
        row["mediaImage"] = "../../img/media/poll.png";
        row["mediaTitle"] = "Poll";
        row["mediaText"] = media["poll"].toMap()["public_voters"].toBool() ? "public" : "anonymous";
        break;
    case MessageMediaDice:
        row["mediaImage"] = "../../img/media/dice-multiple.png";
        row["mediaTitle"] = "Dice";
        row["mediaText"] = media["value"].toString();
        break;
    }

    return row;
}

void MessagesModel::linkActivated(QString link, qint32 listIndex)
{
    QMutexLocker lock(&_mutex);

    QUrl url(link);

    if (url.scheme() == "kutegram") {
        if (url.host() == "spoiler") {
            TgObject listItem = _history[listIndex];
            QDomDocument dom;

            if (!dom.setContent(listItem["messageText"].toString(), false)) {
                return;
            }

            QDomNodeList list = dom.elementsByTagName("a");
            for (qint32 i = 0; i < list.count(); ++i) {
                QDomElement node = list.at(i).toElement();
                if (node.attribute("href") == link) {
                    node.removeAttribute("href");
                    node.removeAttribute("style");
                    break;
                }
            }

            listItem["messageText"] = dom.toString(-1);
            _history[listIndex] = listItem;

            emit dataChanged(index(listIndex), index(listIndex));
        }
    } else if (url.scheme().isEmpty()) {
        openUrl("http://" + link);
    } else {
        openUrl(link);
    }
}

void MessagesModel::openUrl(QString url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void MessagesModel::avatarDownloaded(TgLongVariant photoId, QString filePath)
{
    QMutexLocker lock(&_mutex);

    for (qint32 i = 0; i < _history.size(); ++i) {
        TgObject message = _history[i];

        if (message["photoId"] != photoId) {
            continue;
        }

        message["avatar"] = filePath;
        _history[i] = message;

        emit dataChanged(index(i), index(i));
    }
}

void MessagesModel::photoDownloaded(TgLongVariant photoId, QString filePath)
{
    QMutexLocker lock(&_mutex);

    for (qint32 i = 0; i < _history.size(); ++i) {
        TgObject message = _history[i];

        if (message["photoFileId"] != photoId) {
            continue;
        }

        message["photoFile"] = filePath;
        _history[i] = message;

        emit dataChanged(index(i), index(i));
    }
}

void MessagesModel::downloadFile(qint32 index)
{
    QMutexLocker lock(&_mutex);

    if (!_client || !_client->isAuthorized() || TgClient::commonPeerType(_peer) == 0 || index == -1) {
        return;
    }

    cancelDownload(index);

    QDir::home().mkdir("Kutegram");

    QString fileName = _history[index]["mediaFileName"].toString();
    if (fileName.isEmpty()) fileName = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());

    QStringList split = fileName.split('.');
    QString fileNameBefore;
    QString fileNameAfter;

    if (split.length() == 1) {
        fileNameBefore = split.first();
    } else {
        fileNameBefore = QStringList(split.mid(0, split.length() - 1)).join(".");
        fileNameAfter = split.last();
    }

    if (!fileNameAfter.isEmpty()) {
        fileNameAfter = "." + fileNameAfter;
    }

#if QT_VERSION < 0x050000
    QDir dir(QDir::homePath() + "/Downloads/");
#else
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
#endif
    dir.mkpath("Kutegram/");

    QString indexedFileName = fileName;
    QString indexedFilePath = dir.absoluteFilePath("Kutegram/" + indexedFileName);
    qint32 fileIndex = 0;

    while (QFile(indexedFilePath).exists()) {
        ++fileIndex;
        indexedFileName = fileNameBefore + " (" + QString::number(fileIndex) + ")" + fileNameAfter;
        indexedFilePath = dir.absoluteFilePath("Kutegram/" + indexedFileName);
    }

    qint32 messageId = _history[index]["messageId"].toInt();
    qint64 requestId = _client->downloadFile(indexedFilePath, _history[index]["mediaDownload"].toMap()).toLongLong();
    _downloadRequests.insert(requestId, messageId);
    emit downloadUpdated(messageId, 0, "");
}

void MessagesModel::cancelDownload(qint32 index)
{
    QMutexLocker lock(&_mutex);

    if (!_client) {
        return;
    }

    qint32 messageId = _history[index]["messageId"].toInt();
    qint64 requestId = _downloadRequests.key(messageId);
    _downloadRequests.remove(requestId);
    _client->cancelDownload(requestId);
    emit downloadUpdated(messageId, -1, "");
}

void MessagesModel::fileDownloaded(TgLongVariant fileId, QString filePath)
{
    QMutexLocker lock(&_mutex);

    TgVariant messageId = _downloadRequests.take(fileId.toLongLong());

    if (messageId.isNull()) {
        return;
    }

    emit downloadUpdated(messageId.toInt(), 1, "file:///" + filePath);
}

void MessagesModel::fileDownloadCanceled(TgLongVariant fileId, QString filePath)
{
    QMutexLocker lock(&_mutex);

    TgVariant messageId = _downloadRequests.take(fileId.toLongLong());

    if (messageId.isNull()) {
        return;
    }

    emit downloadUpdated(messageId.toInt(), -1, "");
}

void MessagesModel::gotMessageUpdate(TgObject update, TgLongVariant messageId)
{
    QMutexLocker lock(&_mutex);

    if (_downOffset != -1) {
        return;
    }

    TgObject peerId;
    TgObject fromId;
    qint64 fromIdNumeric;

    if (_sentMessages.contains(messageId.toLongLong())) {
        if (TgClient::isChannel(_peer)) {
            ID_PROPERTY(peerId) = TLType::PeerChannel;
            peerId["channel_id"] = TgClient::getPeerId(_peer);
        } else if (TgClient::isChat(_peer)) {
            ID_PROPERTY(peerId) = TLType::PeerChat;
            peerId["chat_id"] = TgClient::getPeerId(_peer);
        } else {
            ID_PROPERTY(peerId) = TLType::PeerUser;
            peerId["user_id"] = TgClient::getPeerId(_peer);
        }

        update["message"] = _sentMessages.take(messageId.toLongLong());

        fromIdNumeric = _client->getUserId().toLongLong();
    } else if (update["user_id"].toLongLong() == TgClient::getPeerId(_peer) && TgClient::isUser(_peer)) {
        ID_PROPERTY(peerId) = TLType::PeerUser;
        peerId["user_id"] = update["user_id"];

        fromIdNumeric = update["out"].toBool() ? _client->getUserId().toLongLong() : update["user_id"].toLongLong();
    } else if (update["chat_id"].toLongLong() == TgClient::getPeerId(_peer) && TgClient::isChat(_peer)) {
        ID_PROPERTY(peerId) = TLType::PeerChat;
        peerId["chat_id"] = update["chat_id"];

        fromIdNumeric = update["from_id"].toLongLong();
    } else {
        return;
    }

    TgObject sender;
    for (qint32 j = 0; j < _globalUsers.size(); ++j) {
        TgObject peer = _globalUsers[j].toMap();
        if (TgClient::getPeerId(peer) == fromIdNumeric) {
            sender = peer;
            break;
        }
    }
    if (ID(sender) == 0) for (qint32 j = 0; j < _globalChats.size(); ++j) {
        TgObject peer = _globalChats[j].toMap();
        if (TgClient::getPeerId(peer) == fromIdNumeric) {
            sender = peer;
            break;
        }
    }

    if (TgClient::isChannel(sender)) {
        ID_PROPERTY(fromId) = TLType::PeerChannel;
        fromId["channel_id"] = TgClient::getPeerId(sender);
    } else if (TgClient::isChat(sender)) {
        ID_PROPERTY(fromId) = TLType::PeerChat;
        fromId["chat_id"] = TgClient::getPeerId(sender);
    } else if (TgClient::isUser(sender)) {
        ID_PROPERTY(fromId) = TLType::PeerUser;
        fromId["user_id"] = TgClient::getPeerId(sender);
    }

    update["peer_id"] = peerId;
    update["from_id"] = fromId;

    qint32 oldSize = _history.size();

    beginInsertRows(QModelIndex(), _history.size(), _history.size());
    _history.append(createRow(update, sender, _globalUsers, _globalChats));
    endInsertRows();

    if (oldSize > 0) {
        emit dataChanged(index(oldSize - 1), index(oldSize - 1));
    }

    _avatarDownloader->downloadAvatar(sender);

    emit scrollForNew();
}

void MessagesModel::gotUpdate(TgObject update, TgLongVariant messageId, TgList users, TgList chats, qint32 date, qint32 seq, qint32 seqStart)
{
    QMutexLocker lock(&_mutex);

    _globalUsers.append(users);
    _globalChats.append(chats);

    switch (ID(update)) {
    case TLType::UpdateNewMessage:
    case TLType::UpdateNewChannelMessage:
    {
        if (_downOffset != -1) {
            return;
        }

        TgObject message = update["message"].toMap();

        if (!TgClient::peersEqual(_peer, message["peer_id"].toMap())) {
            return;
        }

        TgObject fromId = message["from_id"].toMap();
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
        if (TgClient::commonPeerType(fromId) == 0) {
            //This means that it is a channel feed or personal messages.
            //Authorized user is returned by API, so we don't need to put it manually.
            sender = _peer;
        }

        qint32 oldSize = _history.size();

        beginInsertRows(QModelIndex(), _history.size(), _history.size());
        _history.append(createRow(message, sender, users, chats));
        endInsertRows();

        if (oldSize > 0) {
            emit dataChanged(index(oldSize - 1), index(oldSize - 1));
        }

        _avatarDownloader->downloadAvatar(sender);

        emit scrollForNew();
        break;
    }
    case TLType::UpdateEditMessage:
    case TLType::UpdateEditChannelMessage:
    {
        TgObject message = update["message"].toMap();

        if (!TgClient::peersEqual(_peer, message["peer_id"].toMap())) {
            return;
        }

        qint32 rowIndex = -1;
        for (qint32 i = 0; i < _history.size(); ++i) {
            if (_history[i]["messageId"] == message["id"]) {
                rowIndex = i;
                break;
            }
        }

        if (rowIndex == -1) {
            return;
        }

        TgObject fromId = message["from_id"].toMap();
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
        if (TgClient::commonPeerType(fromId) == 0) {
            //This means that it is a channel feed or personal messages.
            //Authorized user is returned by API, so we don't need to put it manually.
            sender = _peer;
        }

        _history.replace(rowIndex, createRow(message, sender, users, chats));

        emit dataChanged(index(rowIndex), index(rowIndex));

        _avatarDownloader->downloadAvatar(sender);
        break;
    }
    case TLType::UpdateDeleteChannelMessages:
        if (!TgClient::isChat(_peer) || TgClient::getPeerId(_peer) != update["channel_id"].toLongLong()) {
            return;
        }

        //fallthrough
    case TLType::UpdateDeleteMessages:
        TgList ids = update["messages"].toList();
        for (qint32 i = 0; i < _history.size(); ++i) {
            if (ids.removeOne(_history[i]["messageId"])) {
                beginRemoveRows(QModelIndex(), i, i);
                _history.removeAt(i);
                endRemoveRows();
                --i;
            }
        }

        break;
    }
}

void MessagesModel::sendMessage(QString message)
{
    if (!_client || !_client->isAuthorized() || TgClient::commonPeerType(_inputPeer) == 0 || _uploadId.toLongLong() || (message.isEmpty() && GETID(_media["file"].toMap()) == 0)) {
        return;
    }

    _sentMessages.insert(_client->messagesSendMessage(_inputPeer, message, _media).toLongLong(), message);
    cancelUpload();
}

void MessagesModel::uploadFile()
{
    cancelUpload();

    QString selected = QFileDialog::getOpenFileName();

    if (selected.isEmpty()) {
        return;
    }

    if (selected.endsWith(".jpg") || selected.endsWith(".jpeg") || selected.endsWith(".png")) {
        ID_PROPERTY(_media) = TLType::InputMediaUploadedPhoto;
    } else {
        ID_PROPERTY(_media) = TLType::InputMediaUploadedDocument;

        TGOBJECT(TLType::DocumentAttributeFilename, fileName);
        fileName["file_name"] = selected.split('/').last();

        TgList attributes;
        attributes << fileName;
        _media["attributes"] = attributes;
    }

    _uploadId = _client->uploadFile(selected);

    emit uploadingProgress(0);
}

void MessagesModel::cancelUpload()
{
    _media = TgObject();
    if (_uploadId.toLongLong()) {
        _client->cancelUpload(_uploadId);
    }
    _uploadId = 0;

    emit uploadingProgress(-1);
}

void MessagesModel::fileUploadCanceled(TgLongVariant fileId)
{
    if (_uploadId != fileId) {
        return;
    }

    _uploadId = 0;
    cancelUpload();
}

void MessagesModel::fileUploaded(TgLongVariant fileId, TgObject inputFile)
{
    if (_uploadId != fileId) {
        return;
    }

    _uploadId = 0;
    _media["file"] = inputFile;

    emit uploadingProgress(100);
}

void MessagesModel::fileUploading(TgLongVariant fileId, TgLongVariant processedLength, TgLongVariant totalLength, qint32 progressPercentage)
{
    if (_uploadId != fileId) {
        return;
    }

    emit uploadingProgress(progressPercentage);
}
