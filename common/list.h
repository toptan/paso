#ifndef LIST_H
#define LIST_H

#include "entity.h"
#include "jsonserializable.h"

#include <QJsonDocument>
#include <QString>

namespace paso {
namespace data {
namespace entity {

///
/// The List class encapsulates data about student lists.
///
class List : public Entity, public JsonSerializable {
public:
    ///
    /// Constructs new list with given data.
    ///
    /// \param name The list name.
    /// \param permanent Whether list is permanent.
    /// \param id The list id.
    ///
    List(const QString &name = "", bool permanent = false, uint64_t id = 0);

    ///
    /// Constructs list from the data given in the map.
    ///
    /// \param map The map that contains data for the list.
    ///
    explicit List(const QVariantMap &map);

    ///
    /// The equality operator. Two lists are the equal if all their properties
    /// are equial.
    ///
    /// \param other The list to compare to.
    /// \return \c true if lists are equal.
    ///
    bool operator==(const List &other) const;

    ///
    /// \brief name Returns list name.
    /// \return The list name.
    ///
    QString name() const;

    ///
    /// \brief setName Sets new list name.
    /// \param name The name to set.
    ///
    void setName(const QString &name);

    ///
    /// \brief system Returns whether this is a system list.
    ///
    /// \return \c true if this is system list.
    ///
    bool system() const;

    ///
    /// \brief permanent Returns whether this is permanent list.
    ///
    /// \return \c true if this list is permanent.
    ///
    bool permanent() const;

    ///
    /// \brief setPermanent Sets whether this list should be permanent.
    ///
    /// \param permanent Whether this list should be permanent.
    ///
    void setPermanent(bool permanent);

    ///
    /// \ref Entity::toVariantMap();
    ///
    virtual QVariantMap toVariantMap() const override;

    ///
    /// \ref Entity::value(const QStirng &);
    ///
    virtual QVariant value(const QString &property) const override;

    ///
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const override;

    ///
    /// \brief findByNameQuery Returns find by list name query with bound name
    /// value.
    /// \param database The database to use.
    /// \param name The list name.
    /// \return The query.
    ///
    static QSqlQuery findByNameQuery(const QSqlDatabase &database,
                                     const QString &name);

private:
    QString mName;   //!< The list name.
    bool mSystem;    //!< Whether this is a system list.
    bool mPermanent; //!< Whether this list is permanent.
};
}
}
}

#endif // LIST_H
