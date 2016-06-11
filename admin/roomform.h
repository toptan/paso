#ifndef ROOMFORM_H
#define ROOMFORM_H

#include "abstractform.h"
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
class RoomForm : public AbstractForm {
    Q_OBJECT

public:
    explicit RoomForm(QWidget *parent = nullptr);
    ~RoomForm();

    virtual const QList<QAction *> &toolBarActions() const override;

private slots:

    void onSelectionChanged(const QSqlRecord &record);

private:
    Ui::RoomForm *ui;
    RoomTableModel *mModel;

    QList<QAction *> mActions;
    RoomEditorWidget *mRecordEditor;
};
}
}
#endif // ROOMFORM_H
