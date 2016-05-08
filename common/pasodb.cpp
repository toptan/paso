#include "pasodb.h"

#include <QSqlRecord>

namespace paso {
namespace db {

using namespace std;

shared_ptr<QVector<QVariantMap>> getAllSystemUsers(const QString &db_name) {
  auto db = QSqlDatabase::database(db_name);
  auto query = db.exec("SELECT * FROM SYSTEM_USER");
  auto retVal = make_shared<QVector<QVariantMap>>();
  auto record = query.record();
  while (query.next()) {
    QVariantMap row;
    for (auto i = 0; i < record.count(); i++) {
      row.insert(record.fieldName(i), query.value(i));
    }
    retVal->push_back(row);
  }
  return retVal;
}
}
}
