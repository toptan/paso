#ifndef COURSEFORM_H
#define COURSEFORM_H

#include "courseeditorwidget.h"
#include "coursetablemodel.h"
#include "tableform.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class CourseForm;
}

namespace paso {
namespace admin {

class CourseForm : public widget::TableForm {
    Q_OBJECT

public:
    explicit CourseForm(QWidget *parent = nullptr);
    virtual ~CourseForm();

public slots:
    void onImportFileSelected(const QString &fileName);

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

    virtual void updateActions(const QSqlRecord &record) override;

private slots:
    void onImport();
    void onDetails();

private:
    Ui::CourseForm *ui;
    QAction *mImportAction;
    QAction *mDetailsAction;

    static std::pair<QSqlTableModel *, widget::RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // COURSEFORM_H
