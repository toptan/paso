#include "activityform.h"
#include "ui_activityform.h"

#include "activitytablemodel.h"
#include "activityvalidator.h"

#include <QDebug>
#include <QSqlField>

using namespace paso::widget;
using namespace paso::db;

using namespace std;

namespace paso {
namespace admin {

ActivityForm::ActivityForm(QWidget *parent)
    : TableForm(createModelAndEditor(), parent), ui(new Ui::ActivityForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);
    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);
}

ActivityForm::~ActivityForm() { delete ui; }

pair<QSqlTableModel *, RecordEditorWidget *>
ActivityForm::createModelAndEditor() {
    const QVariantMap columnLabels{{"name", QObject::tr("Name")},
                                   {"type", QObject::tr("Type")},
                                   {"schedule", QObject::tr("Schedule")},
                                   {"duration", QObject::tr("Duration")},
                                   {"start_date", QObject::tr("Start date")},
                                   {"finish_date", QObject::tr("Finish date")}};
    const FieldTypes fieldTypes{{"name", FieldType::LineEdit},
                                {"type", FieldType::ComboBox},
                                {"schedule", FieldType::LineEdit},
                                {"duration", FieldType::TimeEdit},
                                {"start_date", FieldType::DateEdit},
                                {"finish_date", FieldType::DateEdit}};

    auto model = new ActivityTableModel(
        columnLabels, QSqlDatabase::database(DEFAULT_DB_NAME));
    auto editor = new ActivityEditorWidget(fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new ActivityValidator(editor->fieldTypes(),
                                               editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlTableModel *, RecordEditorWidget *>(model, editor);
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

void ActivityForm::onNewRecord() {
    // Show wizard.
}
}
}
