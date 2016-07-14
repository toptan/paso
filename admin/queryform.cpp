#include "queryform.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>

using namespace paso::db;

namespace paso {
namespace admin {

QueryForm::QueryForm(
    std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
    QWidget *parent)
    : AbstractForm(modelAndEditor, parent), mManager(DEFAULT_DB_NAME) {}

QueryForm::~QueryForm() {}

bool QueryForm::removeRow(int row, QSqlError &error) { return true; }

bool QueryForm::updateRecord(int row, const QSqlRecord &record,
                             QSqlError &error) {
    return true;
}

DBManager &QueryForm::manager() { return mManager; }
}
}
