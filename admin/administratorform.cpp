#include "administratorform.h"
#include "ui_administratorform.h"

#include "data.h"
#include "pasodb.h"
#include "recordvalidator.h"
#include "systemuservalidator.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlField>
#include <QSqlTableModel>

using namespace paso::db;
using namespace paso::data;

namespace paso {
namespace admin {

AdministratorForm::AdministratorForm(QWidget *parent)
    : AbstractForm(parent), ui(new Ui::AdministratorForm) {
    ui->setupUi(this);
    mModel =
        new SystemUserTableModel(this, QSqlDatabase::database(DEFAULT_DB_NAME));
    ui->tableView->setModel(mModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    QMap<QString, FieldType> fieldTypes;
    fieldTypes["username"] = FieldType::LineEdit;
    fieldTypes["password"] = FieldType::PasswordEdit;
    fieldTypes["first_name"] = FieldType::LineEdit;
    fieldTypes["last_name"] = FieldType::LineEdit;
    fieldTypes["email"] = FieldType::LineEdit;
    fieldTypes["role"] = FieldType::ComboBox;

    ui->recordDetails->setupForRecord(mModel->record(), fieldTypes);
    ui->recordDetails->setValidator(
        new SystemUserValidator(ui->recordDetails->fieldTypes(),
                                ui->recordDetails->fieldEditors(), this));

    ui->recordDetails->clearData();
    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &selected, const QModelIndex &) {
                auto record = mModel->record(selected.row());
                this->onSelectionChanged(record);
            });

    connect(ui->recordDetails, &RecordDisplayWidget::editFinished, this,
            &AdministratorForm::editFinished);
    connect(ui->recordDetails, &RecordDisplayWidget::requestUpdate, this,
            &AdministratorForm::onRequestUpdate);
    connect(ui->recordDetails, &RecordDisplayWidget::requestSave, this,
            &AdministratorForm::onRequestSave);
    mNewUserAction = new QAction(tr("New system user"), this);
    connect(mNewUserAction, &QAction::triggered, this,
            &AdministratorForm::editNew);
    mActions.push_back(mNewUserAction);
    mEditUserAction = new QAction(tr("Edit system user"), this);
    mEditUserAction->setEnabled(false);
    connect(mEditUserAction, &QAction::triggered, this,
            &AdministratorForm::editSelected);
    mActions.push_back(mEditUserAction);
    mDeleteUserAction = new QAction(tr("Delete system user"), this);
    mDeleteUserAction->setEnabled(false);
    connect(mDeleteUserAction, &QAction::triggered, this,
            &AdministratorForm::deleteUser);
    mActions.push_back(mDeleteUserAction);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    mActions.push_back(separator);
    mRefreshAction = new QAction(tr("Refresh data"), this);
    connect(mRefreshAction, &QAction::triggered, mModel,
            &SystemUserTableModel::select);
    connect(mRefreshAction, &QAction::triggered, [this]() {
        ui->tableView->clearSelection();
        mEditUserAction->setEnabled(false);
    });
    connect(mRefreshAction, &QAction::triggered, ui->recordDetails,
            &RecordDisplayWidget::clearData);
    mActions.push_back(mRefreshAction);
}

AdministratorForm::~AdministratorForm() { delete ui; }

const QList<QAction *> &AdministratorForm::toolBarActions() const {
    return mActions;
}

void AdministratorForm::editSelected() {
    if (!ui->tableView->selectionModel()->currentIndex().isValid()) {
        return;
    }
    ui->tableView->setDisabled(true);
    for (auto &action : mActions) {
        action->setEnabled(false);
    }
    ui->recordDetails->onEditExistingRecord(
        mModel->record(ui->tableView->selectionModel()->currentIndex().row()));
}
void AdministratorForm::editNew() {
    ui->tableView->setDisabled(true);
    for (auto &action : mActions) {
        action->setEnabled(false);
    }
    ui->recordDetails->onEditNewRecord(mModel->record());
}

void AdministratorForm::editFinished() {
    for (auto &action : mActions) {
        action->setEnabled(true);
    }
    ui->tableView->setDisabled(false);
    ui->tableView->setFocus();
    auto index = ui->tableView->selectionModel()->currentIndex();
    if (index.isValid()) {
        onSelectionChanged(mModel->record(index.row()));
    }
}

void AdministratorForm::onRequestUpdate(QSqlRecord record) {
    // Ensure that root is always in role SUPER_USER
    if (record.value("username").toString() == "root") {
        record.setValue("role", roleToString(SystemRole::SUPER_USER));
    }
    auto index = ui->tableView->selectionModel()->currentIndex();
    mModel->setRecord(index.row(), record);
    if (!mModel->submitAll()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while saving system user data."));
        msgBox.setDetailedText(mModel->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        mModel->select();
        ui->tableView->selectRow(index.row());
        ui->recordDetails->saveError();
        return;
    }

    ui->recordDetails->saveSuccessfull();
    ui->tableView->clearSelection();
    ui->tableView->selectRow(index.row());
    editFinished();
    onSelectionChanged(
        mModel->record(ui->tableView->selectionModel()->currentIndex().row()));
}

void AdministratorForm::onRequestSave(QSqlRecord record) {
    mModel->insertRecord(-1, record);
    if (!mModel->submitAll()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while saving system user data."));
        msgBox.setDetailedText(mModel->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        mModel->select();
        ui->recordDetails->saveError();
        return;
    }

    ui->recordDetails->saveSuccessfull();
    ui->tableView->clearSelection();
    ui->recordDetails->clearData();
    editFinished();
}

void AdministratorForm::onSelectionChanged(const QSqlRecord &record) {
    if (record.value("username").toString() == "root") {
        mDeleteUserAction->setEnabled(false);
    } else {
        mDeleteUserAction->setEnabled(true);
    }
    if (record.isEmpty()) {
        mEditUserAction->setEnabled(false);
    } else {
        mEditUserAction->setEnabled(true);
    }
    ui->recordDetails->onDisplayRecord(record);
}

void AdministratorForm::deleteUser() {
    if (!ui->tableView->selectionModel()->currentIndex().isValid()) {
        return;
    }
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("Do you want to delete selected system user?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    if (msgBox.exec() == QMessageBox::Rejected) {
        return;
    }
    if (!mModel->removeRow(
            ui->tableView->selectionModel()->currentIndex().row())) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while deleting system user."));
        msgBox.setDetailedText(mModel->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    if (!mModel->submitAll()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while deleting system user."));
        msgBox.setDetailedText(mModel->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    mModel->select();
    ui->recordDetails->clearData();
    mEditUserAction->setEnabled(false);
}
}
}
