#include "listform.h"
#include "ui_listform.h"

#include "listtablemodel.h"
#include "listvalidator.h"

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
    ui->horizontalLayout->addWidget(recordEditor());
    ui->horizontalLayout->setStretch(0, 3);
    ui->horizontalLayout->setStretch(1, 1);
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

void ListForm::prepareRecordForSaving(QSqlRecord &record) {}

bool ListForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return true;
}

bool ListForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return true;
}

bool ListForm::shouldDeleteRecord(const QSqlRecord &record) const {
    return true;
}

void ListForm::updateActions(const QSqlRecord &record) {}
}
}
