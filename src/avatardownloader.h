#ifndef AVATARDOWNLOADER_H
#define AVATARDOWNLOADER_H

#include <QObject>

#include <QMutex>
#include <QColor>
#include <QSettings>
#include "tgclient.h"

class AvatarDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient)

private:
    QMutex _mutex;
    TgClient* _client;
    TgLongVariant _userId;
    QHash<qint64, TgLongVariant> _requestsAvatars;
    QHash<qint64, TgLongVariant> _requestsPhotos;
    TgList _downloadedAvatars;
    TgList _downloadedPhotos;

public:
    explicit AvatarDownloader(QObject *parent = 0);
    void readDatabase();
    void saveDatabase();

    void setClient(QObject *client);
    QObject* client() const;

signals:
    void avatarDownloaded(TgLongVariant photoId, QString filePath);
    void photoDownloaded(TgLongVariant photoId, QString filePath);

public slots:
    void authorized(TgLongVariant userId);
    void fileDownloaded(TgLongVariant fileId, QString filePath);
    void fileDownloadCanceled(TgLongVariant fileId, QString filePath);

    qint64 downloadAvatar(TgObject peer);
    qint64 downloadPhoto(TgObject photo);

    static QString getAvatarText(QString title);
    static QColor userColor(TgLongVariant id);

};

#endif // AVATARDOWNLOADER_H

