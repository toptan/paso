#ifndef ACTIVITYTABLEMODEL_H
#define ACTIVITYTABLEMODEL_H

#include "refreshablesqlquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace model {

class ActivityQueryModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    explicit ActivityQueryModel(const QVariantMap &columnLabels,
                                QSqlDatabase db = QSqlDatabase(),
                                QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const override;

private:
    static const QString QUERY;

    QString generateRepetitionString(const QModelIndex &idx) const;
};
}
}

#endif // ACTIVITYTABLEMODEL_H
