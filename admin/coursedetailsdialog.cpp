#include "coursedetailsdialog.h"
#include "ui_coursedetailsdialog.h"

using namespace paso::data::entity;
using namespace paso::db;

namespace paso {
namespace admin {

CourseDetailsDialog::CourseDetailsDialog(const Course &course, QWidget *parent)
    : QDialog(parent), ui(new Ui::CourseDetailsDialog), mCourse(course) {
    ui->setupUi(this);
    ui->codeLabel->setText(mCourse.code());
    ui->nameLabel->setText(mCourse.name());
}

CourseDetailsDialog::~CourseDetailsDialog() { delete ui; }
}
}
