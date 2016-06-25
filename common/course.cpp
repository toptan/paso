#include "course.h"

#include <QString>
#include <QVariantMap>

namespace paso {
namespace data {

Course::Course(const QString &code, const QString &name)
    : mCode(code), mName(name) {}

Course::Course(const QVariantMap &map)
    : mCode(map["CODE"].toString()), mName(map["NAME"].toString()) {}

//Course::Course(const Course &other) : mCode(other.mCode), mName(other.mName) {}

bool Course::operator==(const Course &other) const {
    if (this == &other) {
        return true;
    }

    return mCode == other.mCode && mName == other.mName;
}

QString Course::code() const { return mCode; }

QString Course::name() const { return mName; }

void Course::setName(const QString &name) { mName = name; }

void Course::read(const QJsonObject &jsonObject) {
    mCode = jsonObject["CODE"].toString();
    mName = jsonObject["NAME"].toString();
}

void Course::write(QJsonObject &jsonObject) const {
    jsonObject["CODE"] = mCode;
    jsonObject["NAME"] = mName;
}
}
}
