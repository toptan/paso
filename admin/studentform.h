#ifndef STUDENTFORM_H
#define STUDENTFORM_H

#include "pasodb.h"
#include "queryform.h"
#include "studenteditorwidget.h"
#include "studentvalidator.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class StudentForm;
}

namespace paso {
namespace admin {

class StudentForm : public QueryForm {
    Q_OBJECT

public:
    explicit StudentForm(QWidget *parent = 0);
    ~StudentForm();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;
    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;
    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;
    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual bool insertRecord(const QSqlRecord &record,
                              QSqlError &error) override;
private slots:
    void onImport();

private:
    Ui::StudentForm *ui;
    QAction *mImportAction;
    paso::db::DBManager m_manager;

    static std::pair<AbstractQueryModel *, RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // STUDENTFORM_H
