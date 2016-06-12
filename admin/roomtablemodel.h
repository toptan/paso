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
    RoomTableModel(QSqlDatabase db = QSqlDatabase(), QObject *parent = nullptr);
};
}
}
#endif // ROOMTABLEMODEL_H
