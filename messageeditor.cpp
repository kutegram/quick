#include "messageeditor.h"

#include <QFileDialog>
#include "tlschema.h"

MessageEditor::MessageEditor(QObject *parent)
    : QObject(parent)
    , _client(0)
    , _userId(0)
    , _peer()
    , _inputPeer()
    , _uploadId(0)
    , _media()
{
}

void MessageEditor::setClient(QObject *client)
{
    if (_client) {
        _client->disconnect(this);
    }

    _client = dynamic_cast<TgClient*>(client);
    _userId = 0;
    _peer = TgObject();
    _inputPeer = TgObject();
    cancelUpload();

    if (!_client) return;

    connect(_client, SIGNAL(authorized(TgLongVariant)), this, SLOT(authorized(TgLongVariant)));
    connect(_client, SIGNAL(fileUploading(TgLongVariant,TgLongVariant,TgLongVariant,qint32)), this, SLOT(fileUploading(TgLongVariant,TgLongVariant,TgLongVariant,qint32)));
    connect(_client, SIGNAL(fileUploaded(TgLongVariant,TgObject)), this, SLOT(fileUploaded(TgLongVariant,TgObject)));
    connect(_client, SIGNAL(fileUploadCanceled(TgLongVariant)), this, SLOT(fileUploadCanceled(TgLongVariant)));
}

QObject* MessageEditor::client() const
{
    return _client;
}

void MessageEditor::setPeer(QByteArray bytes)
{
    _peer = qDeserialize(bytes).toMap();
    _inputPeer = TgClient::toInputPeer(_peer);
    cancelUpload();

    emit draftChanged(_peer["draft"].toMap()["message"].toString());
}

QByteArray MessageEditor::peer() const
{
    return qSerialize(_peer);
}

void MessageEditor::sendMessage(QString message)
{
    if (!_client || !_client->isAuthorized() || TgClient::commonPeerType(_inputPeer) == 0 || _uploadId.toLongLong() || (message.isEmpty() && GETID(_media["file"].toMap()) == 0)) {
        return;
    }

    _client->messagesSendMessage(_inputPeer, message, _media);
    cancelUpload();
}

void MessageEditor::uploadFile()
{
    cancelUpload();

    QString selected = QFileDialog::getOpenFileName();

    if (selected.isEmpty()) {
        return;
    }

    if (selected.endsWith(".jpg") || selected.endsWith(".jpeg") || selected.endsWith(".png")) {
        ID_PROPERTY(_media) = TLType::InputMediaUploadedPhoto;
    } else {
        ID_PROPERTY(_media) = TLType::InputMediaUploadedDocument;

        TGOBJECT(TLType::DocumentAttributeFilename, fileName);
        fileName["file_name"] = selected.split('/').last();

        TgList attributes;
        attributes << fileName;
        _media["attributes"] = attributes;
    }

    _uploadId = _client->uploadFile(selected);

    emit uploadingProgress(0);
}

void MessageEditor::cancelUpload()
{
    _media = TgObject();
    if (_uploadId.toLongLong()) {
        _client->cancelUpload(_uploadId);
    }
    _uploadId = 0;

    emit uploadingProgress(-1);
}

void MessageEditor::authorized(TgLongVariant userId)
{
    if (_userId != userId) {
        cancelUpload();
        _userId = userId;
    }
}

void MessageEditor::fileUploadCanceled(TgLongVariant fileId)
{
    if (_uploadId != fileId) {
        return;
    }

    _uploadId = 0;
    cancelUpload();
}

void MessageEditor::fileUploaded(TgLongVariant fileId, TgObject inputFile)
{
    if (_uploadId != fileId) {
        return;
    }

    _uploadId = 0;
    _media["file"] = inputFile;

    emit uploadingProgress(100);
}

void MessageEditor::fileUploading(TgLongVariant fileId, TgLongVariant processedLength, TgLongVariant totalLength, qint32 progressPercentage)
{
    if (_uploadId != fileId) {
        return;
    }

    emit uploadingProgress(progressPercentage);
}
