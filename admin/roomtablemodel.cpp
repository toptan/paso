#include "roomtablemodel.h"

#include <QSqlRecord>

namespace paso {
namespace admin {

RoomTableModel::RoomTableModel(const QVariantMap &columnLabels, QSqlDatabase db,
                               QObject *parent)
    : AbstractQueryModel("", parent), QSqlTableModel(parent, db) {
    setTable("room");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlTableModel::select();
    const auto &rec = QSqlTableModel::record();
    for (auto i = 0; i < rec.count(); i++) {
        QSqlTableModel::setHeaderData(i, Qt::Horizontal,
                                      columnLabels[rec.fieldName(i)]);
    }
}
}
}
