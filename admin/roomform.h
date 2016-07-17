#ifndef ROOMFORM_H
#define ROOMFORM_H

#include "tableform.h"
#include "roomeditorwidget.h"
#include "roomtablemodel.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class RoomForm;
}

namespace paso {
namespace admin {

class RoomForm : public TableForm {
    Q_OBJECT

public:
    explicit RoomForm(QWidget *parent = nullptr);
    virtual ~RoomForm();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    virtual void updateActions(const QSqlRecord &record) override;

private:
    Ui::RoomForm *ui;

    static std::pair<QSqlTableModel *, RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // ROOMFORM_H
