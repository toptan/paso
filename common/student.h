#ifndef STUDENT_H
#define STUDENT_H

#include "person.h"

namespace paso {
namespace data {
namespace entity {

///
/// \brief The Student class is a class that encapsuletes data about students.
///
class Student : public Person {
public:
    ///
    /// \brief Constructs new student with given data.
    /// \param firstName the first name.
    /// \param lastName the last name.
    /// \param email the email.
    /// \param indexNumber the index number.
    /// \param yearOfStudy the year of study.
    /// \param id the student id.
    /// \param rfid the RFID code.
    ///
    Student(const QString &firstName, const QString &lastName,
            const QString &email, const QString &indexNumber,
            uint8_t yearOfStudy, quint64 id = 0, const QString &rfid = "");

    ///
    /// \brief Constructs student from the data given in the map.
    /// \param map a map that contains data for the student.
    ///
    explicit Student(const QVariantMap &map);

    ///
    /// \brief The equality operator.
    /// \param other a person to compare to.
    /// \return \c true if \c other is a student and all fields are equal.
    ///
    virtual bool operator==(const Person &other) const override;

    ///
    /// \brief Returns students index number.
    /// \return The students index number.
    ///
    QString indexNumber() const;

    ///
    /// \brief Sets the students index number.
    /// \param indexNumber The students index number to set.
    ///
    void setIndexNumber(const QString &indexNumber);

    ///
    /// \brief Returns students year of study.
    /// \return The students year of study.
    ///
    int yearOfStudy() const;

    ///
    /// \brief Sets the students year of study.
    /// \param yearOfStudy The students year of study to set.
    ///
    void setYearOfStudy(const int yearOfStudy);

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
    /// \brief Returns insert query for given student with values bound.
    /// \param database The database to use.
    /// \param student The student to insert.
    /// \return The query.
    ///
    static QSqlQuery insertQuery(const QSqlDatabase &database,
                                 const Student &student);

    ///
    /// \brief Returns update query for given student with values bound.
    /// \param database The database to use.
    /// \param student The student to update.
    /// \return The query.
    ///
    static QSqlQuery updateQuery(const QSqlDatabase &database,
                                 const Student &student);

    ///
    /// \brief Returns delete query for given student.
    /// \param database The database to use.
    /// \param indexNumber The student to delete.
    /// \return The query.
    ///
    static QSqlQuery deleteQuery(const QSqlDatabase &database,
                                 const QString &indexNumber);

    ///
    /// \brief Returns find by index number query with bound index number value.
    /// \param database The database to use.
    /// \param indexNumber The index number.
    /// \return The query.
    ///
    static QSqlQuery findByIndexNumberQuery(const QSqlDatabase &database,
                                            const QString &indexNumber);

    ///
    /// \brief Returns query that finds all courses that this student is
    /// enlisted to.
    /// \param database The database to use.
    /// \param indexNumber The index number.
    /// \return The query.
    ///
    static QSqlQuery findStudentCoursesQuery(const QSqlDatabase &database,
                                             const QString &indexNumber);

    ///
    /// \brief Returns query that finds all lists that this student is member
    /// of.
    /// \param database The database to use.
    /// \param indexNumber The index number.
    /// \return The query.
    ///
    static QSqlQuery findStudentListsQuery(const QSqlDatabase &database,
                                           const QString &indexNumber);

private:
    QString mIndexNumber; //!< The students index number.
    int mYearOfStudy;     //!< The students year of study.
};
}
}
}
#endif // STUDENT_H
