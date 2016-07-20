#ifndef ENTITY_H
#define ENTITY_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariantMap>
#include <cstdint>
#include <memory>
#include <vector>

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
    explicit Entity(uint64_t id = 0);

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

    ///
    /// \brief toVariantMap Returns variant map representation of an entity.
    /// Derived classes should first call this method before adding their own
    /// properties to the map.
    /// \return The variant map representation of entity.
    ///
    virtual QVariantMap toVariantMap() const;

    ///
    /// \brief value Returns value of given property. If property does not
    /// exist, null variant will be returned.
    /// \param propertyName The property.
    /// \return The value of the property.
    ///
    virtual QVariant value(const QString &property) const;

private:
    uint64_t mId;
};

using EntityVector =
    std::shared_ptr<std::vector<std::shared_ptr<paso::data::entity::Entity>>>;
}
}
}

#endif // ENTITY_H
