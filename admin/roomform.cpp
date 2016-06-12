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

    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &selected, const QModelIndex &) {
                auto record = model()->record(selected.row());
                this->onSelectionChanged(record);
            });

    mEditRoomAction = new QAction(tr("Edit room"), this);
    // connect(mEditRoomAction)
    toolBarActions().push_back(mEditRoomAction);
    mDeleteRoomAction = new QAction(tr("Delete room"), this);
    // connect(mDeleteRoomAction);
    toolBarActions().push_back(mDeleteRoomAction);
    auto separator = new QAction(this);
    separator->setSeparator(true);
    toolBarActions().push_back(separator);
    mRefreshAction = new QAction(tr("Refresh data"), this);
    // connect(mRefreshAction)
    toolBarActions().push_back(mRefreshAction);
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

void RoomForm::onSelectionChanged(const QSqlRecord &record) {}
}
}
