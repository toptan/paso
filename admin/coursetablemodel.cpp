#include "coursetablemodel.h"

#include "data.h"

#include <QSqlRecord>

namespace paso {
namespace admin {

using namespace paso::data;

CourseTableModel::CourseTableModel(const QVariantMap &columnLabels,
                                   QSqlDatabase db, QObject *parent)
    : QSqlTableModel(parent, db) {
    setTable("course");
    setEditStrategy(QSqlTableModel::OnManualSubmit);
    select();
    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}
}
}
