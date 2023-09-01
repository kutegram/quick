#include "messageeditor.h"

MessageEditor::MessageEditor(QObject *parent)
    : QObject(parent)
    , _client(0)
    , _peer()
    , _inputPeer()
{
}

void MessageEditor::setClient(QObject *client)
{
    _client = dynamic_cast<TgClient*>(client);
    _peer = TgObject();
    _inputPeer = TgObject();
}

QObject* MessageEditor::client() const
{
    return _client;
}

void MessageEditor::setPeer(QByteArray bytes)
{
    TgObject peer;
    QDataStream peerStream(&bytes, QIODevice::ReadOnly);
    peerStream >> peer;

    _peer = peer;
    _inputPeer = TgClient::toInputPeer(peer);

    emit draftChanged(peer["draft"].toMap()["message"].toString());
}

QByteArray MessageEditor::peer() const
{
    QByteArray array;
    QDataStream peerStream(&array, QIODevice::WriteOnly);
    peerStream << _peer;

    return array;
}

void MessageEditor::sendMessage(QString message)
{
    if (!_client || !_client->isAuthorized() || TgClient::commonPeerType(_inputPeer) == 0 || message.isEmpty()) {
        return;
    }

    _client->messagesSendMessage(_inputPeer, message);
}
