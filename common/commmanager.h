#ifndef COMMMANAGER_H
#define COMMMANAGER_H

#include <QObject>
#include <QString>

#include "commdata.h"

namespace paso {
namespace comm {

class CommManager : public QObject {
    Q_OBJECT
public:
    explicit CommManager(const QString &server, QObject *parent = 0);

    void login(const QString &username, const QString &password);

signals:
    void communicationError(const QString &reason);
    void loginSuccessfull(const LoginResponse &response);
    void loginFailed();

private:
    const QString mServer;      //<! PaSo server name
    const unsigned short mPort; //!< PaSo server port
    const int mTimeout;         //!< Network operations timeout
};
}
}

#endif // COMMMANAGER_H
