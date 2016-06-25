#ifndef ROOM_H
#define ROOM_H

#include "jsonserializable.h"

#include <QJsonDocument>
#include <QUuid>

namespace paso {
namespace data {

///
/// \brief The Room class encapsulates data about rooms/laboratories.
///
class Room : public JsonSerializable {
public:
    ///
    /// \brief Room constructs new room with given data.
    /// \param roomUUID room UUID.
    /// \param name room name.
    /// \param number room number.
    ///
    Room(const QString &roomUUID, const QString &name, const QString &number);

    ///
    /// \brief Room constructs room from the data given in the map.
    /// \param map a map that contains data for the room.
    ///
    explicit Room(const QVariantMap &map);

    ///
    /// \brief Room The copy constructor.
    /// \param other an object to copy from.
    ///
    explicit Room(const Room &other);

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
    /// \ref JsonSerializable::read(const QJsonObject&);
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \ref JsonSerializable::write(QJsonObject&);
    ///
    virtual void write(QJsonObject &jsonObject) const override;

private:
    QString mRoomUUID; //!< The room UUID
    QString mName;     //!< The room name
    QString mNumber;   //!< The room number, i.e. 56, P25
};
}
}

#endif // ROOM_H
