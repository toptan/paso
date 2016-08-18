#include "studentdetailsdialog.h"
#include "ui_studentdetailsdialog.h"

#include "data.h"
#include "pasodb.h"

#include <QPushButton>

using namespace std;
using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;

namespace paso {
namespace admin {

struct StudentDetailsDialog::StudentDetailsDialogPrivate {
    explicit StudentDetailsDialogPrivate(const Student &student)
        : student(student), manager(DEFAULT_DB_NAME) {}

    const Student student;

    DBManager manager;
};

StudentDetailsDialog::StudentDetailsDialog(const data::entity::Student &student,
                                           QWidget *parent)
    : QDialog(parent), ui(new Ui::StudentDetailsDialog),
      mPrivate(new StudentDetailsDialogPrivate(student)) {
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
    connect(ui->buttonBox->button(QDialogButtonBox::Close),
            &QPushButton::clicked, this, &StudentDetailsDialog::reject);
    ui->indexNumber->setText(student.indexNumber());
    ui->firstName->setText(student.firstName());
    ui->lastName->setText(student.lastName());
    ui->email->setText(student.email());
    ui->yearOfStudy->setText(QString("%1").arg(student.yearOfStudy()));
    ui->rfid->setText(student.rfid());
}

StudentDetailsDialog::~StudentDetailsDialog() { delete ui; }
}
}
