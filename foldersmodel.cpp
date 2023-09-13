#include "foldersmodel.h"

#include "tlschema.h"
#include <QMutexLocker>

FoldersModel::FoldersModel(QObject *parent)
    : QAbstractListModel(parent)
    , _mutex(QMutex::Recursive)
    , _folders()
    , _client(0)
    , _userId(0)
    , _requestId(0)
{
#if QT_VERSION < 0x050000
    setRoleNames(roleNames());
#endif
}

void FoldersModel::resetState()
{
    if (!_folders.isEmpty()) {
        beginRemoveRows(QModelIndex(), 0, _folders.size() - 1);
        _folders.clear();
        endRemoveRows();
    }

    _requestId = 0;
}

QHash<int, QByteArray> FoldersModel::roleNames() const
{
    static QHash<int, QByteArray> roles;

    if (!roles.isEmpty())
        return roles;

    roles[TitleRole] = "title";
    roles[IconRole] = "icon";
    roles[FolderIndexRole] = "folderIndex";

    return roles;
}

void FoldersModel::setClient(QObject *client)
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
    connect(_client, SIGNAL(messagesGetDialogFiltersResponse(TgVector,TgLongVariant)), this, SLOT(messagesGetDialogFiltersResponse(TgVector,TgLongVariant)));
}

QObject* FoldersModel::client() const
{
    return _client;
}

int FoldersModel::rowCount(const QModelIndex &parent) const
{
    return _folders.size();
}

QVariant FoldersModel::data(const QModelIndex &index, int role) const
{
    if (role == FolderIndexRole) {
        return index.row();
    }

    return _folders[index.row()][roleNames()[role]];
}

bool FoldersModel::canFetchMoreDownwards() const
{
    return _client && _client->isAuthorized() && !_requestId.toLongLong() && _folders.isEmpty();
}

void FoldersModel::fetchMoreDownwards()
{
    QMutexLocker lock(&_mutex);

    _requestId = _client->messagesGetDialogFilters();
}

void FoldersModel::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&_mutex);

    if (_userId != userId) {
        resetState();
        _userId = userId;
        fetchMoreDownwards();
    }
}

void FoldersModel::messagesGetDialogFiltersResponse(TgVector data, TgLongVariant messageId)
{
    QMutexLocker lock(&_mutex);

    if (_requestId != messageId) {
        return;
    }

    _requestId = 0;

    if (data.isEmpty()) {
        return;
    }

    QList<TgObject> rows;
    rows.reserve(data.size());

    for (qint32 i = 0; i < data.size(); ++i) {
        rows.append(createRow(data[i].toMap()));
    }

    beginInsertRows(QModelIndex(), _folders.size(), _folders.size() + rows.size() - 1);
    _folders.append(rows);
    endInsertRows();
}

#define ICON(key, image) map[key] = image;

QHash<QString, QString> getIconsMap()
{
    //Referenced from Telegram Desktop:
    //https://github.com/telegramdesktop/tdesktop/blob/dev/Telegram/SourceFiles/ui/filter_icons.cpp#L17
    QHash<QString, QString> map;

    ICON("\xF0\x9F\x90\xB1", "cat.png");
    ICON("\xF0\x9F\x93\x95", "book.png");
    ICON("\xF0\x9F\x92\xB0", "money.png");
    ICON("\xF0\x9F\x8E\xAE", "game.png");
    ICON("\xF0\x9F\x92\xA1", "light.png");
    ICON("\xF0\x9F\x91\x8C", "like.png");
    ICON("\xF0\x9F\x8E\xB5", "note.png");
    ICON("\xF0\x9F\x8E\xA8", "palette.png");
    ICON("\xE2\x9C\x88\xEF\xB8\x8F", "travel.png");
    ICON("\xE2\x9A\xBD\xEF\xB8\x8F", "sport.png");
    ICON("\xE2\xAD\x90", "favorite.png");
    ICON("\xF0\x9F\x8E\x93", "study.png");
    ICON("\xF0\x9F\x9B\xAB", "airplane.png");
    ICON("\xF0\x9F\x91\xA4", "private.png");
    ICON("\xF0\x9F\x91\xA5", "groups.png");
    ICON("\xF0\x9F\x92\xAC", "all.png");
    ICON("\xE2\x9C\x85", "unread.png");
    ICON("\xF0\x9F\xA4\x96", "bots.png");
    ICON("\xF0\x9F\x91\x91", "crown.png");
    ICON("\xF0\x9F\x8C\xB9", "flower.png");
    ICON("\xF0\x9F\x8F\xA0", "home.png");
    ICON("\xE2\x9D\xA4", "love.png");
    ICON("\xF0\x9F\x8E\xAD", "mask.png");
    ICON("\xF0\x9F\x8D\xB8", "party.png");
    ICON("\xF0\x9F\x93\x88", "trade.png");
    ICON("\xF0\x9F\x92\xBC", "work.png");
    ICON("\xF0\x9F\x94\x94", "unmuted.png");
    ICON("\xF0\x9F\x93\xA2", "channels.png");
    ICON("\xF0\x9F\x93\x81", "custom.png");
    ICON("\xF0\x9F\x93\x8B", "setup.png");

    return map;
}

