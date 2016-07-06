#ifndef STUDENTQUERYMODEL_H
#define STUDENTQUERYMODEL_H

#include <QObject>
#include <QSqlQueryModel>
#include <QVariantMap>

namespace paso {
namespace admin {

class StudentQueryModel : public QSqlQueryModel {
    Q_OBJECT
public:
    explicit StudentQueryModel(const QVariantMap &columnLabels,
                               QSqlDatabase db = QSqlDatabase(),
                               QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const override;
};
}
}

#endif // STUDENTQUERYMODEL_H
