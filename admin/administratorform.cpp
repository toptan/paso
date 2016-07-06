#include "administratorform.h"
#include "ui_administratorform.h"

#include "data.h"
#include "pasodb.h"
#include "systemusereditorwidget.h"
#include "systemusertablemodel.h"
#include "systemuservalidator.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlField>
#include <QSqlTableModel>

using namespace paso::db;
using namespace paso::data;

using namespace std;

namespace paso {
namespace admin {

AdministratorForm::AdministratorForm(QWidget *parent)
    : TableForm(createModelAndEditor(), parent), ui(new Ui::AdministratorForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);

    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);
}

AdministratorForm::~AdministratorForm() { delete ui; }

pair<AbstractQueryModel *, RecordEditorWidget *>
AdministratorForm::createModelAndEditor() {
    QVariantMap columnLabels = {{"username", QObject::tr("Username")},
                                {"password", QObject::tr("Password")},
                                {"first_name", QObject::tr("First Name")},
                                {"last_name", QObject::tr("Last Name")},
                                {"email", QObject::tr("Email")},
                                {"role", QObject::tr("Role")}};
    auto model = new SystemUserTableModel(
        columnLabels, QSqlDatabase::database(DEFAULT_DB_NAME));
    FieldTypes fieldTypes = {{"username", FieldType::LineEdit},
                             {"password", FieldType::PasswordEdit},
                             {"first_name", FieldType::LineEdit},
                             {"last_name", FieldType::LineEdit},
                             {"email", FieldType::LineEdit},
                             {"role", FieldType::ComboBox}};

    auto editor = new SystemUserEditorWidget(model->record(), fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new SystemUserValidator(
        editor->fieldTypes(), editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlTableModel *, RecordEditorWidget *>(model, editor);
}

void AdministratorForm::prepareRecordForSaving(QSqlRecord &record) {
    // If value of the ID field is null, we need to remove it to let database
    // assign a value.
    auto index = record.indexOf("ID");
    if (index == -1) {
        return;
    }
    if (record.field(index).isNull()) {
        record.remove(index);
    }
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
