#include "abstractquerymodel.h"

namespace paso {
namespace admin {

AbstractQueryModel::AbstractQueryModel(const QString &queryString,
                                       QObject *parent)
    : QSqlQueryModel(parent), m_queryString(queryString) {}

QString AbstractQueryModel::queryString() const { return m_queryString; }
}
}
