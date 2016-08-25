#ifndef LIST_H
#define LIST_H

#include "entity.h"
#include "jsonserializable.h"

#include <QDate>
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
    /// \param expiryDate The date of list expiration.
    /// \param id The list id.
    ///
    List(const QString &name = "", bool permanent = false, quint64 id = 0,
         const QDate &expiryDate = QDate());

    ///
    /// Constructs list from the data given in the map.
    ///
    /// \param map The map that contains data for the list.
    ///
    explicit List(const QVariantMap &map);

    ///
    /// \brief The equality operator. Two lists are the equal if all their
    /// properties are equial.
    /// \param other The list to compare to.
    /// \return \c true if lists are equal.
    ///
    bool operator==(const List &other) const;

    ///
    /// \brief Returns list name.
    /// \return The list name.
    ///
    QString name() const;

    ///
    /// \brief Sets new list name.
    /// \param name The name to set.
    ///
    void setName(const QString &name);

    ///
    /// \brief Returns whether this is a system list.
    /// \return \c true if this is system list.
    ///
    bool system() const;

    ///
    /// \brief Returns whether this is permanent list.
    /// \return \c true if this list is permanent.
    ///
    bool permanent() const;

    ///
    /// \brief Sets whether this list should be permanent.
    /// \param permanent Whether this list should be permanent.
    ///
    void setPermanent(bool permanent);

    ///
    /// \brief Returns the date when this list expires.
    /// \return The date when this list expires.
    ///
    QDate expiryDate() const;

    ///
    /// \brief Sets the date when this list expires.
    /// \param expiryDate The date when this list expires.
    ///
    void setExpiryDate(const QDate &expiryDate);

    ///
    /// \copydoc paso::data::entity::Entity::toVariantMap()
    ///
    virtual QVariantMap toVariantMap() const override;

    ///
    /// \copydoc paso::data::entity::Entity::value()
    ///
    virtual QVariant value(const QString &property) const override;

    ///
    /// \copydoc paso::data::JsonSerializable::read(const QJsonObject&)
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \copydoc paso::data::JsonSerializable::write()
    ///
    virtual void write(QJsonObject &jsonObject) const override;

    ///
    /// \brief Returns find by list name query with bound name value.
    /// \param database The database to use.
    /// \param name The list name.
    /// \return The query.
    ///
    static QSqlQuery findByNameQuery(const QSqlDatabase &database,
                                     const QString &name);

    ///
    /// \brief Returns query that adds student to the list. If student is
    /// already member of the list, the query will do nothing.
    /// \param database The database to use.
    /// \param listId The list id.
    /// \param indexNumber The student's index number.
    /// \return The query.
    ///
    static QSqlQuery addStudentToListQuery(const QSqlDatabase &database,
                                           quint64 listId,
                                           const QString &indexNumber);

    ///
    /// \brief Returns query that removes student from the list. If student is
    /// not a member of the list, the query will do nothing.
    /// \param database The database to use.
    /// \param listId The list id.
    /// \param indexNumber The student's index number.
    /// \return The query.
    ///
    static QSqlQuery removeStudentFromListQuery(const QSqlDatabase &database,
                                                quint64 listId,
                                                const QString &indexNumber);

    ///
    /// \brief Returns query that selects all students that are members of the
    /// list.
    /// \param database The database to use.
    /// \param listId The list id.
    /// \return The query.
    ///
    static QSqlQuery membersQuery(const QSqlDatabase &database, quint64 listId);

    ///
    /// \brief Returns query that selects all students that are not members of
    /// the list.
    /// \param database The database to use.
    /// \param listId The list id.
    /// \return The query.
    ///
    static QSqlQuery nonMembersQuery(const QSqlDatabase &database,
                                     quint64 listId);

    ///
    /// \brief Returns query that removes all students from the given list.
    /// \param database The database to use.
    /// \param listId The list id.
    /// \return The query.
    ///
    static QSqlQuery
    removeAllStudentsFromListQuery(const QSqlDatabase &database,
                                   quint64 listId);

private:
    QString mName;     //!< The list name.
    bool mSystem;      //!< Whether this is a system list.
    bool mPermanent;   //!< Whether this list is permanent.
    QDate mExpiryDate; //!< The date when this list will be expired.
};
}
}
}

#endif // LIST_H
