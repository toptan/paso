#include "listform.h"
#include "ui_listform.h"

using namespace paso::widget;

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
    return make_pair<QSqlTableModel *, RecordEditorWidget *>(nullptr, nullptr);

}

void ListForm::prepareRecordForSaving(QSqlRecord &record) {

}

bool ListForm::shouldEnableEditAction(const QSqlRecord &record) const {
    return true;
}

bool ListForm::shouldEnableDeleteAction(const QSqlRecord &record) const {
    return true;
}

bool ListForm::shouldDeleteRecord(const QSqlRecord &record) const {
    return true;
}

void ListForm::updateActions(const QSqlRecord &record) {

}
}
}
