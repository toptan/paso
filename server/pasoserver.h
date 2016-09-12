#ifndef PASOSERVER_H
#define PASOSERVER_H

#include "commdata.h"
#include "pasodb.h"
#include "sslserver.h"

#include <QObject>
#include <QTcpServer>
#include <memory>

namespace paso {
namespace server {

///
/// \brief The ControllerInfo struct encapsulates the data about controllers
/// required by the server.
///
struct ControllerInfo {
    QHostAddress controllerAddress; //!< The controller's IP address.
    quint16 controllerPort;         //!< The port where contoller listens.
    QUuid controllerUuid;           //!< The controller UUID.
};

///
/// \brief The PasoServer class handles all communication with controllers and
/// provides login interface for administrative application.
///
class PasoServer : public QObject {
    Q_OBJECT
public:
    ///
    /// \brief The constructor.
    /// \param parent The parent object.
    ///
    explicit PasoServer(QObject *parent = 0);

    ///
    /// \brief Loads server configuration from given file.
    /// \param configFile The configuration file.
    /// \return \c true if configuration is sane.
    ///
    bool loadConfiguration(const QString &configFile);

    ///
    /// \brief Initializes database subsystem and connects to the database.
    /// \return \c true on success.
    ///
    bool initDatabaseSystem();

    ///
    /// \brief Starts the server and accepts connections.
    /// \return \c true if server successfully started.
    ///
    bool startServer();

private slots:
    ///
    /// \brief Handles incomming request.
    ///
    void handleRequest();

private:
    comm::SslServer *mTcpServer;               //!< The SSL server.
    std::shared_ptr<db::DBManager> mDbManager; //!< The database manager to use.

    unsigned short mPort;       //!< The port to listen to.
    int mTimeout;               //!< Network operations timeout in milliseconds.
    int mControllerCheckPeriod; //!< Controller check period in milliseconds.

    const QString mDatabaseName; //!< Internal database connection name.

    QString mDbName;        //!< Database name.
    QString mDbUsername;    //!< Database user.
    QString mDbPassword;    //!< Database password.
    QString mDbServer;      //!< Database server.
    unsigned short mDbPort; //!< Database port.
    QString mKeyFile;       //!< Path to the server key file.
    QString mCertFile;      //!< Path to the server certificate.

    QMap<QUuid, ControllerInfo> mControllers; //!< Registered controllers.

    ///
    /// \brief Writes response to the socket and closes connection.
    /// \param clientSocket
    /// \param responseData
    ///
    void writeResponse(QTcpSocket *clientSocket,
                       const paso::comm::Base &responseData) const;

    ///
    /// \brief Handles administrative application login requests.
    /// \param clientSocket The socket.
    /// \param requestString The request data string.
    ///
    void handleLoginRequest(QTcpSocket *clientSocket,
                            const QString &requestString);

    ///
    /// \brief Handles controllers and simulators registration requests.
    /// \param clientSocket The socket.
    /// \param requestString The request data string.
    ///
    void handleRegisterRequest(QTcpSocket *clientSocket,
                               const QString &requestString);

    ///
    /// \brief Handles access check queries.
    /// \param clientSocket The socket.
    /// \param requestString The request data string.
    ///
    void handleAccessRequest(QTcpSocket *clientSocket,
                             const QString &requestString);

    ///
    /// \brief Called periodically to check controllers health.
    ///
    void checkControllers();

    ///
    /// \brief Performs controller check, sends emergency data and waits for the
    /// response.
    /// \param uuid The controller UUID.
    /// \param emergencyData The emergency data.
    /// \return \c true if controller reported back with no problems.
    ///
    bool checkController(const QUuid &uuid, const QStringList &emergencyData);

    ///
    /// \brief Called periodically to purge expired lists.
    ///
    void purgeLists();
};
}
}

#endif // PASOSERVER_H
