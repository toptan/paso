#include "queryform.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>

using namespace paso::db;

namespace paso {
namespace widget {

QueryForm::QueryForm(
    std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
    QWidget *parent)
    : AbstractForm(modelAndEditor, parent), mManager(DEFAULT_DB_NAME) {}

QueryForm::~QueryForm() {}

DBManager &QueryForm::manager() { return mManager; }
}
}
