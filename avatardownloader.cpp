#include "avatardownloader.h"

#include <QCoreApplication>

AvatarDownloader::AvatarDownloader(QObject *parent)
     : QObject(parent)
     , _mutex(QMutex::Recursive)
     , _client(0)
     , _userId(0)
     , _requests()
     , _downloadedAvatars()
{
}

void AvatarDownloader::saveDatabase()
{
    if (!_client) {
        return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName() + "_avatars");
    settings.setValue("DownloadedAvatars", _downloadedAvatars);
}

void AvatarDownloader::readDatabase()
{
    if (!_client) {
        return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName() + "_avatars");
    _downloadedAvatars = settings.value("DownloadedAvatars").toList();
}

void AvatarDownloader::setClient(QObject *client)
{
    QMutexLocker lock(&_mutex);

    if (_client) {
        _client->disconnect(this);
        saveDatabase();
    }

    _client = dynamic_cast<TgClient*>(client);
    _userId = _client->getUserId();

    _requests.clear();
    readDatabase();

    if (!_client) return;

    _client->cacheDirectory().mkdir("avatars");

    connect(_client, SIGNAL(authorized(TgLongVariant)), this, SLOT(authorized(TgLongVariant)));
    connect(_client, SIGNAL(fileDownloaded(TgLongVariant,QString)), this, SLOT(fileDownloaded(TgLongVariant,QString)));
    connect(_client, SIGNAL(fileDownloadCanceled(TgLongVariant,QString)), this, SLOT(fileDownloadCanceled(TgLongVariant,QString)));
}

void AvatarDownloader::authorized(TgLongVariant userId)
{
    QMutexLocker lock(&_mutex);

    if (_userId != userId) {
        _requests.clear();
    }

    _userId = userId;
}

QObject* AvatarDownloader::client() const
{
    return _client;
}

qint64 AvatarDownloader::downloadAvatar(TgObject peer)
{
    QMutexLocker lock(&_mutex);

    if (!_client || !_client->isAuthorized() || TgClient::commonPeerType(peer) == 0) {
        return 0;
    }

    TgObject photo = peer["photo"].toMap();
    if (GETID(photo) == 0) {
        return 0;
    }

    qint64 photoId = photo["photo_id"].toLongLong();

    QString relativePath = "avatars/" + QString::number(photoId) + ".jpg";
    QString avatarFilePath = _client->cacheDirectory().absoluteFilePath(relativePath);

    if (!_downloadedAvatars.contains(photoId)) {
        qint64 loadingId = _client->downloadFile(avatarFilePath, peer).toLongLong();
        _requests[loadingId] = photoId;
    } else {
#if QT_VERSION >= 0x050000
        emit avatarDownloaded(photoId, "file://" + avatarFilePath);
#else
        emit avatarDownloaded(photoId, avatarFilePath);
#endif
    }

    return photoId;
}

void AvatarDownloader::fileDownloaded(TgLongVariant fileId, QString filePath)
{
    QMutexLocker lock(&_mutex);

    TgLongVariant photoId = _requests[fileId.toLongLong()];

    if (photoId.isNull()) {
        return;
    }

    _downloadedAvatars.append(photoId);
    saveDatabase();
#if QT_VERSION >= 0x050000
    emit avatarDownloaded(photoId, "file://" + filePath);
#else
    emit avatarDownloaded(photoId, filePath);
#endif
}

void AvatarDownloader::fileDownloadCanceled(TgLongVariant fileId, QString filePath)
{
    QMutexLocker lock(&_mutex);

    _requests.remove(fileId.toLongLong());
}

QString AvatarDownloader::getAvatarText(QString title)
{
    QStringList split = title.split(" ", QString::SkipEmptyParts);
    QString result;

    for (qint32 i = 0; i < split.size(); ++i) {
        QString item = split[i];
        for (qint32 j = 0; j < item.length(); ++j) {
            if (item[j].isLetterOrNumber()) {
                result += item[j].toUpper();
                break;
            }
        }

        if (result.size() > 1) {
            break;
        }
    }

    if (result.isEmpty() && !title.isEmpty())
        result += title[0].toUpper();

    return result;
}

QColor AvatarDownloader::userColor(TgLongVariant id)
{
   return QColor::fromHsl(id.toLongLong() % 360, 160, 160);
}
