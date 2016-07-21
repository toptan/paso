#include "coursedetailsdialog.h"
#include "ui_coursedetailsdialog.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <memory>

using namespace std;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;

namespace paso {
namespace admin {

struct CourseDetailsDialog::CourseDetailsDialogPrivate {
    explicit CourseDetailsDialogPrivate(const Course &course)
        : ENLISTED_COUNT_FORMAT(tr("Enlisted students: %1")), course(course),
          manager(DEFAULT_DB_NAME) {}

    const QString ENLISTED_COUNT_FORMAT;
    const Course course;

    DBManager manager;
};

CourseDetailsDialog::CourseDetailsDialog(const Course &course, QWidget *parent)
    : QDialog(parent), ui(new Ui::CourseDetailsDialog),
      mPrivate(new CourseDetailsDialogPrivate(course)) {
    ui->setupUi(this);
    ui->codeLabel->setText(mPrivate->course.code());
    ui->nameLabel->setText(mPrivate->course.name());
}

CourseDetailsDialog::~CourseDetailsDialog() { delete ui; }

int CourseDetailsDialog::exec() {
    QSqlError error;
    auto enlistedStudents = mPrivate->manager.studentsEnlistedToCourse(
        mPrivate->course.code(), error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox(parentWidget());
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return QDialog::Rejected;
    }
    auto notEnlistedStudents = mPrivate->manager.studentsNotEnlistedToCourse(
        mPrivate->course.code(), error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox(parentWidget());
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return QDialog::Rejected;
    }
    QStringList columns{"INDEX_NUMBER", "LAST_NAME", "FIRST_NAME"};
    QMap<QString, QString> columnNames{{"INDEX_NUMBER", tr("Index Number")},
                                       {"LAST_NAME", tr("Last Name")},
                                       {"FIRST_NAME", tr("First Name")}};

    ui->totalStudentsLabel->setText(
        QString(tr("Enlisted students: %1")).arg(enlistedStudents.size()));
    ui->addRemoveEntitiesForm->setData(
        tr("Not enlisted students"), columns, columnNames, notEnlistedStudents,
        tr("Enlisted students"), columns, columnNames, enlistedStudents);
    return QDialog::exec();
}

void CourseDetailsDialog::reject() {
    qDebug() << __PRETTY_FUNCTION__;
    QDialog::reject();
}

void CourseDetailsDialog::accept() {
    qDebug() << __PRETTY_FUNCTION__;
    QDialog::accept();
}
}
}
