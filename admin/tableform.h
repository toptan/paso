#ifndef TABLEFORM_H
#define TABLEFORM_H

#include "abstractform.h"

#include <utility>

class QSqlTableModel;

namespace paso {
namespace admin {

class TableForm : public AbstractForm {
    Q_OBJECT
public:
    explicit TableForm(
        std::pair<QSqlTableModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);
    virtual ~TableForm();

protected slots:

protected:
    virtual bool removeRow(int row, QSqlError &error) override;
    virtual bool insertRecord(const QSqlRecord &record,
                              QSqlError &error) override;
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) override;

private:
    QSqlTableModel *tableModel() const;
};
}
}

#endif // TABLEFORM_H
