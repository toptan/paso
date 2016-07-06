#include "roomform.h"
#include "ui_roomform.h"

#include "data.h"
#include "pasodb.h"
#include "recordeditorwidget.h"
#include "roomvalidator.h"
#include "roomeditorwidget.h"

#include <QSqlField>

using namespace paso::db;
using namespace paso::data;

using namespace std;

namespace paso {
namespace admin {

RoomForm::RoomForm(QWidget *parent)
    : TableForm(createModelAndEditor(), parent), ui(new Ui::RoomForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);
    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);
}

RoomForm::~RoomForm() { delete ui; }

pair<AbstractQueryModel *, RecordEditorWidget *> RoomForm::createModelAndEditor() {
    QVariantMap columnLabels = {{"room_uuid", QObject::tr("Room UUID")},
                                {"name", QObject::tr("Name")},
                                {"room_number", QObject::tr("Number")}};

    auto model = new RoomTableModel(columnLabels,
                                    QSqlDatabase::database(DEFAULT_DB_NAME));
    FieldTypes fieldTypes = {{"room_uuid", FieldType::LineEdit},
                             {"name", FieldType::LineEdit},
                             {"room_number", FieldType::LineEdit}};

    auto editor = new RoomEditorWidget(model->record(), fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new RoomValidator(editor->fieldTypes(),
                                           editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlTableModel *, RecordEditorWidget *>(model, editor);
}

void RoomForm::prepareRecordForSaving(QSqlRecord &record) {
    // If value of the ID field is null, we need to remove it to let database
    // assign a value.
    auto index = record.indexOf("ID");
    if (index == -1) {
        return;
    }
    if (record.field(index).isNull()) {
        record.remove(index);
    }
}

bool RoomForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool RoomForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return !record.isEmpty();
}

bool RoomForm::shouldDeleteRecord(const QSqlRecord &record) const {
    // TODO: Check if something is connected with the room first!
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("Do you want to delete selected room from the system?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}
}
}
