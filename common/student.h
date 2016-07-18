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
    /// \brief Student constructs new student with given data.
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
            uint8_t yearOfStudy, uint64_t id = 0, const QString &rfid = "");

    ///
    /// \brief Student constructs student from the data given in the map.
    /// \param map a map that contains data for the student.
    ///
    explicit Student(const QVariantMap &map);

    ///
    /// \brief operator == The equality operator.
    /// \param other a person to compare to.
    /// \return \c true if \c other is a student and all fields are equal.
    ///
    virtual bool operator==(const Person &other) const override;

    ///
    /// \brief indexNumber Returns students index number.
    /// \return The students index number.
    ///
    QString indexNumber() const;

    ///
    /// \brief setIndexNumber Sets the students index number.
    /// \param indexNumber The students index number to set.
    ///
    void setIndexNumber(const QString &indexNumber);

    ///
    /// \brief yearOfStudy Returns students year of study.
    /// \return The students year of study.
    ///
    int yearOfStudy() const;

    ///
    /// \brief setYearOfStudy Sets the students year of study.
    /// \param yearOfStudy The students year of study to set.
    ///
    void setYearOfStudy(const int yearOfStudy);

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
    /// \brief insertQuery Returns insert query for given student with values
    /// bound.
    /// \param database The database to use.
    /// \param student The student to insert.
    /// \return The query.
    ///
    static QSqlQuery insertQuery(const QSqlDatabase &database,
                                 const Student &student);

    ///
    /// \brief updateQuery Returns update query for given student with values
    /// bound.
    /// \param database The database to use.
    /// \param student The student to update.
    /// \return The query.
    ///
    static QSqlQuery updateQuery(const QSqlDatabase &database,
                                 const Student &student);

    ///
    /// \brief findByIndexNumberQuery Returns find by index number query with
    /// bound index number value.
    /// \param database The database to use.
    /// \param indexNumber The index number.
    /// \return The query.
    ///
    static QSqlQuery findByIndexNumberQuery(const QSqlDatabase &database,
                                            const QString &indexNumber);

    ///
    /// \brief findStudentCoursesQuery Returns query that finds all courses that
    /// this student is enlisted to.
    /// \param database The databae to use.
    /// \param indexNumber The index number.
    /// \return The query.
    ///
    static QSqlQuery findStudentCoursesQuery(const QSqlDatabase &database,
                                             const QString &indexNumber);

private:
    QString mIndexNumber; //!< The students index number.
    int mYearOfStudy;     //!< The students year of study.
};
}
}
}
#endif // STUDENT_H
