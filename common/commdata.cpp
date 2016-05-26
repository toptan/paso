#include "commdata.h"

namespace paso {
namespace comm {

using namespace data;

LoginResponse::LoginResponse(const SystemUser &systemUser,
                             const QString &dbName, const QString &dbServer,
                             const QString &dbUsername,
                             const QString &dbPassword, uint16_t dbPort)
    : mSystemUser(systemUser), mDbName(dbName), mDbServer(dbServer),
      mDbUsername(dbUsername), mDbPassword(dbPassword), mDbPort(dbPort) {}

const data::SystemUser &LoginResponse::systemUser() const {
    return mSystemUser;
}

QString LoginResponse::dbName() const { return mDbName; }

QString LoginResponse::dbServer() const { return mDbServer; }

QString LoginResponse::dbUsername() const { return mDbUsername; }

QString LoginResponse::dbPassword() const { return mDbPassword; }

uint16_t LoginResponse::dbPort() const { return mDbPort; }
}
}
