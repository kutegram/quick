#ifndef MESSAGESMODEL_H
#define MESSAGESMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <QMutex>
#include "tgclient.h"
#include "avatardownloader.h"

class MessagesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient)
    Q_PROPERTY(QObject* avatarDownloader READ avatarDownloader WRITE setAvatarDownloader)
    Q_PROPERTY(QByteArray peer READ peer WRITE setPeer)

private:
    QMutex _mutex;
    QList<TgObject> _history;

    TgClient* _client;
    TgLongVariant _userId;

    TgObject _peer;
    TgObject _inputPeer;

    TgLongVariant _upRequestId;
    TgLongVariant _downRequestId;

    qint32 _upOffset;
    qint32 _downOffset;

    AvatarDownloader* _avatarDownloader;

    QHash<qint64, TgVariant> _downloadRequests;

    TgLongVariant _uploadId;
    QHash<TgLong, QString> _sentMessages;
    TgObject _media;

    enum MessageRoles {
        PeerNameRole = Qt::UserRole + 1,
        MessageTextRole,
        MergeMessageRole,
        SenderNameRole,
        MessageTimeRole,
        IsChannelRole,
        ThumbnailColorRole,
        ThumbnailTextRole,
        AvatarRole,
        HasMediaRole,
        MediaImageRole,
        MediaTitleRole,
        MediaTextRole,
        MediaDownloadableRole,
        MessageIdRole,
        ForwardedFromRole,
        MediaUrlRole,
        PhotoFileRole,
        HasPhotoRole,
        PhotoSpoilerRole,
        MediaSpoilerRole
    };

public:
    explicit MessagesModel(QObject *parent = 0);
    void resetState();

    QHash<int, QByteArray> roleNames() const;

    void setClient(QObject *client);
    QObject* client() const;

    void setAvatarDownloader(QObject *client);
    QObject* avatarDownloader() const;

    void setPeer(QByteArray bytes);
    QByteArray peer() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    TgObject createRow(TgObject message, TgObject sender, TgList users, TgList chats);

    void handleHistoryResponse(TgObject data, TgLongVariant messageId);
    void handleHistoryResponseUpwards(TgObject data, TgLongVariant messageId);

signals:
    void scrollTo(qint32 index);
    void downloadUpdated(qint32 messageId, qint32 state, QString filePath);
    void draftChanged(QString draft);
    void uploadingProgress(qint32 progress);
    void scrollForNew();

public slots:
    void authorized(TgLongVariant userId);
    void messagesGetHistoryResponse(TgObject data, TgLongVariant messageId);
    void avatarDownloaded(TgLongVariant photoId, QString filePath);
    void photoDownloaded(TgLongVariant photoId, QString filePath);

    void fileDownloaded(TgLongVariant fileId, QString filePath);
    void fileDownloadCanceled(TgLongVariant fileId, QString filePath);

    void fileUploading(TgLongVariant fileId, TgLongVariant processedLength, TgLongVariant totalLength, qint32 progressPercentage);
    void fileUploaded(TgLongVariant fileId, TgObject inputFile);
    void fileUploadCanceled(TgLongVariant fileId);

    void sendMessage(QString message);
    void uploadFile();
    void cancelUpload();

    void gotMessageUpdate(TgObject update, TgLongVariant messageId);
    void gotUpdate(TgObject update, TgLongVariant messageId, TgList users, TgList chats, qint32 date, qint32 seq, qint32 seqStart);

    bool canFetchMoreDownwards() const;
    void fetchMoreDownwards();

    bool canFetchMoreUpwards() const;
    void fetchMoreUpwards();

    void linkActivated(QString link, qint32 index);
    void downloadFile(qint32 index);
    void cancelDownload(qint32 index);

    void openUrl(QString url);
};

#endif // MESSAGESMODEL_H
