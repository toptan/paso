#ifndef QUERYFORM_H
#define QUERYFORM_H

#include "abstractform.h"
#include "pasodb.h"

#include <utility>

class QSqlQueryModel;

namespace paso {
namespace admin {

class QueryForm : public AbstractForm {
public:
    explicit QueryForm(
        std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);
    virtual ~QueryForm();

protected:
    db::DBManager &manager();

    virtual bool removeRow(int row, QSqlError &error) override;

private:
    db::DBManager mManager; //!< The dabase manager to use.
};
}
}
#endif // QUERYFORM_H
