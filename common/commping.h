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
    ///
    /// \brief Constructs empty ping request.
    ///
    PingRequest();

    ///
    /// \brief Constructs ping request for given room controller.
    /// \param roomId The room UUID.
    /// \param emergencyData The emergency data to use.
    ///
    explicit PingRequest(const QUuid &roomId,
                         const QStringList &emergencyData = {});

    ///
    /// \brief Returns emergency data the controller should use.
    /// \return The emergency data.
    ///
    QStringList emergencyData() const;

    ///
    /// \brief Sets emergency data that controller should use.
    /// \param emergencyData The emergency data.
    ///
    void setEmergencyData(const QStringList &emergencyData);

    ///
    /// \brief This method reads data from given JSON object and populates
    /// this object's properties.
    /// \param jsonObject A JSON object to read data from.
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \brief This method writes data from this object to the given JSON
    /// object.
    /// \param jsonObject A JSON object to write data to.
    ///
    virtual void write(QJsonObject &jsonObject) const override;

private:
    QStringList mEmergencyData; //!< The emergency data for controller to use.
};

///
/// \brief The PingResponse class encapsulates communication check response.
///
class PingResponse : public Base {
public:
    ///
    /// \brief Constructs empty ping response.
    ///
    PingResponse();

    ///
    /// \brief Constructs ping response for given room controller.
    /// \param roomId The room UUID.
    /// \param response The controller response. \c true means all OK.
    /// \param fault The fault cause if any.
    ///
    explicit PingResponse(const QUuid &roomId, bool response = true,
                          const QString &fault = "");

    ///
    /// \brief Returns the response of the controller. \c true means all OK.
    /// \return The controller's response.
    ///
    bool response() const;

    ///
    /// \brief Sets controller's response. \c true means all OK.
    /// \param response The controller's response.
    ///
    void setResponse(bool response);

    ///
    /// \brief Returns the fault cause if any.
    /// \return The fault cause.
    ///
    QString fault() const;

    ///
    /// \brief Sets the fault cause. Empty string means no or unknown fault
    /// cause.
    /// \param fault The fauld cause.
    ///
    void setFault(const QString &fault);

    ///
    /// \brief This method reads data from given JSON object and populates
    /// this object's properties.
    /// \param jsonObject A JSON object to read data from.
    ///
    virtual void read(const QJsonObject &jsonObject) override;

    ///
    /// \brief This method writes data from this object to the given JSON
    /// object.
    /// \param jsonObject A JSON object to write data to.
    ///
    virtual void write(QJsonObject &jsonObject) const override;

private:
    bool mResponse; //!< The controllers response. \c true means all OK.
    QString mFault; //!< The fault cause.
};
}
}

#endif // COMMPING_H
