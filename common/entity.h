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
    /// \brief Constructs new entity.
    /// \param id The id (primary key) of an entity. The value of zero is
    /// considered as indicator that this is a new entity not yet persisted.
    ///
    explicit Entity(quint64 id = 0);

    ///
    /// \brief Returns entity id.
    /// \return The id.
    ///
    quint64 id() const;

    ///
    /// \brief Sets entity id.
    /// \param id The id to set.
    ///
    void setId(const quint64 id);

    ///
    /// \brief Returns variant map representation of an entity.
    /// Derived classes should first call this method before adding their own
    /// properties to the map.
    /// \return The variant map representation of entity.
    ///
    virtual QVariantMap toVariantMap() const;

    ///
    /// \brief Returns value of given property. If property does not exist, null
    /// variant will be returned.
    /// \see operator []()
    /// \param property The property to get.
    /// \return The value of the property.
    ///
    virtual QVariant value(const QString &property) const;

    ///
    /// \brief operator [] Returns value of given property. If property does not
    /// exist, null variant will be returned.
    /// \see value()
    /// \param property The property to get.
    /// \return The value of the property.
    ///
    QVariant operator[](const QString &property) const;

private:
    quint64 mId; //!< The entity id.
};

using EntityVector = std::vector<std::shared_ptr<paso::data::entity::Entity>>;
}
}
}

#endif // ENTITY_H
