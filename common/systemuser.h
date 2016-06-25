#ifndef SYSTEMUSER_H
#define SYSTEMUSER_H

#include "data.h"

#include <QJsonDocument>

namespace paso {
namespace data {

///
/// \brief The SystemUser class encapsulates data about system user.
///
class SystemUser : public JsonSerializable {
public:
    ///
    /// \brief SystemUser constructs new system user with given data.
    /// \param username an username.
    /// \param password a password.
    /// \param firstName a first name.
    /// \param lastName a last name.
    /// \param email an email.
    /// \param role a system role.
    ///
    explicit SystemUser(const QString &username, const QString &password = "",
                        const QString &firstName = "",
                        const QString &lastName = "", const QString &email = "",
                        SystemRole role = SystemRole::INVALID_ROLE);

    ///
    /// \brief SystemUser constructs system user instance from the map.
    /// \param map a map that contains system user data.
    ///
    ///
    explicit SystemUser(const QVariantMap &map);

    ///
    /// \brief SystemUser The copy constructor.
    /// \param other an instance to copy from.
    ///
    explicit SystemUser(const SystemUser &other);

    ///
    /// \brief operator == Equality operator. Two system users are equal all of
    /// their properties are equal.
    /// \param other object to compare against.
    /// \return \code true if two system users are equal/the same.
    ///
    bool operator==(const SystemUser &other) const;

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
    /// \brief setPassword sets new password.
    /// \param password a new password to set.
    ///
    void setPassword(const QString &password);

    ///
    /// \brief firstName returns first name.
    /// \return first name.
    ///
    QString firstName() const;

    ///
    /// \brief setFirstName sets new first name.
    /// \param firstName a new first name.
    ///
    void setFirstName(const QString &firstName);

    ///
    /// \brief lastName returns last name.
    /// \return last name.
    ///
    QString lastName() const;

    ///
    /// \brief setLastName sets new last name.
    /// \param lastName a new last name to set.
    ///
    void setLastName(const QString &lastName);

    ///
    /// \brief email returns email.
    /// \return email.
    ///
    QString email() const;

    ///
    /// \brief setEmail sets new email.
    /// \param email a new email to set.
    ///
    void setEmail(const QString &email);

    ///
    /// \brief role returns system role.
    /// \return system role.
    ///
    SystemRole role() const;

    ///
    /// \brief setRole sets new system role.
    /// \param role a new system role.
    ///
    void setRole(const SystemRole &role);

    ///
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject);

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const;

private:
    QString mUsername;  //!< Username
    QString mPassword;  //!< Password
    QString mFirstName; //!< First name
    QString mLastName;  //!< Last name
    QString mEmail;     //!< Email
    SystemRole mRole;   //!< System role
};

}
}
#endif // SYSTEMUSER_H
