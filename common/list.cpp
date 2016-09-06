#include "list.h"

namespace paso {
namespace data {
namespace entity {

List::List(const QString &name, bool permanent, quint64 id,
           const QDate &expiryDate)
    : Entity(id), mName(name), mSystem(false), mPermanent(permanent),
      mDemonstrators(false) {}

List::List(const QVariantMap &map)
    : Entity(map["ID"].toULongLong()), mName(map["NAME"].toString()),
      mSystem(map["SYSTEM"].toBool()), mPermanent(map["PERMANENT"].toBool()),
      mDemonstrators(map["DEMONSTRATORS"].toBool()),
      mExpiryDate(map["EXPIRY_DATE"].toDate()) {}

bool List::operator==(const List &other) const {
    if (this == &other) {
        return true;
    }

    return id() == other.id() && mName == other.mName &&
           mSystem == other.mSystem && mPermanent == other.mPermanent &&
           mDemonstrators == other.mDemonstrators;
}

QString List::name() const { return mName; }

void List::setName(const QString &name) { mName = name; }

bool List::system() const { return mSystem; }

bool List::permanent() const { return mPermanent; }

void List::setPermanent(bool permanent) { mPermanent = permanent; }

bool List::demonstrators() const { return mDemonstrators; }

void List::setDemonstrators(bool demonstrators) {
    mDemonstrators = demonstrators;
}

QDate List::expiryDate() const { return mExpiryDate; }

void List::setExpiryDate(const QDate &expiryDate) { mExpiryDate = expiryDate; }

QVariantMap List::toVariantMap() const {
    auto retVal = Entity::toVariantMap();
    retVal.insert("NAME", mName);
    retVal.insert("SYSTEM", mSystem);
    retVal.insert("PERMANENT", mPermanent);
    retVal.insert("DEMONSTRATORS", mDemonstrators);
    retVal.insert("EXPIRY_DATE", mExpiryDate);
    return retVal;
}

QVariant List::value(const QString &property) const {
    if (property == "NAME") {
        return mName;
    } else if (property == "SYSTEM") {
        return mSystem;
    } else if (property == "PERMANENT") {
        return mPermanent;
    } else if (property == "DEMONSTRATORS") {
        return mDemonstrators;
    } else if (property == "EXPIRY_DATE") {
        return mExpiryDate;
    }

    return Entity::value(property);
}

void List::read(const QJsonObject &jsonObject) {
    setId(jsonObject["ID"].toVariant().toULongLong());
    mName = jsonObject["NAME"].toString();
    mSystem = jsonObject["SYSTEM"].toBool();
    mPermanent = jsonObject["PERMANENT"].toBool();
    mDemonstrators = jsonObject["DEMONSTRATORS"].toBool();
    mExpiryDate = jsonObject["EXPIRY_DATE"].toVariant().toDate();
}

void List::write(QJsonObject &jsonObject) const {
    jsonObject["ID"] = static_cast<qint64>(id());
    jsonObject["NAME"] = mName;
    jsonObject["SYSTEM"] = mSystem;
    jsonObject["PERMANENT"] = mPermanent;
    jsonObject["DEMONSTRATORS"] = mDemonstrators;
    jsonObject["EXPIRY_DATE"] = QVariant(mExpiryDate).toJsonValue();
}

QSqlQuery List::findByNameQuery(const QSqlDatabase &database,
                                const QString &name) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM LIST WHERE NAME = :name");
    query.bindValue(":name", name);

    return query;
}

QSqlQuery List::addStudentToListQuery(const QSqlDatabase &database,
                                      quint64 listId,
                                      const QString &indexNumber) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO MEMBER(ID_LIST, ID_STUDENT) "
                  "SELECT L.ID, S.ID "
                  "  FROM LIST L, STUDENT S "
                  " WHERE L.ID = :list_id_1 "
                  "   AND S.INDEX_NUMBER = :index_number_1 "
                  "   AND (SELECT COUNT(1) "
                  "          FROM MEMBER M "
                  "         WHERE M.ID_LIST = L.ID "
                  "           AND M.ID_STUDENT = S.ID "
                  "           AND L.ID = :list_id_2 "
                  "           AND S.INDEX_NUMBER = :index_number_2) = 0");
    query.bindValue(":list_id_1", listId);
    query.bindValue(":index_number_1", indexNumber);
    query.bindValue(":list_id_2", listId);
    query.bindValue(":index_number_2", indexNumber);
    return query;
}

QSqlQuery List::removeStudentFromListQuery(const QSqlDatabase &database,
                                           quint64 listId,
                                           const QString &indexNumber) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM MEMBER"
                  " WHERE ID_LIST = :list_id"
                  "   AND ID_STUDENT = (SELECT ID"
                  "                       FROM STUDENT"
                  "                      WHERE INDEX_NUMBER = :index_number)");
    query.bindValue(":list_id", listId);
    query.bindValue(":index_number", indexNumber);
    return query;
}

QSqlQuery List::membersQuery(const QSqlDatabase &database, quint64 listId) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM LIST_MEMBERS WHERE LIST_ID = :list_id");
    query.bindValue(":list_id", listId);

    return query;
}

QSqlQuery List::nonMembersQuery(const QSqlDatabase &database, quint64 listId) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM LIST_MEMBERS "
                  " WHERE COALESCE(LIST_ID, -1) <> :list_id_1 "
                  "  AND ID NOT IN (SELECT ID FROM LIST_MEMBERS WHERE "
                  "LIST_ID = :list_id_2)");
    query.bindValue(":list_id_1", listId);
    query.bindValue(":list_id_2", listId);

    return query;
}

QSqlQuery List::removeAllStudentsFromListQuery(const QSqlDatabase &database,
                                               quint64 listId) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM MEMBER WHERE ID_LIST = :list_id");
    query.bindValue(":list_id", listId);

    return query;
}
}
}
}
