#ifndef ABSTRACTQUERYMODEL_H
#define ABSTRACTQUERYMODEL_H

#include <QSqlError>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QString>

namespace paso {
namespace admin {

class AbstractQueryModel : public QSqlQueryModel {
public:
    explicit AbstractQueryModel(const QString &queryString = "",
                                QObject *parent = nullptr);

    virtual bool select() = 0;
    virtual QSqlError lastError() const = 0;
    virtual bool insertRecord(int row, const QSqlRecord &record) = 0;
    virtual bool setRecord(int row, const QSqlRecord &record) = 0;
    QString queryString() const;

public slots:
    virtual bool submitAll() = 0;

private:
    const QString m_queryString;
};
}
}

#endif // ABSTRACTQUERYMODEL_H
