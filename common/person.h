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
    /// \brief Person constructs new person with given data.
    /// \param firstName first name.
    /// \param lastName last name.
    /// \param email email.
    /// \param id the person id.
    /// \param rfid the RFID code.
    ///
    Person(const QString &firstName, const QString &lastName,
           const QString &email, quint64 id = 0, const QString &rfid = "");

    ///
    /// \brief Person constructs person from the data given in the map.
    /// \param map a map that contains data for the person.
    ///
    explicit Person(const QVariantMap &map);

    ///
    /// \brief operator == The equality operator.
    /// \param other a person to compare to.
    /// \return \c true if persons are equal.
    ///
    virtual bool operator==(const Person &other) const;

    ///
    /// \brief firstName returns persons first name.
    /// \return The first name.
    ///
    QString firstName() const;

    ///
    /// \brief setFirstName Sets the persons first name.
    /// \param firstName The first name to set.
    ///
    void setFirstName(const QString &firstName);

    ///
    /// \brief lastName returns persons last name.
    /// \return The last name.
    ///
    QString lastName() const;

    ///
    /// \brief setLastName Sets the persons last name.
    /// \param lastName The last name to set.
    ///
    void setLastName(const QString &lastName);

    ///
    /// \brief email Returns persons email.
    /// \return The persons email.
    ///
    QString email() const;

    ///
    /// \brief setEmail Sets the persons email.
    /// \param email The email to set.
    ///
    void setEmail(const QString &email);

    ///
    /// \brief rfid Returns persons access card RFID.
    /// \return The access card RFID.
    ///
    QString rfid() const;

    ///
    /// \brief setRfid Sets the persons access card RFID.
    /// \param rfid The RFID to set.
    ///
    void setRfid(const QString &rfid);

    ///
    /// \ref Entity::toVariantMap();
    ///
    virtual QVariantMap toVariantMap() const override;

    ///
    /// \ref Entity::value(const QStirng &);
    ///
    virtual QVariant value(const QString &property) const override;

    ///
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const override;

    ///
    /// \brief insertQuery Returns insert query for given person with values
    /// bound.
    /// \param database The database to use.
    /// \param person The person to insert.
    /// \return The query.
    ///
    static QSqlQuery insertQuery(const QSqlDatabase &database,
                                 const Person &person);

    ///
    /// \brief updateQuery Returns update query for given person with values
    /// bound.
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
