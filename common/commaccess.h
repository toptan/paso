#ifndef COMMACCESS_H
#define COMMACCESS_H

#include "commdata.h"

#include <QUuid>

namespace paso {
namespace comm {

///
/// \brief The AccessRequest class encapsulates room access query requests.
///
class AccessRequest : public Base {
public:
    AccessRequest();

    explicit AccessRequest(const QUuid &roomId, const QString &rfid = "");

    QString rfid() const;

    void setRfid(const QString &rfid);

    virtual void read(const QJsonObject &jsonObject) override;

    virtual void write(QJsonObject &jsonObject) const override;

private:
    QString mRfid;
};

///
/// \brief The AccessResponse class encapsulates room access query responses.
///
class AccessResponse : public Base {
public:
    AccessResponse();

    explicit AccessResponse(const QUuid &roomId, bool granted = false);

    bool granted() const;

    void setGranted(bool granted);

    virtual void read(const QJsonObject &jsonObject) override;

    virtual void write(QJsonObject &jsonObject) const override;

private:
    bool mGranted;
};
}
}

#endif // COMMACCESS_H
