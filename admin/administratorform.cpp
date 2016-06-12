#include "administratorform.h"
#include "ui_administratorform.h"

#include "data.h"
#include "pasodb.h"
#include "systemusertablemodel.h"
#include "systemuservalidator.h"
#include "systemusereditorwidget.h"

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
    : AbstractForm(createModelAndEditor(), parent),
      ui(new Ui::AdministratorForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);

    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);

    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &selected, const QModelIndex &) {
                auto record = model()->record(selected.row());
                this->onSelectionChanged(record);
            });

    mEditUserAction = new QAction(tr("Edit system user"), this);
    mEditUserAction->setEnabled(false);
    connect(mEditUserAction, &QAction::triggered, this,
            &AdministratorForm::editSelected);
    toolBarActions().push_back(mEditUserAction);
    mDeleteUserAction = new QAction(tr("Delete system user"), this);
    mDeleteUserAction->setEnabled(false);
    connect(mDeleteUserAction, &QAction::triggered, this,
            &AdministratorForm::deleteUser);
    toolBarActions().push_back(mDeleteUserAction);
    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    toolBarActions().push_back(separator);
    mRefreshAction = new QAction(tr("Refresh data"), this);
    connect(mRefreshAction, &QAction::triggered, model(),
            &SystemUserTableModel::select);
    connect(mRefreshAction, &QAction::triggered, [this]() {
        ui->tableView->clearSelection();
        mEditUserAction->setEnabled(false);
    });
    connect(mRefreshAction, &QAction::triggered, recordEditor(),
            &RecordEditorWidget::clearData);
    toolBarActions().push_back(mRefreshAction);
}

AdministratorForm::~AdministratorForm() { delete ui; }

std::pair<QSqlTableModel *, RecordEditorWidget *>
AdministratorForm::createModelAndEditor() {
    auto model =
        new SystemUserTableModel(QSqlDatabase::database(DEFAULT_DB_NAME));
    FieldTypes fieldTypes = {{"username", FieldType::LineEdit},
                             {"password", FieldType::PasswordEdit},
                             {"first_name", FieldType::LineEdit},
                             {"last_name", FieldType::LineEdit},
                             {"email", FieldType::LineEdit},
                             {"role", FieldType::ComboBox}};

    auto editor = new SystemUserEditorWidget(model->record(), fieldTypes);
    editor->setValidator(new SystemUserValidator(
        editor->fieldTypes(), editor->fieldEditors(), editor));
    editor->clearData();

    return std::make_pair<QSqlTableModel *, RecordEditorWidget *>(model,
                                                                  editor);
}

void AdministratorForm::editSelected() {
    if (!ui->tableView->selectionModel()->currentIndex().isValid()) {
        return;
    }
    ui->tableView->setDisabled(true);
    for (auto &action : toolBarActions()) {
        action->setEnabled(false);
    }
    recordEditor()->onEditExistingRecord(
        model()->record(ui->tableView->selectionModel()->currentIndex().row()));
}

void AdministratorForm::prepareRecordForSaving(QSqlRecord &record) {
    // Ensure that root is always has SUPER_USER role
    if (record.value("username").toString() == "root") {
        record.setValue("role", roleToString(SystemRole::SUPER_USER));
    }
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
    recordEditor()->onDisplayRecord(record);
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
    if (!model()->removeRow(
            ui->tableView->selectionModel()->currentIndex().row())) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while deleting system user."));
        msgBox.setDetailedText(model()->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    if (!model()->submitAll()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while deleting system user."));
        msgBox.setDetailedText(model()->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    model()->select();
    recordEditor()->clearData();
    mEditUserAction->setEnabled(false);
}
}
}
