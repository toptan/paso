#ifndef ADMINISTRATORFORM_H
#define ADMINISTRATORFORM_H

#include "tableform.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

class QSqlTableModel;

namespace Ui {
class SystemUsersForm;
}

namespace paso {
namespace admin {

class SystemUsersForm : public TableForm {
    Q_OBJECT

public:
    explicit SystemUsersForm(QWidget *parent = nullptr);
    virtual ~SystemUsersForm();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual void updateActions(const QSqlRecord &record) override;

private:
    Ui::SystemUsersForm *ui;

    static std::pair<QSqlTableModel *, RecordEditorWidget *>
    createModelAndEditor();
};
}
}

#endif // ADMINISTRATORFORM_H
