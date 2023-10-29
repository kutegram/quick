#ifndef CURRENTUSERINFO_H
#define CURRENTUSERINFO_H

#include <QObject>
#include <QMutex>
#include "tgclient.h"
#include "avatardownloader.h"

class CurrentUserInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient)
    Q_PROPERTY(QObject* avatarDownloader READ avatarDownloader WRITE setAvatarDownloader)

private:
    QMutex _mutex;
    TgClient* _client;

    TgLongVariant _userId;
    TgLongVariant _requestId;

    AvatarDownloader* _avatarDownloader;

public:
    explicit CurrentUserInfo(QObject *parent = 0);

    void setClient(QObject *client);
    QObject* client() const;

    void setAvatarDownloader(QObject *client);
    QObject* avatarDownloader() const;

signals:
    void userInfoChanged(QString name, QString username, QColor thumbnailColor, QString thumbnailText);
    void userAvatarDownloaded(QString avatar);

public slots:
    void authorized(TgLongVariant userId);
    void usersGetUsersResponse(TgVector data, TgLongVariant messageId);
    void avatarDownloaded(TgLongVariant photoId, QString filePath);

};

#endif // CURRENTUSERINFO_H
