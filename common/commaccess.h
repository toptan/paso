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
    ///
    /// \brief Creates empty access query request.
    ///
    AccessRequest();

    ///
    /// \brief Creates access query request for given room and rfid.
    /// \param roomId The room UUID.
    /// \param rfid The card RFID.
    ///
    explicit AccessRequest(const QUuid &roomId, const QString &rfid = "");

    ///
    /// \brief Returns card RFID.
    /// \return The card RFID.
    ///
    QString rfid() const;

    ///
    /// \brief Sets the card RFID.
    /// \param rfid The card RFID.
    ///
    void setRfid(const QString &rfid);

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
    QString mRfid; //!< The card RFID for which access query is.
};

///
/// \brief The AccessResponse class encapsulates room access query responses.
///
class AccessResponse : public Base {
public:
    ///
    /// \brief Constructs empty access query response.
    ///
    AccessResponse();

    ///
    /// \brief Constructs access query response for given room and grant.
    /// \param roomId The room UUID.
    /// \param granted \c true if access was granted.
    ///
    explicit AccessResponse(const QUuid &roomId, bool granted = false);

    ///
    /// \brief Returns whether access was granted.
    /// \return \c true if access was granted.
    ///
    bool granted() const;

    ///
    /// \brief Sets whether access was granted.
    /// \param granted \c true if access was granted.
    ///
    void setGranted(bool granted);

    ///
    /// \brief Returns whether controller should reregister itself with the
    /// server.
    /// \return \c true if controller should reregister.
    ///
    bool reRegister() const;

    ///
    /// \brief Sets whether controller should reregister itself with the server.
    /// \param reRegister \c true if controller should reregister.
    ///
    void setReRegister(bool reRegister);

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
    bool mGranted;    //!< Whether access was granted.
    bool mReRegister; //!< Whether controller needs to reregister.
};
}
}

#endif // COMMACCESS_H
