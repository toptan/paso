#ifndef JSONSERIALIZABLE_H
#define JSONSERIALIZABLE_H

#include <QJsonObject>
#include <QString>

namespace paso {
namespace data {

///
/// \brief The JsonSerializable class is a common interface for all objects that
/// need to be serialized to JSON or deserialized from JSON.
///
class JsonSerializable {
public:
    ///
    /// \brief ~JsonSerializable The destructor.
    ///
    virtual ~JsonSerializable();

    ///
    /// \brief read This method reads given JSON string and populates this
    /// object's properties
    /// \param jsonString A JSON string to read from.
    ///
    virtual void fromJsonString(const QString &jsonString) final;

    ///
    /// \brief read This method reads data from given JSON object and populates
    /// this object's properties.
    /// \param jsonObject A JSON object to read data from.
    ///
    virtual void read(const QJsonObject &jsonObject) = 0;

    ///
    /// \brief write This method returns JSON string representation of this
    /// object.
    /// \return A JSON string representation of this object.
    ///
    virtual QString toJsonString() const final;

    ///
    /// \brief write This method writes data from this object to the given JSON
    /// object.
    /// \param jsonObject A JSON object to write data to.
    ///
    virtual void write(QJsonObject &jsonObject) const = 0;
};
}
}

#endif // JSONSERIALIZABLE_H
