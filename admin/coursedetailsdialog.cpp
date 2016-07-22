#include "coursedetailsdialog.h"
#include "ui_coursedetailsdialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
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
        : course(course), manager(DEFAULT_DB_NAME) {}

    const Course course;

    DBManager manager;
};

CourseDetailsDialog::CourseDetailsDialog(const Course &course, QWidget *parent)
    : QDialog(parent), ui(new Ui::CourseDetailsDialog),
      mPrivate(new CourseDetailsDialogPrivate(course)) {
    ui->setupUi(this);
    ui->codeLabel->setText(mPrivate->course.code());
    ui->nameLabel->setText(mPrivate->course.name());
    ui->buttonBox->buttons();
    ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("Save"));
    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
    ui->buttonBox->addButton(new QPushButton(tr("Refresh"), ui->buttonBox),
                             QDialogButtonBox::DestructiveRole);
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this,
            &CourseDetailsDialog::onButtonBoxButtonClicked);
}

CourseDetailsDialog::~CourseDetailsDialog() { delete ui; }

int CourseDetailsDialog::exec() {
    return loadData() ? QDialog::exec() : QDialog::Rejected;
}

void CourseDetailsDialog::reject() {
    if (!ui->addRemoveEntitiesForm->dirty()) {
        QDialog::reject();
        return;
    }
    QMessageBox msgBox(this);
    msgBox.setWindowModality(Qt::WindowModal);
    msgBox.setWindowTitle(tr("Unsaved changes"));
    msgBox.setText(tr("You have changes that are not saved."));
    msgBox.setInformativeText(tr("Do you want to save them?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
    msgBox.setButtonText(QMessageBox::No, tr("No"));
    if (msgBox.exec() == QMessageBox::No) {
        QDialog::reject();
        return;
    }
    if (saveData()) {
        QDialog::accept();
    } else {
        QDialog::reject();
    }
}

void CourseDetailsDialog::accept() { saveData(); }

void CourseDetailsDialog::onButtonBoxButtonClicked(QAbstractButton *button) {
    switch (ui->buttonBox->buttonRole(button)) {
    case QDialogButtonBox::AcceptRole:
        if (ui->addRemoveEntitiesForm->dirty()) {
            saveData();
            loadData();
        }
        break;
    case QDialogButtonBox::RejectRole:
        reject();
        break;
    case QDialogButtonBox::DestructiveRole:
        refresh();
        break;
    default:
        break;
    }
}

void CourseDetailsDialog::refresh() {
    if (ui->addRemoveEntitiesForm->dirty()) {
        QMessageBox msgBox(this);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setWindowTitle(tr("Unsaved changes"));
        msgBox.setText(tr("You have changes that are not saved. If you reload "
                          "data all unsaved changes will be lost."));
        msgBox.setInformativeText(tr("Do you still want to reload data?"));
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        msgBox.setButtonText(QMessageBox::Yes, tr("Yes"));
        msgBox.setButtonText(QMessageBox::No, tr("No"));
        if (msgBox.exec() == QMessageBox::No) {
            return;
        }
    }
    loadData();
}

bool CourseDetailsDialog::loadData() {
    QSqlError error;
    auto enlistedStudents = mPrivate->manager.studentsEnlistedToCourse(
        mPrivate->course.code(), error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return false;
    }
    auto notEnlistedStudents = mPrivate->manager.studentsNotEnlistedToCourse(
        mPrivate->course.code(), error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return false;
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
    return true;
}

bool CourseDetailsDialog::saveData() {
    auto added = ui->addRemoveEntitiesForm->addedEntities();
    auto removed = ui->addRemoveEntitiesForm->removedEntities();

    QStringList addedIndexes;
    QStringList removedIndexes;

    for (const auto &entity : added) {
        addedIndexes.push_back(entity->value("INDEX_NUMBER").toString());
    }
    for (const auto &entity : removed) {
        removedIndexes.push_back(entity->value("INDEX_NUMBER").toString());
    }
    QSqlError error;
    auto retVal = mPrivate->manager.updateCourseStudents(
        mPrivate->course.code(), addedIndexes, removedIndexes, error);
    if (error.type() != QSqlError::NoError) {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return false;
    }

    return true;
}
}
}
