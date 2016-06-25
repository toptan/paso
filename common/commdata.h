#ifndef COMMDATA_H
#define COMMDATA_H

#include <cstdint>

#include "data.h"
#include "systemuser.h"

#include <QUuid>

namespace paso {
namespace comm {

///
/// \brief The Operation enum defines all possible operations for server
/// communication requests and responses.
///
enum class Operation {
    LOGIN,            //<! Login operation.
    UNKNOWN_OPERATION //<! Unknown operation, must be last always
};

///
/// \brief operationToString Returns string representation of the server
/// operation.
/// \param operation the operation.
/// \return string representation of operation.
///
const QString operationToString(Operation operation);

///
/// \brief operationFromString Returns operation from its string representation.
/// \param operation string representation of operation.
/// \return operation.
///
Operation operationFromString(const QString &operation);

///
/// \brief The Base class is a base class for all requests and responses.
///
class Base : public data::JsonSerializable {
public:
    ///
    /// \brief Base constructs base object from give json string.
    /// \param jsonString the json string.
    ///
    explicit Base(const QString &jsonString);

    ///
    /// \brief Base constructs base object and sets room uuid and operation.
    /// \param roomId the room uuid.
    /// \param operation the operation.
    ///
    Base(const QUuid &roomId, const Operation &operation);

    ///
    /// \brief roomId returns room uuid.
    /// \return the room uuid.
    ///
    QUuid roomId() const;

    ///
    /// \brief operation returns requested operation.
    /// \return the requested operation.
    ///
    Operation operation() const;

    ///
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject);

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const;

private:
    QUuid mRoomId;        //<! The room uuid.
    Operation mOperation; //<! The requested operation.
};

///
/// \brief The LoginRequest class encapsulates login requests. Login request
/// always carries null room uuid.
///
class LoginRequest : public Base {
public:
    ///
    /// \brief LoginRequest constructs empty login request.
    ///
    LoginRequest();

    ///
    /// \brief LoginRequest constructs new login request.
    /// \param username username
    /// \param password password.
    ///
    LoginRequest(const QString &username, const QString &password);

    ///
    /// \brief username returns username.
    /// \return username.
    ///
    QString username() const;

    ///
    /// \brief password returns password.
    /// \return password.
    ///
    QString password() const;

    ///
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject);

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const;

private:
    QString mUsername; //<! Username
    QString mPassword; //<! Password
};

///
/// \brief The LoginResponse class encapsulate login responses. Login response
/// always carries null room uuid.
///
class LoginResponse : public Base {
public:
    ///
    /// \brief LoginResponse constructs empty login response.
    ///
    LoginResponse();

    ///
    /// \brief LoginResponse constructs new login response.
    /// \param systemUser data about system user that logged in.
    /// \param dbName database name.
    /// \param dbServer database server.
    /// \param dbUsername database user name.
    /// \param dbPassword database password.
    /// \param dbPort database port.
    ///
    LoginResponse(const data::SystemUser &systemUser, const QString &dbName,
                  const QString &dbServer, const QString &dbUsername,
                  const QString &dbPassword, uint16_t dbPort);

    ///
    /// \brief systemUser returns system user.
    /// \return system user.
    ///
    const data::SystemUser &systemUser() const;

    ///
    /// \brief dbName returns database name to use.
    /// \return database name.
    ///
    QString dbName() const;

    ///
    /// \brief dbServer returns database server to use.
    /// \return database server.
    ///
    QString dbServer() const;

    ///
    /// \brief dbUsername returns database username to use.
    /// \return database username.
    ///
    QString dbUsername() const;

    ///
    /// \brief dbPassword returns database password to use.
    /// \return database password.
    ///
    QString dbPassword() const;

    ///
    /// \brief dbPort returns database server port to use.
    /// \return database server port.
    ///
    uint16_t dbPort() const;

    ///
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject);

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const;

private:
    data::SystemUser mSystemUser; //!< The system user.
    QString mDbName;              //!< The database name.
    QString mDbServer;            //!< The database server.
    QString mDbUsername;          //!< The database username.
    QString mDbPassword;          //!< The database password.
    uint16_t mDbPort;             //!< The database port.
};
}
}

#endif // COMMDATA_H
