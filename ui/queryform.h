#ifndef QUERYFORM_H
#define QUERYFORM_H

#include "abstractform.h"
#include "pasodb.h"

#include <utility>

class QSqlQueryModel;

namespace paso {
namespace widget {

class QueryForm : public AbstractForm {
public:
    explicit QueryForm(
        std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);
    virtual ~QueryForm();
};
}
}
#endif // QUERYFORM_H
