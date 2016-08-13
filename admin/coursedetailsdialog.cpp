#include "coursedetailsdialog.h"
#include "ui_coursedetailsdialog.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QFileDialog>
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
    auto refreshButton = new QPushButton(tr("Refresh"), ui->buttonBox);
    refreshButton->setObjectName("REFRESH_BUTTON");
    ui->buttonBox->addButton(refreshButton, QDialogButtonBox::ResetRole);
    auto importButton = new QPushButton(tr("Import Students"), ui->buttonBox);
    importButton->setObjectName("IMPORT_BUTTON");
    ui->buttonBox->addButton(importButton, QDialogButtonBox::DestructiveRole);

    connect(ui->buttonBox, &QDialogButtonBox::clicked, this,
            &CourseDetailsDialog::onButtonBoxButtonClicked);
    loadData();
}

CourseDetailsDialog::~CourseDetailsDialog() { delete ui; }

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
    }
}

void CourseDetailsDialog::onButtonBoxButtonClicked(QAbstractButton *button) {
    auto role = ui->buttonBox->buttonRole(button);
    if (role == QDialogButtonBox::AcceptRole &&
        ui->addRemoveEntitiesForm->dirty()) {
        if (!saveData()) {
            return;
        }
        loadData();
    } else if (role == QDialogButtonBox::RejectRole) {
        reject();
    } else if (role == QDialogButtonBox::ResetRole) {
        refresh();
    } else if (role == QDialogButtonBox::DestructiveRole) {
        importCourseStudents();
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
    QSqlError eErr;
    QSqlError nErr;
    auto enlistedStudents = mPrivate->manager.studentsEnlistedToCourse(
        mPrivate->course.code(), eErr);
    auto notEnlistedStudents = mPrivate->manager.studentsNotEnlistedToCourse(
        mPrivate->course.code(), nErr);

    if (eErr.type() != QSqlError::NoError ||
        nErr.type() != QSqlError::NoError) {
        auto t = eErr.type() != QSqlError::NoError ? eErr.text() : nErr.text();
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setWindowTitle(tr("Critical error"));
        msgBox.setText(tr("There was an error working with the database."));
        msgBox.setDetailedText(t);
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

    return retVal;
}

void CourseDetailsDialog::importCourseStudents() {
    QMessageBox msgBox(this);
    msgBox.setText(tr("Importing course students will overwire currend data."));
    msgBox.setInformativeText(
        tr("Do you still want to import course stundets?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if (msgBox.exec() != QMessageBox::Yes) {
        return;
    }

    auto dialog = new QFileDialog(this, tr("Open course students import file"),
                                  "", "*.csv");
    dialog->setOption(QFileDialog::DontUseNativeDialog, this);
    dialog->setOption(QFileDialog::ReadOnly, true);
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setModal(true);
    connect(dialog, &QFileDialog::fileSelected, this,
            &CourseDetailsDialog::onImportFileSelected);
    connect(dialog, &QFileDialog::rejected, dialog, &QObject::deleteLater);
    connect(dialog, &QFileDialog::accepted, dialog, &QObject::deleteLater);
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
}

void CourseDetailsDialog::onImportFileSelected(const QString &fileName) {
    qDebug() << fileName;
}
}
}
