#ifndef ACTIVITYTABLEMODEL_H
#define ACTIVITYTABLEMODEL_H

#include "refreshablesqlquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace model {

///
/// \brief The ActivityQueryModel class handles data about activities.
///
class ActivityQueryModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    ///
    /// \brief Constructs model with given column labels and database.
    /// \param columnLabels The column labels.
    /// \param db The database to use.
    /// \param parent The parent object.
    ///
    explicit ActivityQueryModel(const QVariantMap &columnLabels,
                                QSqlDatabase db = QSqlDatabase(),
                                QObject *parent = nullptr);

    ///
    /// \brief Overrides default behavior to support custom data display.
    /// \param idx The index of an item to be displayed.
    /// \param role The display role.
    /// \return Variant containing neccessary data for proper displaying.
    ///
    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const override;

private:
    ///
    /// \brief QUERY The query string used to fetch the data.
    ///
    static const QString QUERY;

    ///
    /// \brief Generate string of days or dates when activity occurs.
    /// \details Repetitive string may be generated for both weekly or monthly
    /// repetitions and looks like "Monday, Wednesday and Friday" or "Every 10,
    /// 15 and 20 in month"
    /// \param idx The index of an item for which to generate repetition string.
    /// \return The repetition string.
    ///
    QString generateRepetitionString(const QModelIndex &idx) const;
};
}
}

#endif // ACTIVITYTABLEMODEL_H
