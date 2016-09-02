#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "data.h"
#include "entity.h"
#include "jsonserializable.h"

namespace paso {
namespace data {
namespace entity {

///
/// \brief The Activity class encapsulates data about activities.
///
class Activity : public Entity, public JsonSerializable {
public:
    ///
    /// \brief Constructs new activity with given data.
    /// \param name Activity name.
    /// \param type Activity type.
    /// \param id Activity id.
    ///
    explicit Activity(const QString &name = "",
                      ActivityType type = ActivityType::INVALID_ACTIVITY,
                      quint64 id = 0);

    ///
    /// \brief Constructs activity from the data given in the map.
    /// \param map The map that contains data for the activity.
    ///
    explicit Activity(const QVariantMap &map);

    ///
    /// \brief The equality operator. Two activities are equal if all of their
    /// properties are equal.
    /// \param other Object to compare to.
    /// \return \c true if two activities are equal/the same.
    ///
    bool operator==(const Activity &other) const;

    ///
    /// \brief Returns activity name.
    /// \return Activity name.
    ///
    QString name() const;

    ///
    /// \brief Sets activity name.
    /// \param name The name to set.
    ///
    void setName(const QString &name);

    ///
    /// \brief Returns activity type.
    /// \return Activity type.
    ///
    ActivityType type() const;

    ///
    /// \brief Sets activity type.
    /// \param type The type to set.
    ///
    void setType(const ActivityType &type);

    ///
    /// \brief Returns activity schedule type.
    /// \return Activity schedule type.
    ///
    ActivityScheduleType scheduleType() const;

    ///
    /// \brief Sets activity schedule type.
    /// \param scheduleType The new schedule type to set.
    ///
    void setScheduleType(const ActivityScheduleType &scheduleType);

    ///
    /// \brief Returns the list of scheduled days. The list simply contains
    /// either week day numbers, or month day numbers.
    /// \return The list of scheduled days.
    ///
    QVariantList scheduledDays() const;

    ///
    /// \brief Sets scheduled days.
    /// \param scheduledDays The list of scheduled days.
    ///
    void setScheduledDays(const QVariantList &scheduledDays);

    ///
    /// \brief Returns activity duration in hours for each occurence.
    /// \return Activity duration in hours.
    ///
    QTime duration() const;

    ///
    /// \brief Sets activity duration in hours for each occurence.
    /// \param duration Activity duration in hours.
    ///
    void setDuration(const QTime &duration);

    ///
    /// \brief Returns activity start date.
    /// \return Activity start date.
    ///
    QDate startDate() const;

    ///
    /// \brief Sets activity start date.
    /// \param startDate Activity start date.
    ///
    void setStartDate(const QDate &startDate);

    ///
    /// \brief Returns activity finish date.
    /// \return Activity finish date.
    ///
    QDate finishDate() const;

    ///
    /// \brief Sets activity finish date.
    /// \param finishDate Activity finish date.
    ///
    void setFinishDate(const QDate &finishDate);

    ///
    /// \brief Returns whether activity allows entry/overlap even if some other
    /// activity is in progress.
    /// This applies only to \ref ActivityType::INDIVIDUAL_WORK activities.
    /// \return Whether entry/overlap is allowed.
    ///
    bool canOverlap() const;

    ///
    /// \brief Sets whether activity allows entry/overlap even if some other
    /// activity is in progress.
    /// This applies only to \ref ActivityType::INDIVIDUAL_WORK activities.
    /// \param canOverlap Whether entry/overlap is allowe.d
    ///
    void setCanOverlap(bool canOverlap);

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
    /// \brief Returns insert query for given activity with values bound.
    /// \param database The database to use.
    /// \param activity The activity to insert.
    /// \return The query.
    ///
    static QSqlQuery insertQuery(const QSqlDatabase &database,
                                 const Activity &activity);

    ///
    /// \brief Returns update query for given activity with values bound.
    /// \param database The database to use.
    /// \param activity The activity to update.
    /// \return The query.
    ///
    static QSqlQuery updateQuery(const QSqlDatabase &database,
                                 const Activity &activity);

    ///
    /// \brief Returns delete query for given activity id with values bound.
    /// \param database The database to use.
    /// \param activityId The id of activity to delete.
    /// \return The query.
    ///
    static QSqlQuery deleteQuery(const QSqlDatabase &database,
                                 quint64 activityId);

    ///
    /// \brief Returns the query that finds activity by its id.
    /// \param database The database to use.
    /// \param activityId The activity id.
    /// \return The query.
    ///
    static QSqlQuery findActivityByIdQuery(const QSqlDatabase &database,
                                           quint64 activityId);

    ///
    /// \brief Returns query that finds all lists associated with given
    /// activity.
    /// \param database The database to use.
    /// \param activityId The activity id.
    /// \return The query.
    ///
    static QSqlQuery findActivityListsQuery(const QSqlDatabase &database,
                                            quint64 activityId);

    ///
    /// \brief Returns query that finds all lists that are not associated with
    /// given activity.
    /// \param database The database to use.
    /// \param activityId The activity id.
    /// \return The query.
    ///
    static QSqlQuery findNonActivityListsQuery(const QSqlDatabase &database,
                                               quint64 activityId);

    ///
    /// \brief Returns query that finds all rooms associated with given
    /// activity.
    /// \param database The database to use.
    /// \param activityId The activity id.
    /// \return The query.
    ///
    static QSqlQuery findActivityRoomsQuery(const QSqlDatabase &database,
                                            quint64 activityId);

    ///
    /// \brief Returns query that finds all rooms that are not associated with
    /// given activity.
    /// \param database The database to use.
    /// \param activityId The activity id.
    /// \return The query.
    ///
    static QSqlQuery findNonActivityRoomsQuery(const QSqlDatabase &database,
                                               quint64 activityId);

    ///
    /// \brief Returns query that associates given lists with the activity.
    /// \param database The database to use.
    /// \param activityId The activity id.
    /// \param listIds The list of list ids.
    /// \return The query.
    ///
    static QSqlQuery setActivityListsQuery(const QSqlDatabase &database,
                                           quint64 activityId,
                                           QList<quint64> listIds);

    ///
    /// \brief Returns query that associates given rooms with the activity.
    /// \param database The database to use.
    /// \param activityId The activity id.
    /// \param roomIds The list of room ids.
    /// \return The query.
    ///
    static QSqlQuery setActivityRoomsQuery(const QSqlDatabase &database,
                                           quint64 activityId,
                                           QList<quint64> roomIds);

private:
    QString mName;                      //!< The activity name.
    ActivityType mType;                 //!< The activity type.
    ActivityScheduleType mScheduleType; //!< The activity schedule type.
    QVariantList mScheduledDays;        //!< The array of scheduled days.
    QTime mDuration;                    //!< The activity duration.
    QDate mStartDate;                   //!< The activity start date.
    QDate mFinishDate;                  //!< The activity finish date.

    /// \brief Whether activity allows entry/overlap even if some other activity
    /// is in progress. This applies only to \ref ActivityType::INDIVIDUAL_WORK
    /// activities.
    bool mCanOverlap;
};
}
}
}
#endif // ACTIVITY_H
