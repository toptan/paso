#ifndef STUDENTQUERYMODEL_H
#define STUDENTQUERYMODEL_H

#include "refreshablesqlquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace model {

class StudentQueryModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    explicit StudentQueryModel(const QVariantMap &columnLabels, QSqlDatabase db,
                               QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &item, int role) const override;

private:
    static const QString QUERY;
};
}
}

#endif // STUDENTQUERYMODEL_H
