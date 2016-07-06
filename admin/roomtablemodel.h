#ifndef ROOMTABLEMODEL_H
#define ROOMTABLEMODEL_H

#include "abstractquerymodel.h"

#include <QObject>
#include <QSqlTableModel>
#include <QVariantMap>

namespace paso {
namespace admin {

class RoomTableModel : public QSqlTableModel,
                       virtual public AbstractQueryModel {
    Q_OBJECT
public:
    explicit RoomTableModel(const QVariantMap &columnLabels,
                            QSqlDatabase db = QSqlDatabase(),
                            QObject *parent = nullptr);
};
}
}
#endif // ROOMTABLEMODEL_H
