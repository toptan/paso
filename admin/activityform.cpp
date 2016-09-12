#include "activityform.h"
#include "ui_activityform.h"

#include "activityquerymodel.h"
#include "activityvalidator.h"
#include "activitywizard.h"

#include <QDebug>
#include <QSqlField>

using namespace paso::widget;
using namespace paso::db;
using namespace paso::model;

using namespace std;

namespace paso {
namespace admin {

ActivityForm::ActivityForm(QWidget *parent)
    : QueryForm(createModelAndEditor(), parent), ui(new Ui::ActivityForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);
    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);
}

ActivityForm::~ActivityForm() { delete ui; }

pair<QSqlQueryModel *, RecordEditorWidget *>
ActivityForm::createModelAndEditor() {
    const QVariantMap columnLabels{
        {"name", QObject::tr("Name")},
        {"type", QObject::tr("Type")},
        {"schedule_type", QObject::tr("Schedule type")},
        {"scheduled_days", QObject::tr("Scheduled days")},
        {"duration", QObject::tr("Duration")},
        {"start_date", QObject::tr("Start date")},
        {"start_time", QObject::tr("Start time")},
        {"finish_date", QObject::tr("Finish date")},
        {"can_overlap", QObject::tr("Can overlap")}};
    const FieldTypes fieldTypes{{"name", FieldType::LineEdit},
                                {"type", FieldType::ComboBox},
                                {"schedule_type", FieldType::LineEdit},
                                {"scheduled_days", FieldType::LineEdit},
                                {"duration", FieldType::TimeEdit},
                                {"start_date", FieldType::DateEdit},
                                {"start_time", FieldType::TimeEdit},
                                {"finish_date", FieldType::DateEdit},
                                {"can_overlap", FieldType::CheckBox}};

    auto model = new ActivityQueryModel(
        columnLabels, QSqlDatabase::database(DEFAULT_DB_NAME));
    auto editor = new ActivityEditorWidget(fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new ActivityValidator(editor->fieldTypes(),
                                               editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlQueryModel *, RecordEditorWidget *>(model, editor);
}

void ActivityForm::prepareRecordForSaving(QSqlRecord &record) {
    // If value of the ID field is null, we need to remove it to let database
    // assign a value.
    auto index = record.indexOf("id");
    if (record.field(index).isNull()) {
        record.remove(index);
    }
}

bool ActivityForm::shouldEnableEditAction(const QSqlRecord &record) const {
    // All activities are editable.
    return true;
}

bool ActivityForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    // All activities are deletable.
    return true;
}

bool ActivityForm::shouldDeleteRecord(const QSqlRecord &record) const {
    // TODO: Check if something is connected with the activity first!
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(
        tr("Do you want to delete selected activity from the system?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}

void ActivityForm::updateActions(const QSqlRecord &record) {
    // No form specific actions.
}

bool ActivityForm::insertRecord(QSqlRecord &record, QSqlError &error) {
    // Insert is done via wizard so we just refresh model and return true.
    refreshModel();
    return true;
}

bool ActivityForm::updateRecord(int, const QSqlRecord &record,
                                QSqlError &error) {
    // Update is done via wizard so we just refresh model and return true.
    refreshModel();
    return true;
}

bool ActivityForm::removeRow(int row, QSqlError &error) {
    auto activityId = model()->record(row).value("id").toULongLong();
    auto success = manager().deleteActivity(activityId, error);
    if (success) {
        refreshModel();
    }
    return success;
}
}
}
