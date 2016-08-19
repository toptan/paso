#ifndef PASOSERVER_H
#define PASOSERVER_H

#include "pasodb.h"

#include <QObject>
#include <QTcpServer>
#include <memory>

namespace paso {
namespace server {
class PasoServer : public QObject {
    Q_OBJECT
public:
    explicit PasoServer(QObject *parent = 0);

    bool loadConfiguration(const QString &configFile);
    bool initDatabaseSystem();
    bool startServer();

signals:

private slots:
    void handleRequest();

private:
    QTcpServer *mTcpServer;
    std::shared_ptr<db::DBManager> mDbManager;

    unsigned short mPort;
    int mTimeout;
    const QString mDatabaseName;

    QString mDbName;
    QString mDbUsername;
    QString mDbPassword;
    QString mDbServer;
    unsigned short mDbPort;

    void handleLoginRequest(QTcpSocket *clientSocket,
                            const QString &requestString);
};
}
}

#endif // PASOSERVER_H
