#ifndef COMMREGISTER_H
#define COMMREGISTER_H

#include "commdata.h"

#include <QStringList>
#include <QUuid>

namespace paso {
namespace comm {

///
/// \brief The RegisterRequest class encapsulates room register requests. It has
/// no properties of its own.
///
class RegisterRequest : public Base {
public:
    RegisterRequest();

    explicit RegisterRequest(const QUuid &roomId);
};

///
/// \brief The RegisterResponse class encapsulates room register responses.
///
class RegisterResponse : public Base {
public:
    RegisterResponse();

    explicit RegisterResponse(const QUuid &roomId, bool success = false,
                              quint16 port = 0,
                              const QStringList &emergencyData = {});

    bool success() const;

    void setSuccess(bool success);

    quint16 port() const;

    void setPort(const quint16 &port);

    void setEmergencyData(const QStringList &emergencyData);

    QStringList emergencyData() const;

    virtual void read(const QJsonObject &jsonObject) override;

    virtual void write(QJsonObject &jsonObject) const override;

private:
    bool mSuccess;
    quint16 mPort;
    QStringList mEmergencyData;
};
}
}

#endif // COMMREGISTER_H
