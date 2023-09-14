#ifndef DIALOGSMODEL_H
#define DIALOGSMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <QMutex>
#include "tgclient.h"
#include "avatardownloader.h"
#include "foldersmodel.h"

class DialogsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient)
    Q_PROPERTY(QObject* avatarDownloader READ avatarDownloader WRITE setAvatarDownloader)
    Q_PROPERTY(qint32 elideLength READ elideLength WRITE setElideLength)
    Q_PROPERTY(QObject* folders READ folders WRITE setFolders)

private:
    QMutex _mutex;
    QList<TgObject> _dialogs;

    TgClient* _client;
    TgLongVariant _userId;

    TgLongVariant _requestId;
    TgObject _offsets;

    AvatarDownloader* _avatarDownloader;

    qint32 _elideLength;

    FoldersModel* _folders;

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

    void setElideLength(qint32 length);
    qint32 elideLength() const;

    void setFolders(QObject *model);
    QObject* folders() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    TgObject createRow(TgObject dialog, TgObject peer, TgObject message, TgObject messageSender, QList<TgObject> folders);

signals:

public slots:
    void authorized(TgLongVariant userId);
    void messagesGetDialogsResponse(TgObject data, TgLongVariant messageId);
    void avatarDownloaded(TgLongVariant photoId, QString filePath);

    void refresh();

    bool canFetchMoreDownwards() const;
    void fetchMoreDownwards();

    void foldersChanged(QList<TgObject> folders);
    bool inFolder(qint32 index, qint32 folderIndex);

};

#endif // DIALOGSMODEL_H
