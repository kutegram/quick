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

    if (!_client) return;

    connect(_client, SIGNAL(authorized(TgLongVariant)), this, SLOT(authorized(TgLongVariant)));
    connect(_client, SIGNAL(messagesGetHistoryResponse(TgObject,TgLongVariant)), this, SLOT(messagesGetHistoryResponse(TgObject,TgLongVariant)));
    connect(_client, SIGNAL(fileDownloaded(TgLongVariant,QString)), this, SLOT(fileDownloaded(TgLongVariant,QString)));
    connect(_client, SIGNAL(fileDownloadCanceled(TgLongVariant,QString)), this, SLOT(fileDownloadCanceled(TgLongVariant,QString)));
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

    _downRequestId = _client->messagesGetHistory(_inputPeer, _downOffset, 0, -21, 20);
}

bool MessagesModel::canFetchMoreUpwards() const
{
    return _client && _client->isAuthorized() && TgClient::commonPeerType(_inputPeer) != 0 && !_upRequestId.toLongLong() && _upOffset != -1;
}

void MessagesModel::fetchMoreUpwards()
{
    QMutexLocker lock(&_mutex);

    _upRequestId = _client->messagesGetHistory(_inputPeer, _upOffset, 0, -1, 20);
}

void MessagesModel::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&_mutex);

    if (_userId != userId) {
        resetState();
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

    qint32 newOffset = messages.first().toMap()["id"].toInt();
    if (_downOffset != newOffset) {
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
    if (_upOffset != newOffset) {
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
        emit scrollTo(messagesRows.size() - 1);
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

    row["senderName"] = QString("<html><span style=\"color: "
            + AvatarDownloader::userColor(sender["id"]).name()
            + "\">"
            + row["senderName"].toString()
            + "</span></html>");

    row["date"] = message["date"];
    row["grouped_id"] = message["grouped_id"];
    //TODO 12-hour format
    row["messageTime"] = QDateTime::fromTime_t(qMax(message["date"].toInt(), message["edit_date"].toInt())).toString("hh:mm dd.MM.yyyy");
    //TODO replies support
    row["messageText"] = message["message"].toString().isEmpty() ? "" : QString("<html>" + messageToHtml(message["message"].toString(), message["entities"].toList(), false, 0) + "</html>");
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

    //TODO post author

    row["thumbnailColor"] = AvatarDownloader::userColor(sender["id"].toLongLong());
    row["thumbnailText"] = AvatarDownloader::getAvatarText(row["senderName"].toString());
    row["avatar"] = "";
    row["photoId"] = sender["photo"].toMap()["photo_id"];

    TgObject media = message["media"].toMap();
    row["hasMedia"] = GETID(media) != 0 ? 1 : 0;
    row["mediaDownloadable"] = false;

    row["mediaUrl"] = "";
    switch (GETID(media)) {
    //TODO image viewer and preview
    case MessageMediaPhoto:
        row["mediaImage"] = "../../img/media/image.png";
        row["mediaTitle"] = "Image";
        row["mediaText"] = "no preview";
        break;
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
    }
    else openUrl(link);
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

void MessagesModel::downloadFile(qint32 index)
{
    QMutexLocker lock(&_mutex);

    if (!_client || !_client->isAuthorized() || TgClient::commonPeerType(_peer) == 0 || index == -1) {
        return;
    }

    cancelDownload(index);

    QDir::home().mkdir("Kutegram");

    QString fileName =  _history[index]["mediaFileName"].toString();
    if (fileName.isEmpty()) fileName = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());

    QStringList split = fileName.split('.');
    QString fileNameBefore;
    QString fileNameAfter;

    if (split.length() == 1) {
        fileNameBefore = split.first();
    } else {
        fileNameBefore = QStringList(split.mid(0, split.length() - 1)).join(".");
        fileNameAfter  = split.last();
    }

    if (!fileNameAfter.isEmpty()) {
        fileNameAfter = "." + fileNameAfter;
    }

#if QT_VERSION < 0x050000
    QDir dir = QDir::home();
#else
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation));
#endif

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

