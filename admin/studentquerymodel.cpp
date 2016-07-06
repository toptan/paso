#include "studentquerymodel.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QDebug>

namespace paso {
namespace admin {

StudentQueryModel::StudentQueryModel(const QVariantMap &columnLabels,
                                     QSqlDatabase db, QObject *parent)
    : QSqlQueryModel(parent) {
    setQuery("SELECT P.ID, P.FIRST_NAME, P.LAST_NAME, P.EMAIL, P.RFID, "
             "S.INDEX_NUMBER, S.YEAR_OF_STUDY "
             "FROM PERSON P JOIN STUDENT S USING (ID)",
             db);
    //    query().exec();
    qDebug() << lastError() << " SIZE: " << query().size();
    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}

QVariant StudentQueryModel::data(const QModelIndex &idx, int role) const {
    auto retVal = QSqlQueryModel::data(idx, role);
    qDebug() << idx << " - " << retVal;
    return retVal;
}
}
}
