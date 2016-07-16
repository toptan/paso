#include "tableform.h"

#include <QDebug>
#include <QSqlTableModel>
#include <QSqlError>

namespace paso {
namespace admin {

TableForm::TableForm(
    std::pair<QSqlTableModel *, RecordEditorWidget *> modelAndEditor,
    QWidget *parent)
    : AbstractForm(modelAndEditor, parent) {}

TableForm::~TableForm() {}

bool TableForm::removeRow(int row, QSqlError &error) {
    if (!tableModel()->removeRow(row)) {
        error = tableModel()->lastError();
        return false;
    }

    if (!tableModel()->submitAll()) {
        error = tableModel()->lastError();
        return false;
    }
    return true;
}

bool TableForm::insertRecord(QSqlRecord &record, QSqlError &error) {
    tableModel()->insertRecord(-1, record);
    if (!tableModel()->submitAll()) {
        error = tableModel()->lastError();
        return false;
    }
    record = tableModel()->record(tableModel()->rowCount() - 1);
    return true;
}

bool TableForm::updateRecord(int row, const QSqlRecord &record,
                             QSqlError &error) {
    tableModel()->setRecord(row, record);
    if (!tableModel()->submitAll()) {
        error = tableModel()->lastError();
        return false;
    }

    return true;
}

QSqlTableModel *TableForm::tableModel() const {
    return dynamic_cast<QSqlTableModel *>(model());
}
}
}
