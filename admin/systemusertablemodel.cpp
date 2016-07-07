#include "systemusertablemodel.h"

#include "data.h"

#include <QApplication>
#include <QSqlRecord>

namespace paso {
namespace admin {

using namespace paso::data;

SystemUserTableModel::SystemUserTableModel(const QVariantMap &columnLabels,
                                           QSqlDatabase db, QObject *parent)
    : QSqlTableModel(parent, db) {
    setTable("system_user");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}

QVariant SystemUserTableModel::data(const QModelIndex &idx, int role) const {
    if (role == Qt::DisplayRole) {
        if (idx.column() == 2) {
            return tr("<hidden>");
        } else if (idx.column() == 6) {
            return QApplication::instance()->translate(
                "QObject", stringEnumeratedRoles[QSqlTableModel::data(idx, role)
                                                     .toString()]
                               .toStdString()
                               .c_str());
        }
    }
    return QSqlTableModel::data(idx, role);
}
}
}
