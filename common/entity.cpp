#include "entity.h"

namespace paso {
namespace data {
namespace entity {

Entity::Entity(quint64 id) : mId(id) {}

quint64 Entity::id() const { return mId; }

void Entity::setId(const quint64 id) { mId = id; }

QVariantMap Entity::toVariantMap() const {
    QVariantMap retVal({{"ID", mId}});
    return retVal;
}

QVariant Entity::value(const QString &property) const {
    if (property == "ID") {
        return mId;
    }
    return QVariant();
}

QVariant Entity::operator[](const QString &property) const {
    return value(property);
}
}
}
}
