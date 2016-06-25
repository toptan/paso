#include "jsonserializable.h"

#include <QJsonDocument>

namespace paso {
namespace data {

JsonSerializable::~JsonSerializable() {}

void JsonSerializable::fromJsonString(const QString &jsonString) {
    QJsonObject jsonObject =
        QJsonDocument::fromJson(jsonString.toUtf8()).object();
    read(jsonObject);
}

QString JsonSerializable::toJsonString() const {
    QJsonObject jsonObject;
    write(jsonObject);
    QJsonDocument doc;
    doc.setObject(jsonObject);
    return doc.toJson();
}
}
}
