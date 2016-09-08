#include "commregister.h"

#include <QJsonArray>

namespace paso {
namespace comm {

RegisterRequest::RegisterRequest()
    : Base(QString("{00000000-0000-0000-0000-000000000000}"),
           Operation::REGISTER) {}

RegisterRequest::RegisterRequest(const QUuid &roomId)
    : Base(roomId, Operation::REGISTER) {}

RegisterResponse::RegisterResponse()
    : Base(QString("{00000000-0000-0000-0000-000000000000}"),
           Operation::REGISTER) {}

RegisterResponse::RegisterResponse(const QUuid &roomId, bool success,
                                   quint16 port,
                                   const QStringList &emergencyData)
    : Base(roomId, Operation::REGISTER), mSuccess(success), mPort(port),
      mEmergencyData(emergencyData) {}

bool RegisterResponse::success() const { return mSuccess; }

quint16 RegisterResponse::port() const { return mPort; }

QStringList RegisterResponse::emergencyData() const { return mEmergencyData; }

void RegisterResponse::read(const QJsonObject &jsonObject) {
    Base::read(jsonObject);
    mSuccess = jsonObject["SUCCESS"].toBool();
    mPort = static_cast<quint16>(jsonObject["PORT"].toInt());
    auto jsonArray = jsonObject["EMERGENCY_DATA"].toArray();
    for (const auto temp : jsonArray.toVariantList()) {
        mEmergencyData << temp.toString();
    }
}

void RegisterResponse::write(QJsonObject &jsonObject) const {
    Base::write(jsonObject);
    jsonObject["SUCCESS"] = mSuccess;
    jsonObject["PORT"] = mPort;
    jsonObject["EMERGENCY_DATA"] = QJsonArray::fromStringList(mEmergencyData);
}

void RegisterResponse::setEmergencyData(const QStringList &emergencyData) {
    mEmergencyData = emergencyData;
}

void RegisterResponse::setPort(const quint16 &port) { mPort = port; }

void RegisterResponse::setSuccess(bool success) { mSuccess = success; }
}
}
