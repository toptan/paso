#ifndef ACTIVITYTABLEMODEL_H
#define ACTIVITYTABLEMODEL_H

#include <QSqlTableModel>
#include <QVariantMap>

namespace paso {
namespace admin {

class ActivityTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    explicit ActivityTableModel(const QVariantMap &columnLabels,
                                QSqlDatabase db = QSqlDatabase(),
                                QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const override;
};
}
}

#endif // ACTIVITYTABLEMODEL_H
