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
