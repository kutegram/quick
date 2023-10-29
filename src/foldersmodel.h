#ifndef FOLDERSMODEL_H
#define FOLDERSMODEL_H

#include <QAbstractListModel>
#include <QVariant>
#include <QMutex>
#include "tgclient.h"

class FoldersModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* client READ client WRITE setClient)

private:
    QMutex _mutex;
    QList<TgObject> _folders;

    TgClient* _client;
    TgLongVariant _userId;

    TgLongVariant _requestId;

    enum FolderRoles {
        TitleRole = Qt::UserRole + 1,
        IconRole,
        FolderIndexRole
    };

public:
    explicit FoldersModel(QObject *parent = 0);
    void resetState();

    QHash<int, QByteArray> roleNames() const;

    void setClient(QObject *client);
    QObject* client() const;

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    TgObject createRow(TgObject filter);
    QList<TgObject> folders();

signals:
    void foldersChanged(QList<TgObject> folders);

public slots:
    void authorized(TgLongVariant userId);
    void messagesGetDialogFiltersResponse(TgVector data, TgLongVariant messageId);

    void refresh();

    bool canFetchMoreDownwards() const;
    void fetchMoreDownwards();

    static bool matchesFilter(TgObject filter, TgObject peer);

};

#endif // FOLDERSMODEL_H
