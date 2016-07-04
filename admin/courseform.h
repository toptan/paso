#ifndef COURSEFORM_H
#define COURSEFORM_H

#include "tableform.h"
#include "courseeditorwidget.h"
#include "coursetablemodel.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class CourseForm;
}

namespace paso {
namespace admin {

class CourseForm : public TableForm {
    Q_OBJECT

public:
    explicit CourseForm(QWidget *parent = nullptr);
    virtual ~CourseForm();

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

private slots:
    void onImport();

private:
    Ui::CourseForm *ui;
    QAction *mImportAction;

    static std::pair<QSqlTableModel *, RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // COURSEFORM_H
