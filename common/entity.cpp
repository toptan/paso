#include "entity.h"

namespace paso {
namespace data {
namespace entity {

Entity::Entity(uint64_t id) : mId(id) {}

uint64_t Entity::id() const { return mId; }

void Entity::setId(const uint64_t id) { mId = id; }
}
}
}
