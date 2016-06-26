#ifndef COURSEFORM_H
#define COURSEFORM_H

#include "abstractform.h"
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

class CourseForm : public AbstractForm {
    Q_OBJECT

public:
    explicit CourseForm(QWidget *parent = nullptr);
    virtual ~CourseForm();

signals:
    void newLogLine(const QString &line);

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
