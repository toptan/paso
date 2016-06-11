#include "roomform.h"
#include "ui_roomform.h"

#include "data.h"
#include "pasodb.h"
#include "roomvalidator.h"

using namespace paso::db;
using namespace paso::data;

namespace paso {
namespace admin {

RoomForm::RoomForm(QWidget *parent)
    : AbstractForm(parent), ui(new Ui::RoomForm) {
    ui->setupUi(this);
    mModel = new RoomTableModel(this, QSqlDatabase::database(DEFAULT_DB_NAME));
    ui->tableView->setModel(mModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);

    FieldTypes fieldTypes = {{"room_uuid", FieldType::LineEdit},
                             {"name", FieldType::LineEdit},
                             {"room_number", FieldType::LineEdit}};

    mRecordEditor = new RoomEditorWidget(mModel->record(), fieldTypes, this);
    mRecordEditor->setValidator(new RoomValidator(
        mRecordEditor->fieldTypes(), mRecordEditor->fieldEditors(), this));
    mRecordEditor->clearData();

    ui->horizontalLayout->addWidget(mRecordEditor);
    ui->horizontalLayout->setStretch(0, 3);

    connect(ui->tableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &selected, const QModelIndex &) {
                auto record = mModel->record(selected.row());
                this->onSelectionChanged(record);
            });
}

RoomForm::~RoomForm() { delete ui; }

const QList<QAction *> &RoomForm::toolBarActions() const { return mActions; }

void RoomForm::onSelectionChanged(const QSqlRecord &record) {}
}
}
