#include "commping.h"

#include <QJsonArray>

namespace paso {
namespace comm {

PingRequest::PingRequest()
    : Base(QString("{00000000-0000-0000-0000-000000000000}"), Operation::PING),
      mEmergencyData() {}

PingRequest::PingRequest(const QUuid &roomId, const QStringList &emergencyData)
    : Base(roomId, Operation::PING), mEmergencyData(emergencyData) {}

QStringList PingRequest::emergencyData() const { return mEmergencyData; }

void PingRequest::setEmergencyData(const QStringList &emergencyData) {
    mEmergencyData = emergencyData;
}

void PingRequest::read(const QJsonObject &jsonObject) {
    Base::read(jsonObject);
    auto jsonArray = jsonObject["EMERGENCY_DATA"].toArray();
    for (const auto temp : jsonArray.toVariantList()) {
        mEmergencyData << temp.toString();
    }
}

void PingRequest::write(QJsonObject &jsonObject) const {
    Base::write(jsonObject);
    jsonObject["EMERGENCY_DATA"] = QJsonArray::fromStringList(mEmergencyData);
}

PingResponse::PingResponse()
    : Base(QString("{00000000-0000-0000-0000-000000000000}"), Operation::PING),
      mResponse(true), mFault() {}

PingResponse::PingResponse(const QUuid &roomId, bool response,
                           const QString &fault)
    : Base(roomId, Operation::PING), mResponse(response), mFault(fault) {}

bool PingResponse::response() const { return mResponse; }

void PingResponse::setResponse(bool response) { mResponse = response; }

QString PingResponse::fault() const { return mFault; }

void PingResponse::setFault(const QString &fault) { mFault = fault; }

void PingResponse::read(const QJsonObject &jsonObject) {
    Base::read(jsonObject);
    mResponse = jsonObject["RESPONSE"].toBool();
    mFault = jsonObject["FAULT"].toString();
}

void PingResponse::write(QJsonObject &jsonObject) const {
    Base::write(jsonObject);
    jsonObject["RESPONSE"] = mResponse;
    jsonObject["FAULT"] = mFault;
}
}
}
