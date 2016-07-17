#ifndef COURSEDETAILSDIALOG_H
#define COURSEDETAILSDIALOG_H

#include "pasodb.h"
#include "course.h"
#include "student.h"

#include <QDialog>

namespace Ui {
class CourseDetailsDialog;
}

namespace paso {
namespace admin {

class CourseDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit CourseDetailsDialog(const paso::data::entity::Course &course,
                                 QWidget *parent = 0);
    ~CourseDetailsDialog();

private:
    Ui::CourseDetailsDialog *ui;
    const paso::data::entity::Course mCourse;
};
}
}
#endif // COURSEDETAILSDIALOG_H
