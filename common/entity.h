#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace paso {
namespace data {
namespace entity {

///
/// \brief The Entity class is the base class for all data access object
/// (entities).
///
class Entity {
public:
    ///
    /// \brief Entity Constructs new entity.
    /// \param id The id (primary key) of an entity. The value of zero is
    /// considered as indicator that this is a new entity not yet persisted.
    ///
    Entity(uint64_t id = 0);

    ///
    /// \brief id Returns entity id.
    /// \return The id.
    ///
    uint64_t id() const;

    ///
    /// \brief setId Sets entity id.
    /// \param id The id to set.
    ///
    void setId(const uint64_t id);

private:
    uint64_t mId;
};
}
}
}

#endif // ENTITY_H
