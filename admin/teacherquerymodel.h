#ifndef TEACHERQUERYMODEL_H
#define TEACHERQUERYMODEL_H

#include "refreshablesqlquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace model {

class TeacherQueryModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    TeacherQueryModel(const QVariantMap &columnLabels, QSqlDatabase db,
                      QObject *parent = nullptr);

private:
    static const QString QUERY;
};
}
}
#endif // TEACHERQUERYMODEL_H