TgObject FoldersModel::createRow(TgObject filter)
{
    if (GETID(filter) == TLType::DialogFilterDefault) {
        filter["title"] = "All chats";
        filter["icon"] = "../../img/filters/all.png";

        return filter;
    }

    static QHash<QString, QString> iconsMap = getIconsMap();
    QString icon = iconsMap[filter["emoticon"].toString()];
    if (!icon.isEmpty()) {
        filter["icon"] = QString("../../img/filters/" + icon);

        return filter;
    }

    //Referenced from Telegram Desktop:
    //https://github.com/telegramdesktop/tdesktop/blob/dev/Telegram/SourceFiles/ui/filter_icons.cpp#L242

    quint32 flags = filter["flags"].toUInt();

    const quint32 allFlags = 0b11111;

    icon = "custom.png";

    if (!filter["include_peers"].toList().isEmpty()
            || !filter["exclude_peers"].toList().isEmpty()
            || !(flags & allFlags)) {
        icon = "custom.png";
    } else if ((flags & allFlags) == 1
               || (flags & allFlags) == 2
               || (flags & allFlags) == 3) {
        icon = "private.png";
    } else if ((flags & allFlags) == 4) {
        icon = "groups.png";
    } else if ((flags & allFlags) == 8) {
        icon = "channels.png";
    } else if ((flags & allFlags) == 16) {
        icon = "bots.png";
    } else if ((flags & 2048) == 2048) {
        icon = "unread.png";
    } else if ((flags & 4096) == 4096) {
        icon = "unmuted.png";
    }

    filter["icon"] = QString("../../img/filters/" + icon);

    return filter;
}

bool FoldersModel::matches(qint32 index, QByteArray peerBytes)
{
    //TODO: cache this?
    QMutexLocker lock(&_mutex);

    if (_folders.isEmpty()) {
        return false;
    }

    TgObject filter = _folders[index];

    if (GETID(filter) == TLType::DialogFilterDefault) {
        return true;
    }

    TgObject peer = qDeserialize(peerBytes).toMap();

    TgList includePeers = filter["include_peers"].toList();
    for (qint32 i = 0; i < includePeers.size(); ++i) {
        if (TgClient::peersEqual(peer, includePeers[i].toMap())) {
            return true;
        }
    }

    if (GETID(filter) == TLType::DialogFilterChatlist) {
        return false;
    }

    TgList excludePeers = filter["exclude_peers"].toList();
    for (qint32 i = 0; i < excludePeers.size(); ++i) {
        if (TgClient::peersEqual(peer, excludePeers[i].toMap())) {
            return false;
        }
    }

    if (filter["exclude_muted"].toBool() && peer["notify_settings"].toMap()["silent"].toBool()) {
        return false;
    }

    if (filter["exclude_read"].toBool() && !peer["unread_mark"].toBool()
            && !peer["unread_count"].toBool()
            && !peer["unread_mentions_count"].toBool()
            && !peer["unread_reactions_count"].toBool()) {
        return false;
    }

    if (filter["exclude_archived"].toBool() && peer["folder_id"].toBool()) {
        return false;
    }

    if (filter["contacts"].toBool() && TgClient::isUser(peer) && peer["contact"].toBool()) {
        return true;
    }

    if (filter["non_contacts"].toBool() && TgClient::isUser(peer) && !peer["contact"].toBool()) {
        return true;
    }

    if (filter["groups"].toBool() && TgClient::isGroup(peer)) {
        return true;
    }

    if (filter["broadcasts"].toBool() && TgClient::isChannel(peer)) {
        return true;
    }

    if (filter["bots"].toBool() && TgClient::isUser(peer) && peer["bot"].toBool()) {
        return true;
    }

    return false;
}

void FoldersModel::refresh()
{
    resetState();
    fetchMoreDownwards();
}
