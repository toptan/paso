#ifndef ACTIVITYOVERLAPSDIALOG_H
#define ACTIVITYOVERLAPSDIALOG_H

#include "activity.h"

#include <QDialog>

namespace Ui {
class ActivityOverlapsDialog;
}

namespace paso {
namespace admin {
class ActivityOverlapsDialog : public QDialog {
    Q_OBJECT

public:
    explicit ActivityOverlapsDialog(
        const paso::data::entity::Activity &activity, QWidget *parent = 0);
    virtual ~ActivityOverlapsDialog();

private:
    Ui::ActivityOverlapsDialog *ui;
};
}
}

#endif // ACTIVITYOVERLAPSDIALOG_H
