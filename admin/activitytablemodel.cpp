#include "activitytablemodel.h"

#include "data.h"

#include <QApplication>
#include <QSqlRecord>
#include <QTextStream>

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
        } else if (idx.column() == 4) {
            return generateRepetitionString(idx);
        } else if (idx.column() == 9) {
            return RefreshableSqlQueryModel::data(idx, role).toBool()
                       ? QApplication::instance()->translate("QObject", "Yes")
                       : QApplication::instance()->translate("QObject", "No");
        }
    }
    return RefreshableSqlQueryModel::data(idx, role);
}

QString
ActivityQueryModel::generateRepetitionString(const QModelIndex &idx) const {
    QString retVal = "";
    auto typeIndex = index(idx.row(), 3);
    if (RefreshableSqlQueryModel::data(typeIndex) == "WEEK_DAYS") {
        const QVariantList days = jsonArrayStringToVariantList(
            RefreshableSqlQueryModel::data(idx).toString());
        QTextStream ts(&retVal);
        for (const auto &day : days) {
            ts << repetitionWeekDays[day.toInt() - 1] << ", ";
        }
        retVal = retVal.trimmed();
        if (retVal.endsWith(",")) {
            retVal.chop(1);
        }
        auto lastIndex = retVal.lastIndexOf(",");
        retVal = retVal.replace(lastIndex, 1, QString(" ") + tr("and"));
    } else if (RefreshableSqlQueryModel::data(typeIndex) == "MONTH_DAYS") {
        const QVariantList days = jsonArrayStringToVariantList(
            RefreshableSqlQueryModel::data(idx).toString());
        QTextStream ts(&retVal);
        for (const auto &day : days) {
            ts << day.toInt() + 1 << ", ";
        }
        retVal = retVal.trimmed();
        if (retVal.endsWith(",")) {
            retVal.chop(1);
        }
        auto lastIndex = retVal.lastIndexOf(",");
        retVal = retVal.replace(lastIndex, 1, QString(" ") + tr("and"));
        retVal = tr("Every ") + retVal + QString(" ") + tr("in month");
    }
    return retVal;
}
}
}
