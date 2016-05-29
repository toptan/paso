#ifndef ADMINISTRATORFORM_H
#define ADMINISTRATORFORM_H

#include <QWidget>
#include <QAction>

#include "systemusertablemodel.h"

namespace Ui {
class AdministratorForm;
}

namespace paso {
namespace admin {

class AdministratorForm : public QWidget {
    Q_OBJECT

public:
    explicit AdministratorForm(QWidget *parent = 0);
    virtual ~AdministratorForm();

    const QList<QAction *>& toolBarActions() const;

private:
    Ui::AdministratorForm *ui;
    SystemUserTableModel *mModel;
    QList<QAction *> mActions;
};
}
}

#endif // ADMINISTRATORFORM_H
