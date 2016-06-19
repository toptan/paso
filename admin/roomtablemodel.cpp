#include "roomtablemodel.h"

#include "data.h"

#include <QSqlRecord>

namespace paso {
namespace admin {

using namespace paso::data;

RoomTableModel::RoomTableModel(QSqlDatabase db, QObject *parent)
    : QSqlTableModel(parent, db) {
    setTable("ROOM");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }

}
}
}
