#include "listform.h"
#include "ui_listform.h"

#include "listtablemodel.h"
#include "listvalidator.h"

#include <QSqlField>

using namespace paso::widget;
using namespace paso::db;

using namespace std;

namespace paso {
namespace admin {

ListForm::ListForm(QWidget *parent)
    : TableForm(createModelAndEditor(), parent), ui(new Ui::ListForm) {
    ui->setupUi(this);
    setupWidgets(ui->tableView);
    ui->tableView->hideColumn(0);
    ui->tableView->hideColumn(5);
    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);

    auto separator = new QAction(this);
    mDetailsAction = new QAction(tr("Details"), this);
    mDetailsAction->setObjectName("DETAILS_ACTION");
    toolBarActions().append(separator);
    toolBarActions().append(mDetailsAction);
    mDetailsAction->setEnabled(false);

    connect(mDetailsAction, &QAction::triggered, this, &ListForm::onDetails);
}

ListForm::~ListForm() { delete ui; }

pair<QSqlTableModel *, RecordEditorWidget *> ListForm::createModelAndEditor() {
    const QVariantMap columnLabels = {
        {"name", QObject::tr("Name")},
        {"system", QObject::tr("System list")},
        {"permanent", QObject::tr("Permanent list")},
        {"expiry_date", QObject::tr("Expiry date")}};

    auto model = new ListTableModel(columnLabels,
                                    QSqlDatabase::database(DEFAULT_DB_NAME));

    const FieldTypes fieldTypes = {{"name", FieldType::LineEdit},
                                   {"system", FieldType::CheckBox},
                                   {"permanent", FieldType::CheckBox},
                                   {"expiry_date", FieldType::DateEdit}};

    auto editor = new ListEditorWidget(fieldTypes);
    editor->setupUi(columnLabels, model->record());
    editor->setValidator(new ListValidator(editor->fieldTypes(),
                                           editor->fieldEditors(), editor));
    editor->clearData();

    return make_pair<QSqlTableModel *, RecordEditorWidget *>(model, editor);
}

void ListForm::prepareRecordForSaving(QSqlRecord &record) {
    // If value of the ID field is null, we need to remove it to let database
    // assign a value.
    auto index = record.indexOf("id");
    if (record.field(index).isNull()) {
        record.remove(index);
    }
    // If list is permanent than it has no expiry date.
    if (record.value("permanent").toBool()) {
        record.setNull("expiry_date");
    }
}

bool ListForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return !record.value("system").toBool();
}

bool ListForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return !record.value("system").toBool();
}

bool ListForm::shouldDeleteRecord(const QSqlRecord &record) const {
    // TODO: Check if something is connected with the list first!
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setText(tr("Do you want to delete selected list from the system?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    return msgBox.exec() == QMessageBox::Yes;
}

void ListForm::updateActions(const QSqlRecord &record) {
    mDetailsAction->setEnabled(record.value("id") != 0);
}

void ListForm::onDetails() {
    // TODO: Implement list details.
}
}
}
