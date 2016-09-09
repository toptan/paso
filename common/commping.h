#ifndef COMMPING_H
#define COMMPING_H

#include "commdata.h"

#include <QStringList>

namespace paso {
namespace comm {

///
/// \brief The PingRequest class encapsulates communication check requests.
///
class PingRequest : public Base {
public:
    PingRequest();

    explicit PingRequest(const QUuid &roomId,
                         const QStringList &emergencyData = {});

    QStringList emergencyData() const;

    void setEmergencyData(const QStringList &emergencyData);

    virtual void read(const QJsonObject &jsonObject) override;

    virtual void write(QJsonObject &jsonObject) const override;

private:
    QStringList mEmergencyData;
};

///
/// \brief The PingResponse class encapsulates communication check response.
///
class PingResponse : public Base {
public:
    PingResponse();

    explicit PingResponse(const QUuid &roomId, bool response = true,
                          const QString &fault = "");

    bool response() const;

    void setResponse(bool response);

    QString fault() const;

    void setFault(const QString &fault);

    virtual void read(const QJsonObject &jsonObject) override;

    virtual void write(QJsonObject &jsonObject) const override;

private:
    bool mResponse;
    QString mFault;
};
}
}

#endif // COMMPING_H
