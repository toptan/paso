#ifndef SYSTEMUSERVALIDATOR_H
#define SYSTEMUSERVALIDATOR_H

#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {
class SystemUserValidator : public RecordValidator {
    Q_OBJECT
public:
    SystemUserValidator(const FieldTypes &fieldTypes,
                        const FieldEditors &fieldEditors,
                        QObject *parent = nullptr);
    virtual ~SystemUserValidator() {}

    virtual bool validate() const override;
};
}
}
#endif // SYSTEMUSERVALIDATOR_H
