#include "roomtablemodel.h"

#include <QSqlRecord>

namespace paso {
namespace admin {

RoomTableModel::RoomTableModel(const QVariantMap &columnLabels, QSqlDatabase db,
                               QObject *parent)
    : QSqlTableModel(parent, db) {
    setTable("room");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}
}
}
