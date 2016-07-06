#ifndef ROOMTABLEMODEL_H
#define ROOMTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QVariantMap>

namespace paso {
namespace admin {

class RoomTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    explicit RoomTableModel(const QVariantMap &columnLabels,
                            QSqlDatabase db = QSqlDatabase(),
                            QObject *parent = nullptr);
};
}
}
#endif // ROOMTABLEMODEL_H
