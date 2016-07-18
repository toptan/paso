#ifndef ROOM_H
#define ROOM_H

#include "entity.h"
#include "jsonserializable.h"

#include <QJsonDocument>
#include <QSqlDatabase>
#include <QUuid>

namespace paso {
namespace data {
namespace entity {

///
/// \brief The Room class encapsulates data about rooms/laboratories.
///
class Room : public Entity, public JsonSerializable {
public:
    ///
    /// \brief Room constructs new room with given data.
    /// \param roomUUID room UUID.
    /// \param name room name.
    /// \param number room number.
    /// \param id The room id.
    ///
    Room(const QString &roomUUID, const QString &name, const QString &number,
         uint64_t id = 0);

    ///
    /// \brief Room constructs room from the data given in the map.
    /// \param map a map that contains data for the room.
    ///
    explicit Room(const QVariantMap &map);

    ///
    /// \brief operator == The equality operator. Two rooms are equal if all
    /// their properties are equal.
    /// \param other a room to compare to.
    /// \return \code true if rooms are equal.
    ///
    bool operator==(const Room &other) const;

    ///
    /// \brief roomUUID returns room UUID.
    /// \return room UUID.
    ///
    QString roomUUID() const;

    ///
    /// \brief name returns room name.
    /// \return room name.
    ///
    QString name() const;

    ///
    /// \brief setName sets new room name.
    /// \param name a new room name.
    ///
    void setName(const QString &name);

    ///
    /// \brief number returns room number.
    /// \return room number.
    ///
    QString number() const;

    ///
    /// \brief setNumber sets new room number.
    /// \param number a new room number.
    ///
    void setNumber(const QString &number);

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
    /// \brief insertQuery Returns insert query for given room with values
    /// bound.
    /// \param database The database to use.
    /// \param room The room to insert.
    /// \return The query.
    ///
    static QSqlQuery insertQuery(const QSqlDatabase &database,
                                 const Room &room);

    ///
    /// \brief updateQuery Returns update query for given course with values
    /// bound.
    /// \param database The database to use.
    /// \param room The room to update.
    /// \return The query.
    ///
    static QSqlQuery updateQuery(const QSqlDatabase &database,
                                 const Room &room);

    ///
    /// \brief findByUuidQuery Returns find by UUID query with bound UUID value.
    /// \param database The database to use.
    /// \param uuid The room UUID.
    /// \return The query.
    ///
    static QSqlQuery findByUuidQuery(const QSqlDatabase &database,
                                     const QUuid &uuid);

    ///
    /// \brief findByNumberQuery Returns find by room number query with bound
    /// number value.
    /// \param database The database to use.
    /// \param number The room number.
    /// \return The query.
    ///
    static QSqlQuery findByNumberQuery(const QSqlDatabase &database,
                                       const QString &number);

    ///
    /// \brief findAllQuery Returns find all rooms query.
    /// \param database The database to use.
    /// \return The query.
    ///
    static QSqlQuery findAllQuery(const QSqlDatabase &database);

    ///
    /// \brief deleteByUuidQuery Returns delete by UUID query with bound UUID
    /// value.
    /// \param database The database to use.
    /// \param uuid The room UUID.
    /// \return The query.
    ///
    static QSqlQuery deleteByUuidQuery(const QSqlDatabase &database,
                                       const QUuid &uuid);

private:
    QString mRoomUUID; //!< The room UUID
    QString mName;     //!< The room name
    QString mNumber;   //!< The room number, i.e. 56, P25
};
}
}
}

#endif // ROOM_H
