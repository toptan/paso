#include "data.h"

#include <QJsonDocument>

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

}
}
