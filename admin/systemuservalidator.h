#ifndef SYSTEMUSERVALIDATOR_H
#define SYSTEMUSERVALIDATOR_H

#include "recordvalidator.h"
#include "pasodb.h"

#include <QObject>

namespace paso {
namespace admin {

using namespace paso::db;

class SystemUserValidator : public RecordValidator {
    Q_OBJECT
public:
    SystemUserValidator(const FieldTypes &fieldTypes,
                        const FieldEditors &fieldEditors,
                        QObject *parent = nullptr);
    virtual ~SystemUserValidator() {}

    virtual bool validate(const QSqlRecord& original) const override;

private:
    DBManager dbManager;
    bool validateUsername(const QString& original) const;
    bool validatePassword() const;
    bool validateFirstName() const;
    bool validateLastName() const;
    bool validateEmail() const;
};
}
}
#endif // SYSTEMUSERVALIDATOR_H
