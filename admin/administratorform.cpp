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

void AdministratorForm::prepareRecordForSaving(QSqlRecord &record) {
    // Ensure that root always has SUPER_USER role
    if (record.value("username").toString() == "root") {
        record.setValue("role", roleToString(SystemRole::SUPER_USER));
    }
}

bool AdministratorForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool AdministratorForm::shouldEnableDeleteAction(
    const QSqlRecord &record) const {
    return record.value("username").toString() != "root";
}

bool AdministratorForm::shouldDeleteRecord(const QSqlRecord &record) const {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("Do you want to delete selected system user?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}
}
}
