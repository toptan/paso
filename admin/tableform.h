#ifndef TABLEFORM_H
#define TABLEFORM_H

#include "abstractform.h"

#include <utility>

class AbstractQueryModel;

namespace paso {
namespace admin {

class TableForm : public AbstractForm {
    Q_OBJECT
public:
    explicit TableForm(
        std::pair<AbstractQueryModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);
    virtual ~TableForm();

protected:
    virtual bool removeRow(int row, QSqlError &error) override;
    virtual bool insertRecord(const QSqlRecord &record,
                              QSqlError &error) override;
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) override;
};
}
}

#endif // TABLEFORM_H
