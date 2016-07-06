#include "studentquerymodel.h"

#include "pasodb.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

using namespace paso::db;

namespace paso {
namespace admin {

const QString StudentQueryModel::STR_QUERY =
    "SELECT P.ID, S.INDEX_NUMBER, P.FIRST_NAME, P.LAST_NAME, S.YEAR_OF_STUDY, "
    "P.EMAIL, P.RFID FROM PERSON P JOIN STUDENT S USING (ID)";

StudentQueryModel::StudentQueryModel(const QVariantMap &columnLabels,
                                     QSqlDatabase db, QObject *parent)
    : AbstractQueryModel(STR_QUERY, parent) {
    setQuery(STR_QUERY, db);
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

bool StudentQueryModel::select() {
    setQuery(STR_QUERY, QSqlDatabase::database(DEFAULT_DB_NAME));
//    const auto &rec = record();
//    for (auto i = 0; i < rec.count(); i++) {
//        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
//    }
    return true;
}

QSqlError StudentQueryModel::lastError() const {
    return QSqlQueryModel::lastError();
}

bool StudentQueryModel::insertRecord(int row, const QSqlRecord &record) {
    return true;
}

bool StudentQueryModel::setRecord(int row, const QSqlRecord &record) {
    return true;
}

bool StudentQueryModel::submitAll() { return true; }
}
}
