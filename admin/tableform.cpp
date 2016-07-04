#include "tableform.h"

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

bool TableForm::insertRecord(const QSqlRecord &record, QSqlError &error) {
    tableModel()->insertRecord(-1, record);
    if (!tableModel()->submitAll()) {
        error = tableModel()->lastError();
        return false;
    }
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
