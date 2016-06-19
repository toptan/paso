#ifndef COURSETABLEMODEL_H
#define COURSETABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QVariantMap>

namespace paso {
namespace admin {

class CourseTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    explicit CourseTableModel(const QVariantMap &columnLabels,
                              QSqlDatabase db = QSqlDatabase(),
                              QObject *parent = nullptr);
};
}
}

#endif // COURSETABLEMODEL_H
