#include "entity.h"

namespace paso {
namespace data {
namespace entity {

Entity::Entity(uint64_t id) : mId(id) {}

uint64_t Entity::id() const { return mId; }

void Entity::setId(const uint64_t id) { mId = id; }

QVariantMap Entity::toVariantMap() const {
    QVariantMap retVal({{"ID", static_cast<quint64>(mId)}});
    return retVal;
}

QVariant Entity::value(const QString &property) const {
    if (property == "ID") {
        return static_cast<quint64>(mId);
    }
    return QVariant();
}
}
}
}
