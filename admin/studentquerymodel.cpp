#include "studentquerymodel.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDebug>

namespace paso {
namespace admin {

const QString StudentQueryModel::QUERY =
    "SELECT P.ID, P.FIRST_NAME, P.LAST_NAME, P.EMAIL, P.RFID, S.INDEX_NUMBER, "
    "S.YEAR_OF_STUDY FROM PERSON P JOIN STUDENT S USING (ID)";

StudentQueryModel::StudentQueryModel(const QVariantMap &columnLabels,
                                     QSqlDatabase db, QObject *parent)
    : RefreshableSqlQueryModel(QUERY, db.connectionName(), parent) {

    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}
}
}
