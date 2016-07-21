#ifndef ADDREMOVEENTITIESFORM_H
#define ADDREMOVEENTITIESFORM_H

#include "entity.h"
#include "entitytablemodel.h"
#include "stablerownumbersortfilterproxymodel.h"
#include <QMap>
#include <QStringList>
#include <QWidget>
#include <set>

namespace Ui {
class AddRemoveEntitiesForm;
}

namespace paso {
namespace widget {

///
/// \brief The AddRemoveEntitiesForm class is a widget class that provides two
/// tables and is able to transfer entities from one table to another.
///
class AddRemoveEntitiesForm : public QWidget {
    Q_OBJECT

public:
    explicit AddRemoveEntitiesForm(QWidget *parent = 0);
    ~AddRemoveEntitiesForm();

    ///
    /// Returns whether there were changes.
    ///
    /// \return \c true if there were changes.
    ///
    bool dirty() const;

    ///
    /// Returns entities that were added to destination.
    ///
    /// \return Entities added to the destination.
    ///
    std::set<std::shared_ptr<data::entity::Entity>> addedEntities() const;

    ///
    /// Returns entities that were removed from the destination.
    ///
    /// \return Entities removed from the destination.
    ///
    std::set<std::shared_ptr<data::entity::Entity>> removedEntities() const;

    ///
    /// Prepares widget to operate by giving necessary data.
    ///
    /// \param sourceLabel The label text for the source table.
    /// \param sourceColumns The columns to expose to source table.
    /// \param sourceColumnNames The column names for source table header.
    /// \param sourceData The source data itself.
    /// \param destinationLabel The label text for the destination table.
    /// \param destinationColumns The columns to expose to the destination
    /// table.
    /// \param destinationColumnNames The column names for the destination table
    /// header.
    /// \param destinationData The destination data itself.
    /// \param totalLabelsVisible Whether to show total labels for source and
    /// destination.
    ///
    void setData(const QString &sourceLabel, const QStringList &sourceColumns,
                 const QMap<QString, QString> &sourceColumnNames,
                 const data::entity::EntityVector &sourceData,
                 const QString &destinationLabel,
                 const QStringList &destinationColumns,
                 const QMap<QString, QString> &destinationColumnNames,
                 const data::entity::EntityVector &destinationData,
                 bool totalLabelsVisible = true);

private slots:
    ///
    /// The slot to be executed when add button is clicked.
    ///
    void addButtonClicked();
    ///
    /// The slot to be executed when remove button is clicked.
    ///
    void removeButtonClicked();
    ///
    /// The slot to be executed when reset button is clicked.
    ///
    void resetButtonClicked();

private:
    Ui::AddRemoveEntitiesForm *ui;

    /// The source data.
    data::entity::EntityVector mSourceData;
    /// The destination data.
    data::entity::EntityVector mDestinationData;
    /// Entities transfered from source to destination.
    std::set<std::shared_ptr<data::entity::Entity>> mAddedEntities;
    /// Entities transfered back from destination to the source.
    std::set<std::shared_ptr<data::entity::Entity>> mRemovedEntities;
    /// The table model for the source data.
    model::EntityTableModel *mSourceModel;
    /// The table model for the destination data.
    model::EntityTableModel *mDestinationModel;
    /// The proxy model for the source data.
    model::StableRowNumberSortFilterProxyModel *mSourceProxyModel;
    /// The proxy model for the destination data.
    model::StableRowNumberSortFilterProxyModel *mDestinationProxyModel;
};
}
}

#endif // ADDREMOVEENTITIESFORM_H
