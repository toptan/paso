#include "systemusersform.h"
#include "ui_systemusersform.h"

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
using namespace paso::widget;

using namespace std;

namespace paso {
namespace admin {

SystemUsersForm::SystemUsersForm(QWidget *parent)
    : TableForm(createModelAndEditor(), parent), ui(new Ui::SystemUsersForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);

    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);
}

SystemUsersForm::~SystemUsersForm() { delete ui; }

pair<QSqlTableModel *, RecordEditorWidget *>
SystemUsersForm::createModelAndEditor() {
    const QVariantMap columnLabels{{"username", QObject::tr("Username")},
                                   {"password", QObject::tr("Password")},
                                   {"first_name", QObject::tr("First Name")},
                                   {"last_name", QObject::tr("Last Name")},
                                   {"email", QObject::tr("Email")},
                                   {"role", QObject::tr("Role")}};
    auto model = new SystemUserTableModel(
        columnLabels, QSqlDatabase::database(DEFAULT_DB_NAME));
    const FieldTypes fieldTypes{{"username", FieldType::LineEdit},
                                {"password", FieldType::PasswordEdit},
                                {"first_name", FieldType::LineEdit},
                                {"last_name", FieldType::LineEdit},
                                {"email", FieldType::LineEdit},
                                {"role", FieldType::ComboBox}};

    auto editor = new SystemUserEditorWidget(fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new SystemUserValidator(
        editor->fieldTypes(), editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlTableModel *, RecordEditorWidget *>(model, editor);
}

void SystemUsersForm::prepareRecordForSaving(QSqlRecord &record) {
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

bool SystemUsersForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool SystemUsersForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return record.value("username").toString() != "root";
}

void SystemUsersForm::updateActions(const QSqlRecord &record) {
    // No form specific actions.
}

bool SystemUsersForm::shouldDeleteRecord(const QSqlRecord &record) const {
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("Do you want to delete selected system user?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}
}
}
