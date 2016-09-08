#ifndef TEACHER_H
#define TEACHER_H

#include "person.h"

namespace paso {
namespace data {
namespace entity {

///
/// \brief The Teacher class encapsulates data about teachers like proffersors
/// or assistants.
///
class Teacher : public Person {
public:
    ///
    /// \brief Constructs new teacher with given data.
    /// \param firstName The first name.
    /// \param lastName The last name.
    /// \param email The email.
    /// \param employeeNumber The employee number.
    /// \param office The office number.
    /// \param id The id.
    /// \param rfid The RFID code.
    ///
    Teacher(const QString &firstName, const QString &lastName,
            const QString &email, const QString &employeeNumber,
            const QString &office, quint64 id = 0, const QString &rfid = "");

    ///
    /// \brief Constructs teacher from the data given in the map.
    /// \param map A map that contains data for the teacher.
    ///
    explicit Teacher(const QVariantMap &map);

    ///
    /// \brief The equality operator.
    /// \param other A person to compare to.
    /// \return \c true if \c other is a teacher and all fields are equal.
    ///
    virtual bool operator==(const Person &other) const override;

    ///
    /// \brief Returns teachers employee number.
    /// \return The teacher employee number.
    ///
    QString employeeNumber() const;

    ///
    /// \brief Sets the teachers employee number.
    /// \param employeeNumber The teachers employee number to set.
    ///
    void setEmployeeNumber(const QString &employeeNumber);

    ///
    /// \brief Returns the teachers office number.
    /// \return The teachers office number.
    ///
    QString office() const;

    ///
    /// \brief Sets teachers office number.
    /// \param office The teacher office number.
    ///
    void setOffice(const QString &office);

    ///
    /// \copydoc paso::data::entity::Entity::toVariantMap()
    ///
    virtual QVariantMap toVariantMap() const override;

    ///
    /// \copydoc paso::data::entity::Entity::value()
    ///
    virtual QVariant value(const QString &property) const override;

    ///
    /// \copydoc paso::data::JsonSerializable::read(const QJsonObject&)
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \copydoc paso::data::JsonSerializable::write()
    ///
    virtual void write(QJsonObject &jsonObject) const override;

    ///
    /// \brief Returns insert query for given teacher.
    /// \param database The database to use.
    /// \param teacher The teacher to insert.
    /// \return The query.
    ///
    static QSqlQuery insertQuery(const QSqlDatabase &database,
                                 const Teacher &teacher);

    ///
    /// \brief Returns update query for given teacher.
    /// \param database The database to use.
    /// \param teacher The teacher to update.
    /// \return The query.
    ///
    static QSqlQuery updateQuery(const QSqlDatabase &database,
                                 const Teacher &teacher);

    ///
    /// \brief Return delete query for given teacher.
    /// \param database The database to use.
    /// \param emloyeeNumber The employee number of the teacher to delete.
    /// \return The query.
    ///
    static QSqlQuery deleteQuery(const QSqlDatabase &database,
                                 const QString &emloyeeNumber);

    ///
    /// \brief Returns query that finds teacher by its employee number.
    /// \param database The datase to use.
    /// \param employeeNumber The employee number.
    /// \return The query.
    ///
    static QSqlQuery findByEmployeeNumberQuery(const QSqlDatabase &database,
                                               const QString &employeeNumber);

    ///
    /// \brief Returns query that finds all teachers RFID data.
    /// \param database The database to use.
    /// \return The query.
    ///
    static QSqlQuery emergencyDataQuery(const QSqlDatabase &database);

private:
    QString mEmployeeNumber; //!< The teacher employee number.
    QString mOffice;         //!< The teachers office number.
};
}
}
}

#endif // TEACHER_H
