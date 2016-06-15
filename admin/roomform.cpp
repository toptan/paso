#include "roomform.h"
#include "ui_roomform.h"

#include "data.h"
#include "pasodb.h"
#include "recordeditorwidget.h"
#include "roomvalidator.h"
#include "roomeditorwidget.h"

using namespace paso::db;
using namespace paso::data;

namespace paso {
namespace admin {

RoomForm::RoomForm(QWidget *parent)
    : AbstractForm(createModelAndEditor(), parent), ui(new Ui::RoomForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);

    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 2);
    ui->horizontalLayout->setStretch(1, 1);
}

RoomForm::~RoomForm() { delete ui; }

std::pair<QSqlTableModel *, RecordEditorWidget *>
RoomForm::createModelAndEditor() {
    auto model = new RoomTableModel(QSqlDatabase::database(DEFAULT_DB_NAME));
    FieldTypes fieldTypes = {{"room_uuid", FieldType::LineEdit},
                             {"name", FieldType::LineEdit},
                             {"room_number", FieldType::LineEdit}};

    auto editor = new RoomEditorWidget(model->record(), fieldTypes);
    editor->setValidator(new RoomValidator(editor->fieldTypes(),
                                           editor->fieldEditors(), editor));
    editor->clearData();

    return std::make_pair<QSqlTableModel *, RecordEditorWidget *>(model,
                                                                  editor);
}

void RoomForm::prepareRecordForSaving(QSqlRecord &record) {
    // Nothing to do
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
