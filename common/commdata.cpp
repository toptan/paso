#include "commdata.h"

namespace paso {
namespace comm {

using namespace data;

LoginResponse::LoginResponse() : JsonSerializable(), mSystemUser(""), mDbPort(0) {}

LoginResponse::LoginResponse(const SystemUser &systemUser,
                             const QString &dbName, const QString &dbServer,
                             const QString &dbUsername,
                             const QString &dbPassword, uint16_t dbPort)
    : JsonSerializable(), mSystemUser(systemUser), mDbName(dbName), mDbServer(dbServer),
      mDbUsername(dbUsername), mDbPassword(dbPassword), mDbPort(dbPort) {}

const data::SystemUser &LoginResponse::systemUser() const {
    return mSystemUser;
}

QString LoginResponse::dbName() const { return mDbName; }

QString LoginResponse::dbServer() const { return mDbServer; }

QString LoginResponse::dbUsername() const { return mDbUsername; }

QString LoginResponse::dbPassword() const { return mDbPassword; }

uint16_t LoginResponse::dbPort() const { return mDbPort; }

void LoginResponse::read(const QJsonObject &jsonObject) {
    mDbName = jsonObject["DB_NAME"].toString();
    mDbServer = jsonObject["DB_SERVER"].toString();
    mDbUsername = jsonObject["DB_USERNAME"].toString();
    mDbPassword = jsonObject["DB_PASSWORD"].toString();
    mDbPort = static_cast<unsigned short>(jsonObject["DB_PORT"].toInt());
    mSystemUser.read(jsonObject["SYSTEM_USER"].toObject());
}

void LoginResponse::write(QJsonObject &jsonObject) const {
    jsonObject["DB_NAME"] = mDbName;
    jsonObject["DB_SERVER"] = mDbServer;
    jsonObject["DB_USERNAME"] = mDbUsername;
    jsonObject["DB_PASSWORD"] = mDbPassword;
    jsonObject["DB_PORT"] = mDbPort;
    QJsonObject systemUser;
    mSystemUser.write(systemUser);
    jsonObject["SYSTEM_USER"] = systemUser;
}
}
}
