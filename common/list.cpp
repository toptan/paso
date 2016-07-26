#include "list.h"

namespace paso {
namespace data {
namespace entity {

List::List(const QString &name, bool permanent, uint64_t id)
    : Entity(id), mName(name), mSystem(false), mPermanent(permanent) {}

List::List(const QVariantMap &map)
    : Entity(map["ID"].toULongLong()), mName(map["NAME"].toString()),
      mSystem(map["SYSTEM"].toBool()), mPermanent(map["PERMANENT"].toBool()) {}

bool List::operator==(const List &other) const {
    if (this == &other) {
        return true;
    }

    return id() == other.id() && mName == other.mName &&
           mSystem == other.mSystem && mPermanent == other.mPermanent;
}

QString List::name() const { return mName; }

void List::setName(const QString &name) { mName = name; }

bool List::system() const { return mSystem; }

bool List::permanent() const { return mPermanent; }

void List::setPermanent(bool permanent) { mPermanent = permanent; }

QVariantMap List::toVariantMap() const {
    auto retVal = Entity::toVariantMap();
    retVal.insert("NAME", mName);
    retVal.insert("SYSTEM", mSystem);
    retVal.insert("PERMANENT", mPermanent);
    return retVal;
}

QVariant List::value(const QString &property) const {
    if (property == "NAME") {
        return mName;
    } else if (property == "SYSTEM") {
        return mSystem;
    } else if (property == "PERMANENT") {
        return mPermanent;
    }

    return Entity::value(property);
}

void List::read(const QJsonObject &jsonObject) {
    setId(jsonObject["ID"].toVariant().toULongLong());
    mName = jsonObject["NAME"].toString();
    mSystem = jsonObject["SYSTEM"].toBool();
    mPermanent = jsonObject["PERMANENT"].toBool();
}

void List::write(QJsonObject &jsonObject) const {
    jsonObject["ID"] = static_cast<qint64>(id());
    jsonObject["NAME"] = mName;
    jsonObject["SYSTEM"] = mSystem;
    jsonObject["PERMANENT"] = mPermanent;
}

QSqlQuery List::findByNameQuery(const QSqlDatabase &database, const QString &name) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM LIST WHERE NAME = :name");
    query.bindValue(":name", name);

    return query;
}
}
}
}
