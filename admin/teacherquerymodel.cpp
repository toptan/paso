#include "teacherquerymodel.h"

#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

namespace paso {
namespace model {

const QString TeacherQueryModel::QUERY =
    "SELECT P.ID, T.EMPLOYEE_NUMBER, P.LAST_NAME, P.FIRST_NAME, P.EMAIL, "
    "T.OFFICE, P.RFID "
    "FROM PERSON P JOIN TEACHER T USING (ID)";

TeacherQueryModel::TeacherQueryModel(const QVariantMap &columnLabels,
                                     QSqlDatabase db, QObject *parent)
    : RefreshableSqlQueryModel(QUERY, db.connectionName(), parent) {

    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}
}
}
