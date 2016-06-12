#include "systemusertablemodel.h"

#include "data.h"

#include <QSqlRecord>

namespace paso {
namespace admin {

using namespace paso::data;

SystemUserTableModel::SystemUserTableModel(QSqlDatabase db, QObject *parent)
    : QSqlTableModel(parent, db) {
    setTable("SYSTEM_USER");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}

QVariant SystemUserTableModel::data(const QModelIndex &idx, int role) const {
    if (role == Qt::DisplayRole) {
        if (idx.column() == 1) {
            return tr("<hidden>");
        } else if (idx.column() == 5) {
            return stringEnumeratedRoles[QSqlTableModel::data(idx, role)
                                             .toString()];
        }
    }
    return QSqlTableModel::data(idx, role);
}
}
}
