#include "roomtablemodel.h"

#include <QSqlRecord>

namespace paso {
namespace model {

const QString RoomTableModel::QUERY = "SELECT * FROM ROOMS_VIEW";

RoomTableModel::RoomTableModel(const QVariantMap &columnLabels, QSqlDatabase db,
                               QObject *parent)
    : RefreshableSqlQueryModel(QUERY, db.connectionName(), parent) {
    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}
}
}
