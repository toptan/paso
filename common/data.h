#ifndef PASO_DATA_H
#define PASO_DATA_H

#include "jsonserializable.h"

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
/// \brief The CourseImportError enum defines error codes for importing courses.
///
enum class CourseImportError {
    NO_ERROR,      //!< All fine, no error.
    INVALID_LINE,  //!< The CSV line is illformed.
    NO_CODE,       //!< Missing course code.
    CODE_TOO_LONG, //!< Course code too long.
    NO_NAME,       //!< Missing course name.
    NAME_TOO_LONG, //!< Course name too long.
    DB_ERROR       //!< Generic database error.
};

///
/// \brief The StudentImportError enum defines error codes for importing
/// students.
///
enum class StudentImportError {
    NO_ERROR,            //!< All fine, no error.
    INVALID_LINE,        //!< The CSV line is illformed.
    NO_INDEX_NUMBER,     //!< Missing index number.
    BAD_INDEX_NUMBER,    //!< The index number is wrong.
    NO_FIRST_NAME,       //!< Missing first name.
    FIRST_NAME_TOO_LONG, //!< First name is too long.
    NO_LAST_NAME,        //!< Missing last name.
    LAST_NAME_TOO_LONG,  //!< Last name is too long.
    BAD_EMAIL,           //!< The email is bad.
    NO_YEAR_OF_STUDY,    //!< Year of study missing.
    BAD_YEAR_OF_STUDY,   //<! The year of study is bad.
    DB_ERROR             //!< Generic database error.
};

///
/// \brief The ListStudentImportError enum defines error codes for importing
/// list or course students.
///
enum class ListStudentImportError {
    NO_ERROR,             //!< All fine, no error.
    BAD_INDEX_NUMBER,     //!< The index number is in wrong format or missing.
    NON_EXISTING_STUDENT, //!< The student with given index number does not
                          //! exist in the database.
    DB_ERROR              //!< Generic database error.
};

///
/// \brief The enumeratedRoles provides translatable system roles for displaying
/// text.
///
const QMap<SystemRole, QString> enumeratedRoles = {
    {SystemRole::ADMINISTRATOR, QObject::tr("Administrator")},
    {SystemRole::MANAGER, QObject::tr("Manager")},
    {SystemRole::ROOM_MANAGER, QObject::tr("Room manager")},
    {SystemRole::SCHEDULER, QObject::tr("Scheduler")},
    {SystemRole::SUPER_USER, QObject::tr("Super user")}};

///
/// \brief The stringEnumeratedRoles provides translatable system roles from
/// displaying text out if their string representations.
///
const QMap<QString, QString> stringEnumeratedRoles = {
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

}
}

#endif // PASO_DATA_H
