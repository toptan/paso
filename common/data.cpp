#include "data.h"

#include <QJsonDocument>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

using namespace std;

namespace paso {
namespace data {

const QString roleToString(data::SystemRole role) {
    switch (role) {
    case SystemRole::ADMINISTRATOR:
        return "ADMINISTRATOR";
    case SystemRole::MANAGER:
        return "MANAGER";
    case SystemRole::ROOM_MANAGER:
        return "ROOM_MANAGER";
    case SystemRole::SCHEDULER:
        return "SCHEDULER";
    case SystemRole::SUPER_USER:
        return "SUPER_USER";
    default:
        return "INVALID_ROLE";
    }
}

SystemRole stringToRole(const QString &role) {
    if (role == "ADMINISTRATOR") {
        return SystemRole::ADMINISTRATOR;
    } else if (role == "ROOM_MANAGER") {
        return SystemRole::ROOM_MANAGER;
    } else if (role == "MANAGER") {
        return SystemRole::MANAGER;
    } else if (role == "SCHEDULER") {
        return SystemRole::SCHEDULER;
    } else if (role == "SUPER_USER") {
        return SystemRole::SUPER_USER;
    } else {
        return SystemRole::INVALID_ROLE;
    }
}

list<QDateTime> scheduledDates(const QString &cronString,
                               const QDate &startDate, const QDate &endDate) {
    list<QDateTime> retVal;
    auto temp = cronString.trimmed();
    auto segments = cronString.split(" ");
    if (segments.size() != 5) {
        return retVal;
    }
    auto minutes = segments[0].trimmed();
    auto hours = segments[1].trimmed();
    auto daysOfMont = segments[2].trimmed();
    auto months = segments[3].trimmed();
    auto days = segments[4].trimmed();

    QRegularExpression minutesRegEx("^[0-9]{1,2}?");
    auto match = minutesRegEx.match(minutes);
    if (!match.hasMatch()) {
        return retVal;
    }


    retVal.push_back(QDateTime());
    return retVal;
}
}
}
