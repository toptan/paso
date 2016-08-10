#ifndef LISTFORM_H
#define LISTFORM_H

#include "listeditorwidget.h"
#include "tableform.h"

namespace Ui {
class ListForm;
}

namespace paso {
namespace admin {
class ListForm : public widget::TableForm {
    Q_OBJECT

public:
    explicit ListForm(QWidget *parent = nullptr);
    virtual ~ListForm();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual void updateActions(const QSqlRecord &record) override;

private slots:
    void onDetails();

private:
    Ui::ListForm *ui;
    QAction *mDetailsAction;

    static std::pair<QSqlTableModel *, widget::RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // LISTFORM_H
