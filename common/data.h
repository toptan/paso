#ifndef PASO_DATA_H
#define PASO_DATA_H

#include "jsonserializable.h"

#include <QDate>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>
#include <list>

namespace paso {
namespace data {

///
/// \brief The SystemRole enum defines all possible system roles.
///
enum class SystemRole {
    ADMINISTRATOR = 0, //!< The system administrator.
    ROOM_MANAGER,      //!< The room manager.
    MANAGER,           //!< The general manager.
    SCHEDULER,         //!< The activity scheduler.
    SUPER_USER,        //!< The almighty user.
    INVALID_ROLE       //!< Invalid role.
};

///
/// \brief The ActivityType enum defines all possible activity types.
///
enum class ActivityType {
    LECTURE = 0,     //!< Regular class lecture or excercise.
    EXAM,            //!< Exams.
    COLLOQUIUM,      //!< Colloquiums.
    LAB_EXCERCISE,   //!< Lab excercises.
    INDIVIDUAL_WORK, //!< Individual work.
    SPECIAL_EVENT,   //!< Special events like conferences, public lectures, etc.
    INVALID_ACTIVITY //!< Invalid or unknown activity.
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
    BAD_YEAR_OF_STUDY,   //!< The year of study is bad.
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
const QMap<SystemRole, QString> enumeratedRoles{
    {SystemRole::ADMINISTRATOR, QObject::tr("Administrator")},
    {SystemRole::MANAGER, QObject::tr("Manager")},
    {SystemRole::ROOM_MANAGER, QObject::tr("Room manager")},
    {SystemRole::SCHEDULER, QObject::tr("Scheduler")},
    {SystemRole::SUPER_USER, QObject::tr("Super user")}};

///
/// \brief The stringEnumeratedRoles provides translatable system roles for
/// displaying text out if their string representations.
///
const QMap<QString, QString> stringEnumeratedRoles{
    {"ADMINISTRATOR", QObject::tr("Administrator")},
    {"MANAGER", QObject::tr("Manager")},
    {"ROOM_MANAGER", QObject::tr("Room manager")},
    {"SCHEDULER", QObject::tr("Scheduler")},
    {"SUPER_USER", QObject::tr("Super user")}};

///
/// \brief The enumeratedActivityTypes provides translatable activity types for
/// displaying text.
///
const QMap<ActivityType, QString> enumeratedActivityTypes{
    {ActivityType::LECTURE, QObject::tr("Lecture or excercise")},
    {ActivityType::EXAM, QObject::tr("Exam")},
    {ActivityType::COLLOQUIUM, QObject::tr("Colloquium")},
    {ActivityType::LAB_EXCERCISE, QObject::tr("Lab excercise")},
    {ActivityType::INDIVIDUAL_WORK, QObject::tr("Individual work")},
    {ActivityType::SPECIAL_EVENT, QObject::tr("Special event")}};

///
/// \brief stringEnumeratedActivityTypes provides translatable activity types
/// for displaying text out of their string representations.
///
const QMap<QString, QString> stringEnumeratedActivityTypes{
    {"LECTURE", QObject::tr("Lecture or excercise")},
    {"EXAM", QObject::tr("Exam")},
    {"COLLOQUIUM", QObject::tr("Colloquium")},
    {"LAB_EXCERCIZE", QObject::tr("Lab excercise")},
    {"INDIVIDUAL_WORK", QObject::tr("Individual work")},
    {"SPECIAL_EVENT", QObject::tr("Special event")}};

///
/// \brief Returns string representation of the system role.
/// \param role The role.
/// \return String representation of the role.
///
const QString roleToString(paso::data::SystemRole role);

///
/// \brief Returns role from its string representation.
/// \param role String representation of the role.
/// \return The role
///
paso::data::SystemRole stringToRole(const QString &role);

///
/// \brief Returns string representation of the acitivity type.
/// \param type The activity type.
/// \return String representation of the activity type.
///
const QString activityTypeToString(paso::data::ActivityType type);

///
/// \brief Returns activity type from its string representation.
/// \param type String representation of the activity type.
/// \return The activity type.
///
paso::data::ActivityType stringToActivityType(const QString &type);

///
/// \brief Returns difference in months between two dates.
/// \note If start date is after end date, \c 0 will be returned.
/// \param startDate The start date.
/// \param endDate The end date.
/// \return The difference in months.
///
int differenceInMonths(const QDate &startDate, const QDate &endDate);

///
/// \brief Returns list of timestapms that are scheduled via cron string that
/// are after start date and before end date including both.
/// \note The cron string is in reduced cron syntax. Minutes are fixed and hours
/// can be given as list but not range or wildcard. Day of month and month are
/// treated fully cron like. Day of the week can only be given as list or *.
/// Day of month and day of week are mutually exclusive fields, and if both are
/// present empty list will be returned.
/// <br/><strong>Examples:</strong>
/// <ul>
/// <li>30 11,16 * * 1,3,5 - Every Monday, Wednesday and Friday at 11:30 and
/// 16:30</li>
/// <li>0 9 * * * - Every day at 9:00</li>
/// </ul>
/// If string does not comply with above rules, empty list will be
/// returned.
///
/// \param cronString The cron string.
/// \param startDate Start date.
/// \param endDate End date.
/// \return List of timestamps that are after start date and before end date.
///
std::list<QDateTime> scheduledDates(const QString &cronString,
                                    const QDate &startDate,
                                    const QDate &endDate);
}
}

#endif // PASO_DATA_H
