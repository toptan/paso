#ifndef COURSE_H
#define COURSE_H

#include "jsonserializable.h"

#include <QJsonDocument>
#include <QString>

namespace paso {
namespace data {

///
/// \brief The Course class encapsulates data about courses.
///
class Course : public JsonSerializable {
public:
    ///
    /// \brief Course constructs new course with given data.
    /// \param code The course code.
    /// \param name The course name.
    ///
    Course(const QString &code, const QString &name);

    ///
    /// \brief Course constructs course from the data given in the map.
    /// \param map A map that contains data for the course.
    ///
    explicit Course(const QVariantMap &map);

    ///
    /// \brief Course The copy constructor.
    /// \param other An object to copy from.
    ///
//    explicit Course(const Course &other);

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
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const override;

private:
    QString mCode; //!< The course code.
    QString mName; //!< The course name.
};
}
}
#endif // COURSE_H
