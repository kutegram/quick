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
    TgObject _peer;
    TgObject _inputPeer;

public:
    explicit MessageEditor(QObject *parent = 0);

    void setClient(QObject *client);
    QObject* client() const;

    void setPeer(QByteArray bytes);
    QByteArray peer() const;

signals:
    void draftChanged(QString draft);

public slots:
    void sendMessage(QString message);

};

#endif // MESSAGEEDITOR_H
