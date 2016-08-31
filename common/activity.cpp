#include "activity.h"

#include <QTextStream>

namespace paso {
namespace data {
namespace entity {

Activity::Activity(const QString &name, ActivityType type, quint64 id)
    : Entity(id), mName(name), mType(type),
      mScheduleType(ActivityScheduleType::INVALID), mDuration(), mStartDate(),
      mFinishDate(), mCanOverlap(false) {}

Activity::Activity(const QVariantMap &map)
    : Entity(map["ID"].toULongLong()), mName(map["NAME"].toString()),
      mType(stringToActivityType(map["ROLE"].toString())),
      mScheduleType(
          stringToActivityScheduleType(map["SCHEDULE_TYPE"].toString())),
      mDuration(map["DURATION"].toTime()),
      mStartDate(map["START_DATE"].toDate()),
      mFinishDate(map["FINISH_DATE"].toDate()),
      mCanOverlap(map["CAN_OVERLAP"].toBool()) {}

bool Activity::operator==(const Activity &other) const {
    if (this == &other) {
        return true;
    }

    return id() == other.id() && mName == other.mName && mType == other.mType &&
           mScheduleType == other.mScheduleType &&
           mDuration == other.mDuration && mStartDate == other.mStartDate &&
           mFinishDate == other.mFinishDate && mCanOverlap == other.mCanOverlap;
}

QString Activity::name() const { return mName; }

void Activity::setName(const QString &name) { mName = name; }

ActivityType Activity::type() const { return mType; }

void Activity::setType(const ActivityType &type) { mType = type; }

ActivityScheduleType Activity::scheduleType() const { return mScheduleType; }

void Activity::setScheduleType(const ActivityScheduleType &scheduleType) {
    mScheduleType = scheduleType;
}

QTime Activity::duration() const { return mDuration; }

void Activity::setDuration(const QTime &duration) { mDuration = duration; }

QDate Activity::startDate() const { return mStartDate; }

void Activity::setStartDate(const QDate &startDate) { mStartDate = startDate; }

QDate Activity::finishDate() const { return mFinishDate; }

void Activity::setFinishDate(const QDate &finishDate) {
    mFinishDate = finishDate;
}

bool Activity::canOverlap() const { return mCanOverlap; }

void Activity::setCanOverlap(bool canOverlap) { mCanOverlap = canOverlap; }

QVariantMap Activity::toVariantMap() const {
    auto retVal = Entity::toVariantMap();
    retVal.insert("NAME", mName);
    retVal.insert("TYPE", activityTypeToString(mType));
    retVal.insert("SCHEDULE_TYPE", activityScheduleTypeToString(mScheduleType));
    retVal.insert("DURATION", mDuration);
    retVal.insert("START_DATE", mStartDate);
    retVal.insert("FINISH_DATE", mFinishDate);
    retVal.insert("CAN_OVERLAP", mCanOverlap);
    return retVal;
}

QVariant Activity::value(const QString &property) const {
    if (property == "NAME") {
        return mName;
    } else if (property == "TYPE") {
        return activityTypeToString(mType);
    } else if (property == "SCHEDULE_TYPE") {
        return activityScheduleTypeToString(mScheduleType);
    } else if (property == "DURATION") {
        return mDuration;
    } else if (property == "START_DATE") {
        return mStartDate;
    } else if (property == "FINISH_DATE") {
        return mFinishDate;
    } else if (property == "CAN_OVERLAP") {
        return mCanOverlap;
    }

    return Entity::value(property);
}

void Activity::read(const QJsonObject &jsonObject) {
    setId(jsonObject["ID"].toVariant().toULongLong());
    mName = jsonObject["NAME"].toString();
    auto strType = jsonObject["TYPE"].toString();
    mType = stringToActivityType(strType);
    strType = jsonObject["SCHEDULE_TYPE"].toString();
    mScheduleType = stringToActivityScheduleType(strType);
    mDuration = jsonObject["DURATION"].toVariant().toTime();
    mStartDate = jsonObject["START_DATE"].toVariant().toDate();
    mFinishDate = jsonObject["FINISH_DATE"].toVariant().toDate();
    mCanOverlap = jsonObject["CAN_OVERLAP"].toBool();
}

void Activity::write(QJsonObject &jsonObject) const {
    jsonObject["ID"] = static_cast<qint64>(id());
    jsonObject["NAME"] = mName;
    jsonObject["TYPE"] = activityTypeToString(mType);
    jsonObject["SCHEDULE_TYPE"] = activityScheduleTypeToString(mScheduleType);
    jsonObject["DURATION"] = QVariant(mDuration).toJsonValue();
    jsonObject["START_DATE"] = QVariant(mStartDate).toJsonValue();
    jsonObject["FINISH_DATE"] = QVariant(mFinishDate).toJsonValue();
    jsonObject["CAN_OVERLAP"] = mCanOverlap;
}

QSqlQuery Activity::findActivityListsQuery(const QSqlDatabase &database,
                                           quint64 activityId) {
    QSqlQuery query(database);
    query.prepare("SELECT L.*"
                  "  FROM LIST L"
                  "  JOIN ACTIVITY_LISTS AL ON AL.ID_LIST = L.ID"
                  " WHERE AL.ID_ACTIVITY = :activity_id");
    query.bindValue(":activity_id", activityId);
    return query;
}

QSqlQuery Activity::findNonActivityListsQuery(const QSqlDatabase &database,
                                              quint64 activityId) {
    QSqlQuery query(database);
    query.prepare(
        "SELECT DISTINCT L.*"
        "  FROM LIST L"
        " WHERE L.ID NOT IN (SELECT L1.ID"
        "						 FROM LIST L1"
        "						 JOIN ACTIVITY_LISTS AL"
        "                          ON AL.ID_LIST = L1.ID"
        " 					    WHERE AL.ID_ACTIVITY = "
        ":activity_id)");
    query.bindValue(":activity_id", activityId);
    return query;
}

QSqlQuery Activity::findActivityRoomsQuery(const QSqlDatabase &database,
                                           quint64 activityId) {
    QSqlQuery query(database);
    query.prepare("SELECT R.*"
                  "  FROM ROOM R"
                  "  JOIN ACTIVITY_ROOMS AR ON AR.ID_ROOM = R.ID"
                  " WHERE AR.ID_ACTIVITY = :activity_id");
    query.bindValue(":activity_id", activityId);
    return query;
}

QSqlQuery Activity::findNonActivityRoomsQuery(const QSqlDatabase &database,
                                              quint64 activityId) {
    QSqlQuery query(database);
    query.prepare(
        "SELECT DISTINCT R.*"
        "  FROM ROOM R"
        " WHERE R.ID NOT IN (SELECT R1.ID"
        "						 FROM ROOM R1"
        "						 JOIN ACTIVITY_ROOMS AR"
        "                          ON AR.ID_ROOM = R1.ID"
        " 					    WHERE AR.ID_ACTIVITY = "
        ":activity_id)");
    query.bindValue(":activity_id", activityId);
    return query;
}

QSqlQuery Activity::setActivityListsQuery(const QSqlDatabase &database,
                                          quint64 activityId,
                                          QList<quint64> listIds) {
    QSqlQuery query(database);
    QString strIds;
    QTextStream ts(&strIds);
    for (auto i = 0; i < listIds.size(); i++) {
        ts << listIds[i] << " ";
    }

    query.prepare("SELECT set_activity_lists(:activity_id, :list_ids)");
    query.bindValue(":activity_id", activityId);
    query.bindValue(":list_ids", strIds);
    return query;
}

QSqlQuery Activity::setActivityRoomsQuery(const QSqlDatabase &database,
                                          quint64 activityId,
                                          QList<quint64> roomIds) {
    QSqlQuery query(database);
    QString strIds;
    QTextStream ts(&strIds);
    for (auto i = 0; i < roomIds.size(); i++) {
        ts << roomIds[i] << " ";
    }
    query.prepare("SELECT set_activity_rooms(:activity_id, :room_ids)");
    query.bindValue(":activity_id", activityId);
    query.bindValue(":room_ids", strIds);
    return query;
}
}
}
}
