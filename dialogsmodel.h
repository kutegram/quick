#ifndef DIALOGSMODEL_H
#define DIALOGSMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <QMutex>
#include "tgclient.h"
#include "avatardownloader.h"

class DialogsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient)
    Q_PROPERTY(QObject* avatarDownloader READ avatarDownloader WRITE setAvatarDownloader)

private:
    QMutex _mutex;
    QList<TgObject> _dialogs;

    TgClient* _client;
    TgLongVariant _userId;

    TgLongVariant _requestId;
    TgObject _offsets;

    AvatarDownloader* _avatarDownloader;

    enum DialogRoles {
        TitleRole = Qt::UserRole + 1,
        ThumbnailColorRole,
        ThumbnailTextRole,
        AvatarRole,
        MessageTimeRole,
        MessageTextRole,
        TooltipRole,
        PeerBytesRole
    };

public:
    explicit DialogsModel(QObject *parent = 0);
    void resetState();

    QHash<int, QByteArray> roleNames() const;

    void setClient(QObject *client);
    QObject* client() const;

    void setAvatarDownloader(QObject *client);
    QObject* avatarDownloader() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    TgObject createRow(TgObject dialog, TgObject peer, TgObject message, TgObject messageSender);

signals:

public slots:
    void authorized(TgLongVariant userId);
    void messagesGetDialogsResponse(TgObject data, TgLongVariant messageId);
    void avatarDownloaded(TgLongVariant photoId, QString filePath);

    void refresh();

    bool canFetchMoreDownwards() const;
    void fetchMoreDownwards();

};

#endif // DIALOGSMODEL_H
