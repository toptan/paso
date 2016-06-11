#ifndef PASO_DATA_H
#define PASO_DATA_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>

namespace paso {
namespace data {

///
/// \brief The SystemRole enum defines all possible system roles.
///
enum class SystemRole {
    ADMINISTRATOR = 0,
    ROOM_MANAGER,
    MANAGER,
    SCHEDULER,
    SUPER_USER,
    INVALID_ROLE
};

///
/// \brief The columnLabels provides translatable SQL table column names.
///
static const QVariantMap columnLabels = {
    // System user record labels.
    {"username", QObject::tr("Username")},
    {"password", QObject::tr("Password")},
    {"first_name", QObject::tr("First Name")},
    {"last_name", QObject::tr("Last Name")},
    {"email", QObject::tr("Email")},
    {"role", QObject::tr("Role")},
    // Room record labels
    {"room_uuid", QObject::tr("Room UUID")},
    {"name", QObject::tr("Name")},
    {"room_number", QObject::tr("Number")}
};

///
/// \brief The enumeratedRoles provides translatable system roles for displaying
/// text.
///
static const QMap<SystemRole, QString> enumeratedRoles = {
    {SystemRole::ADMINISTRATOR, QObject::tr("Administrator")},
    {SystemRole::MANAGER, QObject::tr("Manager")},
    {SystemRole::ROOM_MANAGER, QObject::tr("Room manager")},
    {SystemRole::SCHEDULER, QObject::tr("Scheduler")},
    {SystemRole::SUPER_USER, QObject::tr("Super user")}};

///
/// \brief The stringEnumeratedRoles provides translatable system roles from
/// displaying text out if their string representations.
///
static const QMap<QString, QString> stringEnumeratedRoles = {
    {"ADMINISTRATOR", QObject::tr("Administrator")},
    {"MANAGER", QObject::tr("Manager")},
    {"ROOM_MANAGER", QObject::tr("Room manager")},
    {"SCHEDULER", QObject::tr("Scheduler")},
    {"SUPER_USER", QObject::tr("Super user")}};

///
/// \brief roleToString Returns string representation of the system role.
/// \param role The role.
/// \return String representation of the role.
///
const QString roleToString(paso::data::SystemRole role);

///
/// \brief stringToRole Returns role from its string representation.
/// \param role String representation of the role.
/// \return The role
///
paso::data::SystemRole stringToRole(const QString &role);

///
/// \brief The JsonSerializable class is a common interface for all objects that
/// need to be serialized to JSON or deserialized from JSON.
///
class JsonSerializable {
public:
    ///
    /// \brief ~JsonSerializable The destructor.
    ///
    virtual ~JsonSerializable();

    ///
    /// \brief read This method reads given JSON string and populates this
    /// object's properties
    /// \param jsonString A JSON string to read from.
    ///
    virtual void fromJsonString(const QString &jsonString) final;

    ///
    /// \brief read This method reads data from given JSON object and populates
    /// this object's properties.
    /// \param jsonObject A JSON object to read data from.
    ///
    virtual void read(const QJsonObject &jsonObject) = 0;

    ///
    /// \brief write This method returns JSON string representation of this
    /// object.
    /// \return A JSON string representation of this object.
    ///
    virtual QString toJsonString() const final;

    ///
    /// \brief write This method writes data from this object to the given JSON
    /// object.
    /// \param jsonObject A JSON object to write data to.
    ///
    virtual void write(QJsonObject &jsonObject) const = 0;
};

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

///
/// \brief The Room class encapsulates data about rooms/laboratories.
///
class Room : public JsonSerializable {
public:
    ///
    /// \brief Room constructs new room with given data.
    /// \param roomUUID room UUID.
    /// \param name room name.
    /// \param number room number.
    ///
    Room(const QString &roomUUID, const QString &name, const QString &number);

    ///
    /// \brief Room constructs room from the data given in the map.
    /// \param map a map that contains data for the room.
    ///
    explicit Room(const QVariantMap &map);

    ///
    /// \brief Room The copy constructor.
    /// \param other an object to copy from.
    ///
    explicit Room(const Room &other);

    ///
    /// \brief operator == The equality operator. Two rooms are equal if all
    /// their properties are equal.
    /// \param other a room to compare to.
    /// \return \code true if rooms are equal.
    ///
    bool operator==(const Room &other) const;

    ///
    /// \brief roomUUID returns room UUID.
    /// \return room UUID.
    ///
    QString roomUUID() const;

    ///
    /// \brief name returns room name.
    /// \return room name.
    ///
    QString name() const;

    ///
    /// \brief setName sets new room name.
    /// \param name a new room name.
    ///
    void setName(const QString &name);

    ///
    /// \brief number returns room number.
    /// \return room number.
    ///
    QString number() const;

    ///
    /// \brief setNumber sets new room number.
    /// \param number a new room number.
    ///
    void setNumber(const QString &number);

    ///
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject);

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const;

private:
    QString mRoomUUID; //!< The room UUID
    QString mName;     //!< The room name
    QString mNumber;   //!< The room number, i.e. 56, P25
};
}
}

#endif // PASO_DATA_H
