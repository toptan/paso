#include "tableform.h"

#include "abstractquerymodel.h"

#include <QSqlError>

namespace paso {
namespace admin {

TableForm::TableForm(
    std::pair<AbstractQueryModel *, RecordEditorWidget *> modelAndEditor,
    QWidget *parent)
    : AbstractForm(modelAndEditor, parent) {}

TableForm::~TableForm() {}

bool TableForm::removeRow(int row, QSqlError &error) {
    if (!model()->removeRow(row)) {
        error = model()->lastError();
        return false;
    }

    if (!model()->submitAll()) {
        error = model()->lastError();
        return false;
    }
    return true;
}

bool TableForm::insertRecord(const QSqlRecord &record, QSqlError &error) {
    model()->insertRecord(-1, record);
    if (!model()->submitAll()) {
        error = model()->lastError();
        return false;
    }
    return true;
}

bool TableForm::updateRecord(int row, const QSqlRecord &record,
                             QSqlError &error) {
    model()->setRecord(row, record);
    if (!model()->submitAll()) {
        error = model()->lastError();
        return false;
    }

    return true;
}
}
}
