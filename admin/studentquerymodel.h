#ifndef STUDENTQUERYMODEL_H
#define STUDENTQUERYMODEL_H

#include "refreshablesqlquerymodel.h"

#include "pasodb.h"
#include "student.h"

#include <QObject>
#include <QVariantMap>

using namespace paso::db;
using namespace paso::data::entity;

namespace paso {
namespace admin {

class StudentQueryModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    explicit StudentQueryModel(const QVariantMap &columnLabels, QSqlDatabase db,
                               QObject *parent = nullptr);

private:
    static const QString QUERY;
};
}
}

#endif // STUDENTQUERYMODEL_H
