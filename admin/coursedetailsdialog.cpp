#include "coursedetailsdialog.h"
#include "ui_coursedetailsdialog.h"

#include "data.h"
#include "logdialog.h"
#include "pasodb.h"

#include <QDebug>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QThread>
#include <QtConcurrent>
#include <memory>

using namespace std;
using namespace paso::data;
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
    connect(this, &CourseDetailsDialog::importDone, this,
            &CourseDetailsDialog::loadData);
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
    msgBox.setText(
        tr("Importing course students will overwrite current data."));
    msgBox.setInformativeText(
        tr("Do you still want to import course students?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if (msgBox.exec() != QMessageBox::Yes) {
        return;
    }

    auto dialog = new QFileDialog(this, tr("Open course students import file"),
                                  "", "*.csv");
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);
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
    auto file = new QFile(fileName);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox msgBox(this);
        msgBox.setWindowModality(Qt::WindowModal);
        msgBox.setIcon(QMessageBox::Critical);
        QString text =
            QString(tr("The file %1 cannot be opened.")).arg(fileName);
        msgBox.setText(text);
        msgBox.setDetailedText(file->errorString());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        delete file;
        return;
    }

    auto logDialog = new LogDialog(tr("Importing course students..."));
    logDialog->setModal(true);
    logDialog->show();
    connect(logDialog, &LogDialog::accepted, logDialog,
            &LogDialog::deleteLater);
    connect(logDialog, &LogDialog::rejected, logDialog,
            &LogDialog::deleteLater);
    connect(this, &CourseDetailsDialog::newLogLine, logDialog,
            &LogDialog::appendLine);
    connect(this, &CourseDetailsDialog::importDone, logDialog,
            &LogDialog::processingDone);

    auto work = [this, file, logDialog]() {
        QTextStream in(file);
        bool errorOccured = false;
        int lineNo = 1;
        DBManager manager;
        QSqlError sqlError;
        QString format(tr("Importing line%1... %2"));
        manager.beginTransaction();
        if (!manager.removeAllStudentsFromCourse(mPrivate->course.code(),
                                                 sqlError)) {
            QString message(tr("Clearing old data has failed... %1"));
            emit newLogLine(message.arg(sqlError.text()));
            emit newLogLine("");
            emit newLogLine(tr("Importing course students has failed."));
            emit newLogLine(tr("Data remains unchanged."));
            manager.rollback();
            emit importDone();
            delete file;
            return;
        }
        emit newLogLine(tr("Clearing old data... OK."));
        while (!in.atEnd()) {
            QString message;
            QString line = in.readLine();
            auto error = manager.importCourseStudent(mPrivate->course.code(),
                                                     line, sqlError);
            switch (error) {
            case ListStudentImportError::NO_ERROR:
                message = format.arg(lineNo).arg(tr("OK."));
                break;
            case ListStudentImportError::BAD_INDEX_NUMBER:
                message = format.arg(lineNo).arg(
                    tr("Index number is in wrong format or does not exist."));
                errorOccured = true;
                break;
            case ListStudentImportError::NON_EXISTING_STUDENT:
                message = format.arg(lineNo).arg(
                    tr("Student with given index does not exist."));
                errorOccured = true;
                break;
            case ListStudentImportError::DB_ERROR:
                message = format.arg(lineNo).arg(sqlError.text());
                errorOccured = true;
                break;
            }

            emit newLogLine(message);
            lineNo++;
        }
        delete file;
        emit newLogLine("");
        if (!errorOccured) {
            emit newLogLine(tr("Import finished without errors."));
            manager.commit();
        } else {
            emit newLogLine(tr("Not all lines could be imported. Please see "
                               "log messages above."));
            emit newLogLine(tr("Importing course students has failed."));
            emit newLogLine(tr("Data remains unchanged."));
            manager.rollback();
        }
        emit importDone();
    };

    QtConcurrent::run(work);
}
}
}
