#ifndef PERSON_H
#define PERSON_H

#include "entity.h"
#include "jsonserializable.h"

#include <QJsonDocument>
#include <QSqlDatabase>
#include <QVariantMap>

namespace paso {
namespace data {
namespace entity {

///
/// \brief The Person class is a class that encapsulates data about persons
/// regardless of whether the person is a professor or student.
///
class Person : public Entity, public JsonSerializable {
public:
    ///
    /// \brief Constructs new person with given data.
    /// \param firstName first name.
    /// \param lastName last name.
    /// \param email email.
    /// \param id the person id.
    /// \param rfid the RFID code.
    ///
    Person(const QString &firstName, const QString &lastName,
           const QString &email, quint64 id = 0, const QString &rfid = "");

    ///
    /// \brief Constructs person from the data given in the map.
    /// \param map a map that contains data for the person.
    ///
    explicit Person(const QVariantMap &map);

    ///
    /// \brief The equality operator.
    /// \param other a person to compare to.
    /// \return \c true if persons are equal.
    ///
    virtual bool operator==(const Person &other) const;

    ///
    /// \brief Returns persons first name.
    /// \return The first name.
    ///
    QString firstName() const;

    ///
    /// \brief Sets the persons first name.
    /// \param firstName The first name to set.
    ///
    void setFirstName(const QString &firstName);

    ///
    /// \brief Returns persons last name.
    /// \return The last name.
    ///
    QString lastName() const;

    ///
    /// \brief Sets the persons last name.
    /// \param lastName The last name to set.
    ///
    void setLastName(const QString &lastName);

    ///
    /// \brief Returns persons email.
    /// \return The persons email.
    ///
    QString email() const;

    ///
    /// \brief Sets the persons email.
    /// \param email The email to set.
    ///
    void setEmail(const QString &email);

    ///
    /// \brief Returns persons access card RFID.
    /// \return The access card RFID.
    ///
    QString rfid() const;

    ///
    /// \brief Sets the persons access card RFID.
    /// \param rfid The RFID to set.
    ///
    void setRfid(const QString &rfid);

    ///
    /// \copydoc paso::data::entity::Entity::toVariantMap() const
    ///
    virtual QVariantMap toVariantMap() const override;

    ///
    /// \copydoc paso::data::entity::Entity::value(const QString &) const
    ///
    virtual QVariant value(const QString &property) const override;

    ///
    /// \copydoc paso::data::JsonSerializable::read(const QJsonObject&)
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \copydoc paso::data::JsonSerializable::write(QJsonObject&) const;
    ///
    virtual void write(QJsonObject &jsonObject) const override;

    ///
    /// \brief Returns insert query for given person with values bound.
    /// \param database The database to use.
    /// \param person The person to insert.
    /// \return The query.
    ///
    static QSqlQuery insertQuery(const QSqlDatabase &database,
                                 const Person &person);

    ///
    /// \brief Returns update query for given person with values bound.
    /// \param database The database to use.
    /// \param person The person to update.
    /// \return The query.
    ///
    static QSqlQuery updateQuery(const QSqlDatabase &database,
                                 const Person &person);

private:
    QString mFirstName; //!< The first name.
    QString mLastName;  //!< The last name.
    QString mEmail;     //!< The email.
    QString mRFID;      //!< The persons access card RFID.
};
}
}
}

#endif // PERSON_H
