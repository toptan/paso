#ifndef COMMDATA_H
#define COMMDATA_H

#include <cstdint>

#include "data.h"

namespace paso {
namespace comm {

///
/// \brief The LoginResponse class encapsulate login responses.
///
class LoginResponse : public data::JsonSerializable {
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
