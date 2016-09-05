#ifndef ROOMTABLEMODEL_H
#define ROOMTABLEMODEL_H

#include "refreshablesqlquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace model {

class RoomTableModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    explicit RoomTableModel(const QVariantMap &columnLabels,
                            QSqlDatabase db = QSqlDatabase(),
                            QObject *parent = nullptr);

private:
    static const QString QUERY;
};
}
}
#endif // ROOMTABLEMODEL_H
