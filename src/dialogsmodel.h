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
    Q_PROPERTY(QObject* folders READ folders WRITE setFolders)

private:
    QMutex _mutex;
    QList<TgObject> _dialogs;

    TgClient* _client;
    TgLongVariant _userId;

    TgLongVariant _requestId;
    TgObject _offsets;

    AvatarDownloader* _avatarDownloader;

    FoldersModel* _folders;
    qint32 _lastPinnedIndex;

    enum DialogRoles {
        TitleRole = Qt::UserRole + 1,
        ThumbnailColorRole,
        ThumbnailTextRole,
        AvatarRole,
        MessageTimeRole,
        MessageTextRole,
        TooltipRole,
        PeerBytesRole,
        MessageSenderNameRole,
        MessageSenderColorRole
    };

public:
    explicit DialogsModel(QObject *parent = 0);
    void resetState();

    QHash<int, QByteArray> roleNames() const;

    void setClient(QObject *client);
    QObject* client() const;

    void setAvatarDownloader(QObject *client);
    QObject* avatarDownloader() const;

    void setFolders(QObject *model);
    QObject* folders() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    TgObject createRow(TgObject dialog, TgObject peer, TgObject message, TgObject messageSender, QList<TgObject> folders, TgList users, TgList chats);
    void handleDialogMessage(TgObject &row, TgObject message, TgObject messageSender, TgList users, TgList chats);
    void prepareNotification(TgObject row);

signals:
    void sendNotification(qint64 peerId, QString peerName, QString senderName, QString text, bool silent);

public slots:
    void authorized(TgLongVariant userId);
    void messagesGetDialogsResponse(TgObject data, TgLongVariant messageId);
    void avatarDownloaded(TgLongVariant photoId, QString filePath);

    void refresh();

    bool canFetchMoreDownwards() const;
    void fetchMoreDownwards();

    void foldersChanged(QList<TgObject> folders);
    bool inFolder(qint32 index, qint32 folderIndex);

    void gotUpdate(TgObject update, TgLongVariant messageId, TgList users, TgList chats, qint32 date, qint32 seq, qint32 seqStart);
    void gotMessageUpdate(TgObject update, TgLongVariant messageId);

};

#endif // DIALOGSMODEL_H
