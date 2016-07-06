#include "queryform.h"

#include <QSqlQueryModel>
#include <QSqlError>

namespace paso {
namespace admin {

QueryForm::QueryForm(
    std::pair<AbstractQueryModel *, RecordEditorWidget *> modelAndEditor,
    QWidget *parent)
    : AbstractForm(modelAndEditor, parent) {}

QueryForm::~QueryForm() {}

bool QueryForm::removeRow(int row, QSqlError &error) { return true; }

bool QueryForm::updateRecord(int row, const QSqlRecord &record,
                             QSqlError &error) {
    return true;
}
}
}
