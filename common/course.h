#ifndef COURSE_H
#define COURSE_H

#include "entity.h"
#include "jsonserializable.h"

#include <QJsonDocument>
#include <QString>

namespace paso {
namespace data {
namespace entity {

///
/// \brief The Course class encapsulates data about courses.
///
class Course : public Entity, public JsonSerializable {
public:
    ///
    /// \brief Course constructs new course with given data.
    /// \param code The course code.
    /// \param name The course name.
    /// \param id The course id.
    ///
    Course(const QString &code = "", const QString &name = "", uint64_t id = 0);

    ///
    /// \brief Course constructs course from the data given in the map.
    /// \param map A map that contains data for the course.
    ///
    explicit Course(const QVariantMap &map);

    ///
    /// \brief operator == The equality operator. Two courses are equal if all
    /// their properties are equal.
    /// \param other The course to compare to.
    /// \return \code true if courses are equal.
    ///
    bool operator==(const Course &other) const;

    ///
    /// \brief code Returns course code.
    /// \return The course code.
    ///
    QString code() const;

    ///
    /// \brief setCode Sets new course code.
    /// \param code The course code to set.
    ///
    void setCode(const QString &code);

    ///
    /// \brief name Returns course name.
    /// \return The course name.
    ///
    QString name() const;

    ///
    /// \brief setName Sets new course name.
    /// \param name The new course name.
    ///
    void setName(const QString &name);

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
    /// \brief insertQuery Returns insert query for given course with values
    /// bound.
    /// \param database The database to use.
    /// \param course The course to insert.
    /// \return The query.
    ///
    static QSqlQuery insertQuery(const QSqlDatabase &database,
                                 const Course &course);

    ///
    /// \brief updateQuery Returns update query for given course with values
    /// bound.
    /// \param database The database to use.
    /// \param course The course to update.
    /// \return The query.
    ///
    static QSqlQuery updateQuery(const QSqlDatabase &database,
                                 const Course &course);

    ///
    /// \brief deleteByCodeQuery Returns delete by course code query with bound
    /// code value.
    /// \param database The database to use.
    /// \param code The course code.
    /// \return The query.
    ///
    static QSqlQuery deleteByCodeQuery(const QSqlDatabase &database,
                                       const QString &code);

    ///
    /// \brief findByCodeQuery Returns find by course code query with bound code
    /// value.
    /// \param database The database to use.
    /// \param code The course code.
    /// \return The query.
    ///
    static QSqlQuery findByCodeQuery(const QSqlDatabase &database,
                                     const QString &code);

    ///
    /// \brief findCourseStudents Returns query that finds all students enlisted
    /// to a given course.
    /// \param database The database to use.
    /// \param code The course code.
    /// \return The query.
    ///
    static QSqlQuery findCourseStudentsQuery(const QSqlDatabase &database,
                                             const QString &code);

    ///
    /// \brief enlistStudentToCourseQuery Returns query that enlists student to
    /// the course. If student is already enlisted the query will do nothing.
    /// \param database The database to use.
    /// \param code The course code.
    /// \param indexNumber The student's index number.
    /// \return The query.
    ///
    static QSqlQuery enlistStudentToCourseQuery(const QSqlDatabase &database,
                                                const QString &code,
                                                const QString &indexNumber);

    ///
    /// \brief removeStudentFromCourseQuery Returns query that removes a student
    /// from the course. If student is not enlisted the query will do nothing.
    /// \param database The database to use.
    /// \param code The course code.
    /// \param indexNumber The student's index number.
    /// \return The query.
    ///
    static QSqlQuery removeStudentFromCourseQuery(const QSqlDatabase &database,
                                                  const QString &code,
                                                  const QString &indexNumber);

private:
    QString mCode; //!< The course code.
    QString mName; //!< The course name.
};
}
}
}
#endif // COURSE_H
