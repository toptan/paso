#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QObject>
#include <QSslCertificate>
#include <QSslKey>
#include <QSslSocket>
#include <QTcpServer>

#include <memory>

namespace paso {
namespace comm {
class SslServer : public QTcpServer {
public:
    explicit SslServer(std::shared_ptr<QSslCertificate> certificate,
                       std::shared_ptr<QSslKey> key, QObject *parent = nullptr);
    virtual ~SslServer();

protected:
    virtual void incomingConnection(qintptr socketDescriptor) override;

private:
    std::shared_ptr<QSslCertificate> mCertificate;
    std::shared_ptr<QSslKey> mKey;
};
}
}
#endif // SSLSERVER_H
