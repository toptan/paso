#include "listdetailsdialog.h"
#include "ui_listdetailsdialog.h"

#include "logdialog.h"
#include "pasodb.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>
#include <QtConcurrent>

using namespace std;
using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;

namespace paso {
namespace admin {

struct ListDetailsDialog::ListDetailsDialogPrivate {
    explicit ListDetailsDialogPrivate(const List &list)
        : list(list), manager(DEFAULT_DB_NAME) {}

    const List list;

    DBManager manager;
};

ListDetailsDialog::ListDetailsDialog(const List &list, QWidget *parent)
    : QDialog(parent), ui(new Ui::ListDetailsDialog),
      mPrivate(new ListDetailsDialogPrivate(list)) {
    ui->setupUi(this);
    if (list.system()) {
        ui->typeLabel->setText(tr("System list:"));
        ui->addRemoveEntitiesForm->setReadOnly();
    } else if (list.permanent()) {
        ui->typeLabel->setText(tr("Permanent list:"));
    } else {
        ui->typeLabel->setText(tr("List:"));
    }
    ui->nameLabel->setText(list.name());
    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));
    auto refreshButton = new QPushButton(tr("Refresh"), ui->buttonBox);
    refreshButton->setObjectName("REFRESH_BUTTON");
    ui->buttonBox->addButton(refreshButton, QDialogButtonBox::ResetRole);
    if (!list.system()) {
        auto importButton =
            new QPushButton(tr("Import Students"), ui->buttonBox);
        importButton->setObjectName("IMPORT_BUTTON");
        ui->buttonBox->addButton(importButton,
                                 QDialogButtonBox::DestructiveRole);
        ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("Save"));
    } else {
        ui->buttonBox->removeButton(
            ui->buttonBox->button(QDialogButtonBox::Save));
    }
    connect(ui->buttonBox, &QDialogButtonBox::clicked, this,
            &ListDetailsDialog::onButtonBoxButtonClicked);
    loadData();
}

ListDetailsDialog::~ListDetailsDialog() { delete ui; }

void ListDetailsDialog::reject() {
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

void ListDetailsDialog::onButtonBoxButtonClicked(QAbstractButton *button) {
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
        importListStudents();
    }
}

void ListDetailsDialog::refresh() {
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

bool ListDetailsDialog::loadData() {
    QSqlError mErr;
    QSqlError nErr;
    auto members =
        mPrivate->manager.membersOfTheList(mPrivate->list.id(), mErr);
    auto nonMembers =
        mPrivate->manager.nonMembersOfTheList(mPrivate->list.id(), nErr);
    if (mErr.type() != QSqlError::NoError ||
        nErr.type() != QSqlError::NoError) {
        auto t = mErr.type() != QSqlError::NoError ? mErr.text() : nErr.text();
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
        QString(tr("Members: %1")).arg(members.size()));
    ui->addRemoveEntitiesForm->setData(tr("Non members"), columns, columnNames,
                                       nonMembers, tr("Members"), columns,
                                       columnNames, members);
    return true;
}

bool ListDetailsDialog::saveData() {
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
    auto retVal = mPrivate->manager.updateListStudents(
        mPrivate->list.id(), addedIndexes, removedIndexes, error);
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

void ListDetailsDialog::importListStudents() {
    QMessageBox msgBox(this);
    msgBox.setText(tr("Importing list members will overwrite current data."));
    msgBox.setInformativeText(tr("Do you still want to import list members?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if (msgBox.exec() != QMessageBox::Yes) {
        return;
    }

    auto dialog =
        new QFileDialog(this, tr("Open list members import file"), "", "*.csv");
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);
    dialog->setOption(QFileDialog::ReadOnly, true);
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setModal(true);
    connect(dialog, &QFileDialog::fileSelected, this,
            &ListDetailsDialog::onImportFileSelected);
    connect(dialog, &QFileDialog::rejected, dialog, &QObject::deleteLater);
    connect(dialog, &QFileDialog::accepted, dialog, &QObject::deleteLater);
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
}

void ListDetailsDialog::onImportFileSelected(const QString &fileName) {
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

    auto logDialog = new LogDialog(tr("Importing list members..."));
    logDialog->setModal(true);
    logDialog->show();
    connect(logDialog, &LogDialog::accepted, logDialog,
            &LogDialog::deleteLater);
    connect(logDialog, &LogDialog::rejected, logDialog,
            &LogDialog::deleteLater);
    connect(this, &ListDetailsDialog::newLogLine, logDialog,
            &LogDialog::appendLine);
    connect(this, &ListDetailsDialog::importDone, logDialog,
            &LogDialog::processingDone);

    auto work = [this, file, logDialog]() {
        QTextStream in(file);
        bool errorOccured = false;
        int lineNo = 1;
        DBManager manager;
        QSqlError sqlError;
        QString format(tr("Importing line%1... %2"));
        manager.beginTransaction();
        if (!manager.removeAllStudentsFromList(mPrivate->list.id(), sqlError)) {
            QString message(tr("Clearing old data has failed... %1"));
            emit newLogLine(message.arg(sqlError.text()));
            emit newLogLine("");
            emit newLogLine(tr("Importing list members has failed."));
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
            auto error =
                manager.importListStudent(mPrivate->list.id(), line, sqlError);
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
            loadData();
            manager.commit();
        } else {
            emit newLogLine(tr("Not all lines could be imported. Please see "
                               "log messages above."));
            emit newLogLine(tr("Importing list members has failed."));
            emit newLogLine(tr("Data remains unchanged."));
            manager.rollback();
        }
        emit importDone();
    };

    QtConcurrent::run(work);
}
}
}
