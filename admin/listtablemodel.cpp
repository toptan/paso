#include "listtablemodel.h"

#include <QDate>
#include <QSqlRecord>

namespace paso {
namespace admin {

ListTableModel::ListTableModel(const QVariantMap &columnLabels, QSqlDatabase db,
                               QObject *parent)
    : QSqlTableModel(parent, db) {
    setTable("list");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();

    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}

QVariant ListTableModel::data(const QModelIndex &idx, int role) const {
    if (role == Qt::DisplayRole) {
        if (idx.column() == 2 || idx.column() == 3 || idx.column() == 4) {
            return QSqlTableModel::data(idx, role).toBool() ? tr("Yes")
                                                            : tr("No");
        }
        if (idx.column() == 5) {
            return QSqlTableModel::data(idx, role).toDate().toString(
                "dd.MM.yyyy.");
        }
    }
    if (role == Qt::TextAlignmentRole) {
        if (idx.column() == 2 || idx.column() == 3 || idx.column() == 4) {
            return Qt::AlignCenter;
        }
    }
    return QSqlTableModel::data(idx, role);
}
}
}
