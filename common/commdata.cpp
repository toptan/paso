#include "commdata.h"

namespace paso {
namespace comm {

using namespace data;
using namespace data::entity;

const QString operationToString(Operation operation) {
    switch (operation) {
    case Operation::LOGIN:
        return "login";
    default:
        return "unknown_operation";
    }
}

Operation operationFromString(const QString &operation) {
    if (operation == "login") {
        return Operation::LOGIN;
    } else {
        return Operation::UNKNOWN_OPERATION;
    }
}

// Base methods.

Base::Base(const QString &jsonString)
    : JsonSerializable(), mOperation(Operation::UNKNOWN_OPERATION) {
    JsonSerializable::fromJsonString(jsonString);
}

Base::Base(const QUuid &roomId, const Operation &operation)
    : JsonSerializable(), mRoomId(roomId), mOperation(operation) {}

QUuid Base::roomId() const { return mRoomId; }

Operation Base::operation() const { return mOperation; }

void Base::read(const QJsonObject &jsonObject) {
    mRoomId = QUuid(jsonObject["ROOM_ID"].toString());
    mOperation = operationFromString(jsonObject["OPERATION"].toString());
}

void Base::write(QJsonObject &jsonObject) const {
    jsonObject["ROOM_ID"] = mRoomId.toString();
    jsonObject["OPERATION"] = operationToString(mOperation);
}

// LoginRequest methods.

LoginRequest::LoginRequest()
    : Base(QString("{00000000-0000-0000-0000-000000000000}"),
           Operation::LOGIN) {}

LoginRequest::LoginRequest(const QString &username, const QString &password)
    : Base(QUuid("{00000000-0000-0000-0000-000000000000}"), Operation::LOGIN),
      mUsername(username), mPassword(password) {}

QString LoginRequest::username() const { return mUsername; }

QString LoginRequest::password() const { return mPassword; }

void LoginRequest::read(const QJsonObject &jsonObject) {
    Base::read(jsonObject);
    mUsername = jsonObject["USERNAME"].toString();
    mPassword = jsonObject["PASSWORD"].toString();
}

void LoginRequest::write(QJsonObject &jsonObject) const {
    Base::write(jsonObject);
    jsonObject["USERNAME"] = mUsername;
    jsonObject["PASSWORD"] = mPassword;
}

// LoginResponse methods.

LoginResponse::LoginResponse()
    : Base("{00000000-0000-0000-0000-000000000000}", Operation::LOGIN),
      mSystemUser(""), mDbPort(0) {}

LoginResponse::LoginResponse(const SystemUser &systemUser,
                             const QString &dbType, const QString &dbName,
                             const QString &dbServer, const QString &dbUsername,
                             const QString &dbPassword, uint16_t dbPort)
    : Base("{00000000-0000-0000-0000-000000000000}", Operation::LOGIN),
      mSystemUser(systemUser), mDbType(dbType), mDbName(dbName),
      mDbServer(dbServer), mDbUsername(dbUsername), mDbPassword(dbPassword),
      mDbPort(dbPort) {}

const SystemUser &LoginResponse::systemUser() const { return mSystemUser; }

QString LoginResponse::dbType() const { return mDbType; }

QString LoginResponse::dbName() const { return mDbName; }

QString LoginResponse::dbServer() const { return mDbServer; }

QString LoginResponse::dbUsername() const { return mDbUsername; }

QString LoginResponse::dbPassword() const { return mDbPassword; }

uint16_t LoginResponse::dbPort() const { return mDbPort; }

void LoginResponse::read(const QJsonObject &jsonObject) {
    Base::read(jsonObject);
    mDbType = jsonObject["DB_TYPE"].toString();
    mDbName = jsonObject["DB_NAME"].toString();
    mDbServer = jsonObject["DB_SERVER"].toString();
    mDbUsername = jsonObject["DB_USERNAME"].toString();
    mDbPassword = jsonObject["DB_PASSWORD"].toString();
    mDbPort = static_cast<unsigned short>(jsonObject["DB_PORT"].toInt());
    mSystemUser.read(jsonObject["SYSTEM_USER"].toObject());
}

void LoginResponse::write(QJsonObject &jsonObject) const {
    Base::write(jsonObject);
    jsonObject["DB_TYPE"] = mDbType;
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
