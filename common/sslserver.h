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

///
/// \brief The SslServer class is a SSL capable version of regular TCP server.
///
class SslServer : public QTcpServer {
public:
    ///
    /// \brief Constructs server with given SSL key and certificate.
    /// \param certificate The SSL certificate.
    /// \param key The SSL key.
    /// \param parent The parent object.
    ///
    explicit SslServer(std::shared_ptr<QSslCertificate> certificate,
                       std::shared_ptr<QSslKey> key, QObject *parent = nullptr);

    ///
    /// \brief Destructor.
    ///
    virtual ~SslServer();

protected:
    ///
    /// \brief Overriden method that tries to nagotiate and start encrypted
    /// connection.
    /// \param socketDescriptor The incomming connection socket descriptor.
    ///
    virtual void incomingConnection(qintptr socketDescriptor) override;

private:
    std::shared_ptr<QSslCertificate> mCertificate; //!< The SSL certificate.
    std::shared_ptr<QSslKey> mKey;                 //!< The SSL key.
};
}
}
#endif // SSLSERVER_H
