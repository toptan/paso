#include "roomvalidator.h"

#include <QLineEdit>
#include <QVariant>
#include <QSqlError>
#include <QMessageBox>

namespace paso {
namespace admin {

using namespace std;

RoomValidator::RoomValidator(const FieldTypes &fieldTypes,
                             const FieldEditors &fieldEditors, QObject *parent)
    : RecordValidator(fieldTypes, fieldEditors, parent), dbManager() {}

shared_ptr<ValidationError>
RoomValidator::validate(const QSqlRecord &original) const {
    auto retVal = validateRoomUuid(original.value("room_uuid").toString());

    if (retVal) {
        return retVal;
    }

    retVal = validateName();
    if (retVal) {
        return retVal;
    }

    return validateRoomNumber(original.value("room_number").toString());
}

shared_ptr<ValidationError>
RoomValidator::validateRoomUuid(const QString &original) const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["room_uuid"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("Room UUID has to be provided."));
    }

    if (original.isEmpty() || text != original) {
        QSqlError error;
        auto unique = dbManager.roomUuidUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            return make_shared<ValidationError>(
                nullptr, tr("Critical error"),
                tr("There was an error working with the database."),
                error.text(), QMessageBox::Critical);
        }
        if (!unique) {
            return make_shared<ValidationError>(
                editor, tr("Invalid data entered"),
                tr("The room UUID you entered is not unique."));
        }
    }
    return nullptr;
}

shared_ptr<ValidationError> RoomValidator::validateName() const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["name"]);
    auto text = editor->text().trimmed();
    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The name of the room cannot be left emtpy."));
    }
    if (text.size() > 64) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The name of the room cannot exceed 64 characters."));
    }
    return nullptr;
}

shared_ptr<ValidationError>
RoomValidator::validateRoomNumber(const QString &original) const {
    auto editor = dynamic_cast<QLineEdit *>(mFieldEditors["room_number"]);
    auto text = editor->text().trimmed();

    if (text.isEmpty()) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The room number has to be provided."));
    }

    if (text.size() > 8) {
        return make_shared<ValidationError>(
            editor, tr("Invalid data entered"),
            tr("The room number cannot exceed 8 characters."));
    }

    if (original.isEmpty() || text != original) {
        QSqlError error;
        auto unique = dbManager.roomNumberUnique(text, error);
        if (error.type() != QSqlError::NoError) {
            return make_shared<ValidationError>(
                nullptr, tr("Critical error"),
                tr("There was an error working with the database."),
                error.text(), QMessageBox::Critical);
        }
        if (!unique) {
            return make_shared<ValidationError>(
                editor, tr("Invalid data entered"),
                tr("The room with entered number already exists in the "
                   "system."));
        }
    }
    return nullptr;
}
}
}
