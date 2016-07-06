#ifndef STUDENTQUERYMODEL_H
#define STUDENTQUERYMODEL_H

#include "abstractquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace admin {

class StudentQueryModel : public AbstractQueryModel {
    Q_OBJECT
public:
    explicit StudentQueryModel(const QVariantMap &columnLabels,
                               QSqlDatabase db = QSqlDatabase(),
                               QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const override;

    virtual bool select() override;
    virtual QSqlError lastError() const;
    virtual bool insertRecord(int row, const QSqlRecord &record);
    virtual bool setRecord(int row, const QSqlRecord &record);

public slots:
    virtual bool submitAll();

private:
    const static QString STR_QUERY;
};
}
}

#endif // STUDENTQUERYMODEL_H
