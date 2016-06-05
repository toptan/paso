#include "systemusertablemodel.h"

#include "data.h"

namespace paso {
namespace admin {

using namespace paso::data;

SystemUserTableModel::SystemUserTableModel(QObject *parent, QSqlDatabase db)
    : QSqlTableModel(parent, db) {
    setTable("SYSTEM_USER");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    setHeaderData(0, Qt::Horizontal, tr("Username"));
    setHeaderData(1, Qt::Horizontal, tr("Password"));
    setHeaderData(2, Qt::Horizontal, tr("First Name"));
    setHeaderData(3, Qt::Horizontal, tr("Last Name"));
    setHeaderData(4, Qt::Horizontal, tr("Email"));
    setHeaderData(5, Qt::Horizontal, tr("Role"));
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
