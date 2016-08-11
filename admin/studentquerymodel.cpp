#include "studentquerymodel.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDebug>

namespace paso {
namespace model {

const QString StudentQueryModel::QUERY =
    "SELECT P.ID, S.INDEX_NUMBER, P.LAST_NAME, P.FIRST_NAME, P.EMAIL, "
    "S.YEAR_OF_STUDY, P.RFID "
    "FROM PERSON P JOIN STUDENT S USING (ID)";

StudentQueryModel::StudentQueryModel(const QVariantMap &columnLabels,
                                     QSqlDatabase db, QObject *parent)
    : RefreshableSqlQueryModel(QUERY, db.connectionName(), parent) {

    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}

QVariant StudentQueryModel::data(const QModelIndex &item, int role) const {
    if (role == Qt::TextAlignmentRole && item.column() == 5) {
        return Qt::AlignCenter;
    }

    return RefreshableSqlQueryModel::data(item, role);
}

}
}
