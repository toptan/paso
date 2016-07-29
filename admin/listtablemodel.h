#ifndef LISTTABLEMODEL_H
#define LISTTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QVariantMap>

namespace paso {
namespace admin {

class ListTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    explicit ListTableModel(const QVariantMap &columnLabels,
                            QSqlDatabase db = QSqlDatabase(),
                            QObject *parent = nullptr);
};
}
}

#endif // LISTTABLEMODEL_H
