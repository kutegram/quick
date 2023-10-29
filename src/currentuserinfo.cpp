#include "currentuserinfo.h"

#include <QMutexLocker>

CurrentUserInfo::CurrentUserInfo(QObject *parent)
    : QObject(parent)
    , _mutex(QMutex::Recursive)
    , _client(0)
    , _userId(0)
    , _requestId(0)
    , _avatarDownloader(0)
{
}

void CurrentUserInfo::setClient(QObject *client)
{
    QMutexLocker lock(&_mutex);

    if (_client) {
        _client->disconnect(this);
    }

    _client = dynamic_cast<TgClient*>(client);
    _userId = 0;

    _requestId = 0;
    emit userInfoChanged("", "", AvatarDownloader::userColor(0), "");

    if (!_client) return;

    connect(_client, SIGNAL(authorized(TgLongVariant)), this, SLOT(authorized(TgLongVariant)));
    connect(_client, SIGNAL(vectorUserResponse(TgVector,TgLongVariant)), this, SLOT(usersGetUsersResponse(TgVector,TgLongVariant)));
}

QObject* CurrentUserInfo::client() const
{
    return _client;
}

void CurrentUserInfo::setAvatarDownloader(QObject *avatarDownloader)
{
    QMutexLocker lock(&_mutex);

    if (_avatarDownloader) {
        _avatarDownloader->disconnect(this);
    }

    _avatarDownloader = dynamic_cast<AvatarDownloader*>(avatarDownloader);

    if (!_avatarDownloader) return;

    connect(_avatarDownloader, SIGNAL(avatarDownloaded(TgLongVariant,QString)), this, SLOT(avatarDownloaded(TgLongVariant,QString)));
}

QObject* CurrentUserInfo::avatarDownloader() const
{
    return _avatarDownloader;
}

void CurrentUserInfo::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&_mutex);

    if (_userId != userId) {
        _requestId = 0;
        emit userInfoChanged("", "", AvatarDownloader::userColor(0), "");
        _userId = userId;
    }
}

void CurrentUserInfo::usersGetUsersResponse(TgVector data, TgLongVariant messageId)
{
    QMutexLocker lock(&_mutex);

    for (qint32 i = 0; i < data.size(); ++i) {
        TgObject obj = data.first().toMap();

        if (_client->getUserId() != TgClient::getPeerId(obj)) {
            continue;
        }

        QString name = obj["first_name"].toString() + " " + obj["last_name"].toString();
        emit userInfoChanged(name, obj["username"].toString(), AvatarDownloader::userColor(obj["id"]), AvatarDownloader::getAvatarText(name));

        if (_avatarDownloader)
            _requestId = _avatarDownloader->downloadAvatar(obj);

        return;
    }
}

void CurrentUserInfo::avatarDownloaded(TgLongVariant photoId, QString filePath)
{
    QMutexLocker lock(&_mutex);

    if (_requestId != photoId) {
        return;
    }

    _requestId = 0;
    emit userAvatarDownloaded(filePath);
}
