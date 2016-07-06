#ifndef QUERYFORM_H
#define QUERYFORM_H

#include "abstractform.h"

#include <utility>

class QSqlQueryModel;

namespace paso {
namespace admin {

class QueryForm : public AbstractForm {
public:
    explicit QueryForm(
        std::pair<AbstractQueryModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);
    virtual ~QueryForm();

protected:
    virtual bool removeRow(int row, QSqlError &error) override;
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) override;
};
}
}
#endif // QUERYFORM_H
