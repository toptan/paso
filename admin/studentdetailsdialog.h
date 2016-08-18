#ifndef STUDENTDETAILSDIALOG_H
#define STUDENTDETAILSDIALOG_H

#include "student.h"
#include <QDialog>

namespace Ui {
class StudentDetailsDialog;
}

namespace paso {
namespace admin {

class StudentDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit StudentDetailsDialog(const paso::data::entity::Student &student,
                                  QWidget *parent = 0);
    virtual ~StudentDetailsDialog();

private:
    Ui::StudentDetailsDialog *ui;

    struct StudentDetailsDialogPrivate;
    std::unique_ptr<StudentDetailsDialogPrivate> mPrivate;
};
}
}
#endif // STUDENTDETAILSDIALOG_H
