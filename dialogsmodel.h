#ifndef DIALOGSMODEL_H
#define DIALOGSMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <QMutex>
#include "tgclient.h"

class DialogsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient)

private:
    QMutex mutex;
    QList<TgObject> _dialogs;

    TgClient* _client;
    TgLongVariant _userId;

    TgLongVariant _requestId;
    TgObject _offsets;

    QHash<qint64, TgObject> _loadingAvatars;

    enum DialogRoles {
        TitleRole = Qt::UserRole + 1,
        ThumbnailColorRole,
        ThumbnailTextRole,
        AvatarRole,
        MessageTimeRole,
        MessageTextRole,
        AvatarLoadedRole,
        InputPeerRole,
        PeerBytesRole
    };

public:
    explicit DialogsModel(QObject *parent = 0);
    void resetState();

    void setClient(QObject *client);
    QObject* client() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    bool canFetchMore(const QModelIndex &parent) const;
    void fetchMore(const QModelIndex &parent);

    TgObject createRow(TgObject dialog, TgObject peer, TgObject message);

signals:

public slots:
    void authorized(TgLongVariant userId);
    void messagesGetDialogsResponse(TgObject data, TgLongVariant messageId);
    void fileDownloaded(TgLongVariant fileId, QString filePath);
    void fileDownloadCanceled(TgLongVariant fileId, QString filePath);
};

#endif // DIALOGSMODEL_H
