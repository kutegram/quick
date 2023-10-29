#ifndef MESSAGEEDITOR_H
#define MESSAGEEDITOR_H

#include <QObject>

#include "tgclient.h"

class MessageEditor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient)
    Q_PROPERTY(QByteArray peer READ peer WRITE setPeer)

private:
    TgClient* _client;
    TgLongVariant _userId;
    TgObject _peer;
    TgObject _inputPeer;
    TgLongVariant _uploadId;
    TgObject _media;

public:
    explicit MessageEditor(QObject *parent = 0);

    void setClient(QObject *client);
    QObject* client() const;

    void setPeer(QByteArray bytes);
    QByteArray peer() const;

signals:
    void draftChanged(QString draft);
    void uploadingProgress(qint32 progress);

public slots:
    void sendMessage(QString message);
    void uploadFile();
    void cancelUpload();

    void authorized(TgLongVariant userId);
    void fileUploading(TgLongVariant fileId, TgLongVariant processedLength, TgLongVariant totalLength, qint32 progressPercentage);
    void fileUploaded(TgLongVariant fileId, TgObject inputFile);
    void fileUploadCanceled(TgLongVariant fileId);

};

#endif // MESSAGEEDITOR_H
