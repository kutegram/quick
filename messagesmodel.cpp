#include "messagesmodel.h"

#include "tlschema.h"
#include <QMutexLocker>
#include <QColor>
#include <QDateTime>

MessagesModel::MessagesModel(QObject *parent)
    : QAbstractListModel(parent)
    , mutex(QMutex::Recursive)
    , _client(0)
    , _userId(0)
    , _peer()
    , _upRequestId(0)
    , _downRequestId(0)
    , _upOffset(0)
    , _downOffset(0)
{
    QHash<int, QByteArray> roles;
    roles[PeerNameRole] = "peerName";
    roles[MessageTextRole] = "messageText";
    roles[MergeMessageRole] = "mergeMessage";
    roles[MessageTimeRole] = "messageTime";
    roles[SenderNameRole] = "senderName";
    setRoleNames(roles);
}

void MessagesModel::resetState()
{
    if (!_history.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, _history.size() - 1);
        _history.clear();
        endRemoveRows();
    }

    _peer = TgObject();
    _upRequestId = 0;
    _downRequestId = 0;
    _upOffset = 0;
    _downOffset = 0;
}

void MessagesModel::setClient(QObject *client)
{
    QMutexLocker lock(&mutex);

    if (_client) {
        _client->disconnect(this);
    }

    _client = dynamic_cast<TgClient*>(client);
    _userId = 0;

    resetState();

    if (!_client) return;

    connect(_client, SIGNAL(authorized(TgLongVariant)), this, SLOT(authorized(TgLongVariant)));
    connect(_client, SIGNAL(messagesGetHistoryResponse(TgObject,TgLongVariant)), this, SLOT(messagesGetHistoryResponse(TgObject,TgLongVariant)));
}

QObject* MessagesModel::client() const
{
    return _client;
}

void MessagesModel::setPeer(QByteArray bytes)
{
    QMutexLocker lock(&mutex);

    resetState();

    TgObject peer;
    QDataStream peerStream(&bytes, QIODevice::ReadOnly);
    peerStream >> peer;

    _peer = peer;

    _upOffset = _downOffset = qMax(peer["read_inbox_max_id"].toInt(), peer["read_outbox_max_id"].toInt());
    fetchMoreUpwards();
    fetchMore(QModelIndex());
}

QByteArray MessagesModel::peer() const
{
    QByteArray array;
    QDataStream peerStream(&array, QIODevice::WriteOnly);
    peerStream << _peer;
    return array;
}

int MessagesModel::rowCount(const QModelIndex &parent) const
{
    return _history.size();
}

QVariant MessagesModel::data(const QModelIndex &index, int role) const
{
    if (role == MergeMessageRole) {
        if (index.row() < 1) {
            return false;
        }

        TgObject curr = _history[index.row()];
        TgObject prev = _history[index.row() - 1];

        return (curr["grouped_id"] == prev["grouped_id"]
                || curr["date"].toInt() - prev["date"].toInt() < 300)
                && curr["sender"] == prev["sender"];
    }

    return _history[index.row()][roleNames()[role]];
}

bool MessagesModel::canFetchMore(const QModelIndex &parent) const
{
    return _client && _userId.toLongLong() && TgClient::commonPeerType(_peer) != 0 && !_downRequestId.toLongLong() && _downOffset != -1;
}

void MessagesModel::fetchMore(const QModelIndex &parent)
{
    QMutexLocker lock(&mutex);

    _downRequestId = _client->messagesGetHistory(_peer, _downOffset, 0, -20, 20);
}

bool MessagesModel::canFetchMoreUpwards() const
{
    return _client && _userId.toLongLong() && TgClient::commonPeerType(_peer) != 0 && !_upRequestId.toLongLong() && _upOffset != -1;
}

void MessagesModel::fetchMoreUpwards()
{
    QMutexLocker lock(&mutex);

    _upRequestId = _client->messagesGetHistory(_peer, _upOffset, 0, 0, 20);
}

void MessagesModel::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&mutex);

    if (_userId != userId) {
        resetState();
    }

    _userId = userId;
}

void MessagesModel::messagesGetHistoryResponse(TgObject data, TgLongVariant messageId)
{
    QMutexLocker lock(&mutex);

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

        messagesRows.append(createRow(message, sender));
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

    if (oldSize > 0)
        emit dataChanged(index(oldSize - 1), index(oldSize - 1));
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

        messagesRows.append(createRow(message, sender));
    }

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

    if (oldSize > 0)
        emit dataChanged(index(messagesRows.size()), index(messagesRows.size()));
}

TgObject MessagesModel::createRow(TgObject message, TgObject sender)
{
    TgObject row;

    if (TgClient::isUser(sender)) {
        row["senderName"] = sender["first_name"].toString() + " " + sender["last_name"].toString();
    } else {
        row["senderName"] = sender["title"].toString();
    }

    row["date"] = message["date"];
    row["grouped_id"] = message["grouped_id"];
    row["messageTime"] = QDateTime::fromTime_t(qMax(message["date"].toInt(), message["edit_date"].toInt())).toString("hh:mm");
    row["messageText"] = message["message"];
    row["sender"] = TgClient::toInputPeer(sender);

    return row;
}
