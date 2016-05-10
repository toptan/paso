#include "pasodb.h"

#include <QSqlRecord>
#include <QSqlError>

namespace paso {
namespace db {

using namespace std;

QVariantMap recordToVariantMap(const QSqlRecord &record) {
    QVariantMap retVal;
    for (auto i = 0; i < record.count(); i++) {
        retVal.insert(record.fieldName(i), record.value(i));
    }
    return retVal;
}

shared_ptr<QVector<QVariantMap>> getAllSystemUsers(const QString &dbName,
                                                   QSqlError &error) {
    auto db = QSqlDatabase::database(dbName);
    auto query = db.exec("SELECT * FROM SYSTEM_USER ORDER BY USERNAME");
    error = query.lastError();
    auto retVal = make_shared<QVector<QVariantMap>>();
    if (error.type() == QSqlError::NoError) {
//        auto record = query.record();
//        while (query.next()) {
//            QVariantMap row;
//            for (auto i = 0; i < record.count(); i++) {
//                row.insert(record.fieldName(i), query.value(i));
//            }
        while (query.next()) {
            retVal->push_back(recordToVariantMap(query.record()));
        }
    }
    return retVal;
}

QVariantMap getSystemUser(const QString &dbName, const QString &username,
                          QSqlError &error) {
    auto db = QSqlDatabase::database(dbName);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM SYSTEM_USER WHERE USERNAME = :username");
    query.bindValue(":username", username);
    query.exec();
    error = query.lastError();
    QVariantMap retVal;
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            retVal = recordToVariantMap(query.record());
        }
    }
    return retVal;
}
}
}
