#ifndef ROOMTABLEMODEL_H
#define ROOMTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QVariant>

namespace paso {
namespace admin {

class RoomTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    RoomTableModel(QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase());
};
}
}
#endif // ROOMTABLEMODEL_H
