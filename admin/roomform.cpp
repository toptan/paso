#include "roomform.h"
#include "ui_roomform.h"

#include "data.h"
#include "pasodb.h"
#include "recordeditorwidget.h"
#include "roomeditorwidget.h"
#include "roomvalidator.h"

#include <QSqlField>

using namespace paso::db;
using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::model;
using namespace paso::widget;

using namespace std;

namespace paso {
namespace admin {

RoomForm::RoomForm(QWidget *parent)
    : QueryForm(createModelAndEditor(), parent), ui(new Ui::RoomForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(4);
    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 2);
}

RoomForm::~RoomForm() { delete ui; }

std::pair<QSqlQueryModel *, RecordEditorWidget *>
RoomForm::createModelAndEditor() {
    QVariantMap columnLabels = {{"room_uuid", QObject::tr("Room UUID")},
                                {"name", QObject::tr("Name")},
                                {"room_number", QObject::tr("Number")}};

    auto model = new RoomTableModel(columnLabels,
                                    QSqlDatabase::database(DEFAULT_DB_NAME));
    FieldTypes fieldTypes = {{"room_uuid", FieldType::LineEdit},
                             {"name", FieldType::LineEdit},
                             {"room_number", FieldType::LineEdit}};

    auto editor = new RoomEditorWidget(fieldTypes);
    editor->setupUi(columnLabels, model->record(), {"barred_students"});
    editor->setValidator(new RoomValidator(editor->fieldTypes(),
                                           editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlQueryModel *, RecordEditorWidget *>(model, editor);
}

void RoomForm::prepareRecordForSaving(QSqlRecord &record) {
    // If value of the ID field is null, we need to remove it to let database
    // assign a value.
    auto index = record.indexOf("id");
    if (record.value(index).isNull()) {
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

void RoomForm::updateActions(const QSqlRecord &record) {
    // No form specific actions.
}

bool RoomForm::insertRecord(QSqlRecord &record, QSqlError &error) {
    auto map = DBManager::recordToVariantMap(record);
    map.remove("id");
    Room room(map);
    auto success = manager().saveRoom(room, error);
    if (success) {
        refreshModel();
        record.setValue("id", room.id());
    }
    return success;
}

bool RoomForm::updateRecord(int row, const QSqlRecord &record,
                            QSqlError &error) {
    auto map = DBManager::recordToVariantMap(record);
    Room room(map);
    auto success = manager().saveRoom(room, error);
    if (success) {
        refreshModel();
    }
    return success;
}

bool RoomForm::removeRow(int row, QSqlError &error) {
    auto roomUUID = model()->record(row).value("room_uuid").toUuid();
    auto success = manager().deleteRoom(roomUUID, error);
    if (success) {
        refreshModel();
    }
    return success;
}
}
}
