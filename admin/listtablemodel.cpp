#include "listtablemodel.h"

#include <QDebug>
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
}
}
