#include "administratorform.h"
#include "ui_administratorform.h"

#include "data.h"
#include "pasodb.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlTableModel>

using namespace paso::db;
using namespace paso::data;

namespace paso {
namespace admin {

AdministratorForm::AdministratorForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::AdministratorForm) {
    ui->setupUi(this);
    mModel =
        new SystemUserTableModel(this, QSqlDatabase::database(DEFAULT_DB_NAME));
    ui->tableView->setModel(mModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    QMap<QString, RecordDisplayWidget::FieldType> fieldTypes;
    fieldTypes["username"] = RecordDisplayWidget::FieldType::LineEdit;
    fieldTypes["password"] = RecordDisplayWidget::FieldType::PasswordEdit;
    fieldTypes["first_name"] = RecordDisplayWidget::FieldType::LineEdit;
    fieldTypes["last_name"] = RecordDisplayWidget::FieldType::LineEdit;
    fieldTypes["email"] = RecordDisplayWidget::FieldType::LineEdit;
    fieldTypes["role"] = RecordDisplayWidget::FieldType::ComboBox;

    ui->recordDetails->setupForRecord(mModel->record(), fieldTypes);
    ui->recordDetails->clearData();
    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &selected, const QModelIndex &) {
                ui->recordDetails->onDisplayRecord(
                    mModel->record(selected.row()));
            });

    connect(ui->recordDetails, &RecordDisplayWidget::editFinished, this,
            &AdministratorForm::editFinished);
    connect(ui->recordDetails, &RecordDisplayWidget::requestSave, this,
            &AdministratorForm::onRequestSave);
    QAction *action = new QAction(tr("New system user"), this);
    mActions.push_back(action);
    action = new QAction(tr("Edit system user"), this);
    connect(action, &QAction::triggered, this,
            &AdministratorForm::editSelected);
    mActions.push_back(action);
    action = new QAction(tr("Delete system user"), this);
    mActions.push_back(action);
    action = new QAction(this);
    action->setSeparator(true);
    mActions.push_back(action);
    action = new QAction(tr("Refresh data"), this);
    connect(action, &QAction::triggered, mModel, &SystemUserTableModel::select);
    connect(action, &QAction::triggered, ui->recordDetails,
            &RecordDisplayWidget::clearData);
    mActions.push_back(action);
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
    ui->recordDetails->onEditRecord(
        mModel->record(ui->tableView->selectionModel()->currentIndex().row()));
}

void AdministratorForm::editFinished() {
    ui->tableView->setDisabled(false);
    ui->tableView->setFocus();
}

void AdministratorForm::onRequestSave(QSqlRecord record) {
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
        ui->recordDetails->saveError();
        return;
    }

    ui->recordDetails->saveSuccessfull();
    ui->tableView->clearSelection();
    ui->tableView->selectRow(index.row());
    editFinished();
}
}
}
