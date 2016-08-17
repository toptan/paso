#include "listdetailsdialog.h"
#include "ui_listdetailsdialog.h"

#include "pasodb.h"

#include <QDebug>
#include <QMessageBox>
#include <QPushButton>
#include <QSqlError>

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
    //    return true;
}

void ListDetailsDialog::importListStudents() {}

void ListDetailsDialog::onImportFileSelected(const QString &fileName) {}
}
}
