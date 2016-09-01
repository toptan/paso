#include "activitytablemodel.h"

#include "data.h"

#include <QApplication>
#include <QSqlRecord>

namespace paso {
namespace model {

using namespace paso::data;

const QString ActivityQueryModel::QUERY = "SELECT * FROM ACTIVITY";

ActivityQueryModel::ActivityQueryModel(const QVariantMap &columnLabels,
                                       QSqlDatabase db, QObject *parent)
    : RefreshableSqlQueryModel(QUERY, db.connectionName(), parent) {
    const auto &rec = record();
    for (auto i = 0; i < rec.count(); i++) {
        setHeaderData(i, Qt::Horizontal, columnLabels[rec.fieldName(i)]);
    }
}

QVariant ActivityQueryModel::data(const QModelIndex &idx, int role) const {
    if (role == Qt::DisplayRole) {
        if (idx.column() == 2) {
            return QApplication::instance()->translate(
                "QObject",
                stringEnumeratedActivityTypes
                    [RefreshableSqlQueryModel::data(idx, role).toString()]
                        .toStdString()
                        .c_str());
        } else if (idx.column() == 3) {
            return QApplication::instance()->translate(
                "QObject",
                stringEnumeratedActivityScheduleTypes
                    [RefreshableSqlQueryModel::data(idx, role).toString()]
                        .toStdString()
                        .c_str());

        } else if (idx.column() == 7) {
            return RefreshableSqlQueryModel::data(idx, role).toBool()
                       ? QApplication::instance()->translate("QObject", "Yes")
                       : QApplication::instance()->translate("QObject", "No");
        }
    }
    return RefreshableSqlQueryModel::data(idx, role);
}
}
}
