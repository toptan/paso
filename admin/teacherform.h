#ifndef TEACHERFORM_H
#define TEACHERFORM_H

#include "queryform.h"
#include "teachereditorwidget.h"
#include "teachervalidator.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class TeacherForm;
}

namespace paso {
namespace admin {

class TeacherForm : public widget::QueryForm {
    Q_OBJECT

public:
    explicit TeacherForm(QWidget *parent = 0);
    virtual ~TeacherForm();

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
    Ui::TeacherForm *ui;

    static std::pair<QSqlQueryModel *, widget::RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // TEACHERFORM_H
