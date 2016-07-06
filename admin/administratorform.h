#ifndef ADMINISTRATORFORM_H
#define ADMINISTRATORFORM_H

#include "tableform.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

class QSqlTableModel;

namespace Ui {
class AdministratorForm;
}

namespace paso {
namespace admin {

class AdministratorForm : public TableForm {
    Q_OBJECT

public:
    explicit AdministratorForm(QWidget *parent = nullptr);
    virtual ~AdministratorForm();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

private:
    Ui::AdministratorForm *ui;

    static std::pair<AbstractQueryModel *, RecordEditorWidget *>
    createModelAndEditor();
};
}
}

#endif // ADMINISTRATORFORM_H
