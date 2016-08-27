#ifndef ACTIVITYFORM_H
#define ACTIVITYFORM_H

#include "activityeditorwidget.h"
#include "tableform.h"

namespace Ui {
class ActivityForm;
}

namespace paso {
namespace admin {

class ActivityForm : public widget::TableForm {
    Q_OBJECT

public:
    explicit ActivityForm(QWidget *parent = 0);
    virtual ~ActivityForm();

protected slots:
    virtual void onNewRecord();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual void updateActions(const QSqlRecord &record) override;

private:
    Ui::ActivityForm *ui;

    static std::pair<QSqlTableModel *, widget::RecordEditorWidget *>
    createModelAndEditor();
};
}
}

#endif // ACTIVITYFORM_H
