#ifndef ADMINISTRATORFORM_H
#define ADMINISTRATORFORM_H

#include "abstractform.h"
#include "systemusertablemodel.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class AdministratorForm;
}

namespace paso {
namespace admin {

class AdministratorForm : public AbstractForm {
    Q_OBJECT

public:
    explicit AdministratorForm(QWidget *parent = 0);
    virtual ~AdministratorForm();

    virtual const QList<QAction *> &toolBarActions() const override;

private slots:
    void editNew();
    void editSelected();
    void editFinished();
    void deleteUser();

    void onRequestUpdate(QSqlRecord record);
    void onRequestSave(QSqlRecord record);
    void onSelectionChanged(const QSqlRecord &record);

private:
    Ui::AdministratorForm *ui;
    SystemUserTableModel *mModel;
    QAction *mNewUserAction;
    QAction *mEditUserAction;
    QAction *mDeleteUserAction;
    QAction *mRefreshAction;
    QList<QAction *> mActions;
};
}
}

#endif // ADMINISTRATORFORM_H
