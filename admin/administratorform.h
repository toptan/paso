#ifndef ADMINISTRATORFORM_H
#define ADMINISTRATORFORM_H

#include "abstractform.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

class QSqlModel;

namespace Ui {
class AdministratorForm;
}

namespace paso {
namespace admin {

class AdministratorForm : public AbstractForm {
    Q_OBJECT

public:
    explicit AdministratorForm(QWidget *parent = nullptr);
    virtual ~AdministratorForm();

protected:
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

private slots:
    void editSelected();
    void deleteUser();

    virtual void onSelectionChanged(const QSqlRecord &record) override;

private:
    Ui::AdministratorForm *ui;
    QAction *mEditUserAction;
    QAction *mDeleteUserAction;
    QAction *mRefreshAction;

    static std::pair<QSqlTableModel *, RecordEditorWidget *>
    createModelAndEditor();
};
}
}

#endif // ADMINISTRATORFORM_H
