#ifndef ACTIVITYFORM_H
#define ACTIVITYFORM_H

#include "activityeditorwidget.h"
#include "queryform.h"

namespace Ui {
class ActivityForm;
}

namespace paso {
namespace admin {

class ActivityForm : public widget::QueryForm {
    Q_OBJECT

public:
    explicit ActivityForm(QWidget *parent = 0);
    virtual ~ActivityForm();

protected slots:
    virtual void onNewRecord() override;
    virtual void onEditRecord() override;

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual void updateActions(const QSqlRecord &record) override;

    virtual bool insertRecord(QSqlRecord &record, QSqlError &error) override;
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) override;

    virtual bool removeRow(int row, QSqlError &error) override;

private:
    Ui::ActivityForm *ui;

    static std::pair<QSqlQueryModel *, widget::RecordEditorWidget *>
    createModelAndEditor();
};
}
}

#endif // ACTIVITYFORM_H
