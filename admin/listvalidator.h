#ifndef LISTVALIDATOR_H
#define LISTVALIDATOR_H

#include "pasodb.h"
#include "recordvalidator.h"

#include <QObject>

namespace paso {
namespace admin {

class ListValidator : public widget::RecordValidator {
    Q_OBJECT
public:
    ListValidator(const widget::FieldTypes &fieldTypes,
                  const widget::FieldEditors &fieldEditors,
                  QObject *parent = nullptr);
    virtual ~ListValidator();

    virtual std::shared_ptr<widget::ValidationError>
    validate(const QSqlRecord &original) const override;
};
}
}

#endif // LISTVALIDATOR_H
