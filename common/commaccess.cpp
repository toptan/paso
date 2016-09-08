#include "commaccess.h"

namespace paso {
namespace comm {

AccessRequest::AccessRequest()
    : Base(QString("{00000000-0000-0000-0000-000000000000}"),
           Operation::ACCESS),
      mRfid() {}

AccessRequest::AccessRequest(const QUuid &roomId, const QString &rfid)
    : Base(roomId, Operation::ACCESS), mRfid(rfid) {}

QString AccessRequest::rfid() const { return mRfid; }

void AccessRequest::setRfid(const QString &rfid) { mRfid = rfid; }

void AccessRequest::read(const QJsonObject &jsonObject) {
    Base::read(jsonObject);
    mRfid = jsonObject["RFID"].toString();
}

void AccessRequest::write(QJsonObject &jsonObject) const {
    Base::write(jsonObject);
    jsonObject["RFID"] = mRfid;
}

AccessResponse::AccessResponse()
    : Base(QString("{00000000-0000-0000-0000-000000000000}"),
           Operation::ACCESS),
      mGranted(false) {}

AccessResponse::AccessResponse(const QUuid &roomId, bool granted)
    : Base(roomId, Operation::ACCESS), mGranted(granted) {}

bool AccessResponse::granted() const { return mGranted; }

void AccessResponse::setGranted(bool granted) { mGranted = granted; }

void AccessResponse::read(const QJsonObject &jsonObject) {
    Base::read(jsonObject);
    mGranted = jsonObject["GRANTED"].toBool();
}

void AccessResponse::write(QJsonObject &jsonObject) const {
    Base::write(jsonObject);
    jsonObject["GRANTED"] = mGranted;
}
}
}
