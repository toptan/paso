#ifndef STUDENTFORM_H
#define STUDENTFORM_H

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

signals:
    void newLogLine(const QString &line);
    void importDone();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual bool insertRecord(QSqlRecord &record, QSqlError &error) override;
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) override;

    virtual bool removeRow(int row, QSqlError &error) override;

private slots:
    void onImport();

private:
    Ui::StudentForm *ui;
    QAction *mImportAction;

    static std::pair<QSqlQueryModel *, RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // STUDENTFORM_H
