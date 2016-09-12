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
    ///
    /// \brief Constructs empty register request.
    ///
    RegisterRequest();

    ///
    /// \brief Constructs register request for given room.
    /// \param roomId The room UUID.
    ///
    explicit RegisterRequest(const QUuid &roomId);
};

///
/// \brief The RegisterResponse class encapsulates room register responses.
///
class RegisterResponse : public Base {
public:
    ///
    /// \brief Constructs empty register response.
    ///
    RegisterResponse();

    ///
    /// \brief Constructs register response for the room with given data.
    /// \param roomId The room UUID.
    /// \param success Whether registration was successful.
    /// \param port The port where controller should listen for connections.
    /// \param emergencyData The emergency data to use.
    ///
    explicit RegisterResponse(const QUuid &roomId, bool success = false,
                              quint16 port = 0,
                              const QStringList &emergencyData = {});

    ///
    /// \brief Returns whether registration was successful.
    /// \return \c true if registration was successful.
    ///
    bool success() const;

    ///
    /// \brief Sets the status of registration.
    /// \param success The registration status. \c true means success.
    ///
    void setSuccess(bool success);

    ///
    /// \brief Returns the port where on which controller should listen.
    /// \return The port on which controller should listen.
    ///
    quint16 port() const;

    ///
    /// \brief Sets the port on which the controller should listen.
    /// \param port The port on which controller should listen.
    ///
    void setPort(const quint16 &port);

    ///
    /// \brief Sets the emergency data for controller to use.
    /// \param emergencyData The emergency data for controller to use.
    ///
    void setEmergencyData(const QStringList &emergencyData);

    ///
    /// \brief Returns emergency data that controller should use.
    /// \return The emergency data that controller should use.
    ///
    QStringList emergencyData() const;

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
    bool mSuccess;              //!< Whether registration was successful.
    quint16 mPort;              //!< The port on which controller should listen.
    QStringList mEmergencyData; //!< Emergency data for controller to use.
};
}
}

#endif // COMMREGISTER_H
