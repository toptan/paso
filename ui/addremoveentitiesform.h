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
    ///
    /// \brief The constructor.
    /// \param parent The parent widget.
    /// \param readOnly Whether to operate in read only mode.
    ///
    explicit AddRemoveEntitiesForm(QWidget *parent = 0, bool readOnly = false);

    ///
    /// \brief The destructor.
    ///
    virtual ~AddRemoveEntitiesForm();

    ///
    /// \brief Returns whether this form is read only.
    ///
    /// \return \c true if this form is read only.
    ///
    bool readOnly() const;

    ///
    /// \brief Sets whether this form should be read only.
    ///
    /// \param readOnly Whether this form should be read only.
    ///
    void setReadOnly(bool readOnly = true);

    ///
    /// \brief Returns whether there were changes.
    ///
    /// \return \c true if there were changes.
    ///
    bool dirty() const;

    ///
    /// \brief Returns entities that were added to destination.
    ///
    /// \return Entities added to the destination.
    ///
    std::set<std::shared_ptr<data::entity::Entity>> addedEntities() const;

    ///
    /// \brief Returns entities that were removed from the destination.
    ///
    /// \return Entities removed from the destination.
    ///
    std::set<std::shared_ptr<data::entity::Entity>> removedEntities() const;

    ///
    /// \brief Prepares widget to operate by giving necessary data.
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

    ///
    /// \brief Returns list of entity ids in the destination table.
    ///
    /// \return  list of entity ids in the destination table.
    ///
    QList<QVariant> destinationIds() const;

signals:
    ///
    /// \brief Emitted whenever list of added entities changes.
    /// \see addedEntities()
    /// \param entityIdList The list of all added entities ids.
    ///
    void addedEntitiesChanged(QList<quint64> entityIdList) const;

    ///
    /// \brief Emitted whenever list of removed entities changes.
    /// \see removedEntities()
    /// \param entityIdList The list of all removed entities ids.
    ///
    void removedEntitiesChanged(QList<quint64> entityIdList) const;

    ///
    /// \brief Emited whenever destination data vector is changed.
    /// \see mDestinationData
    ///
    /// \param entityIdList The list of entity ids from \ref mDestinationData.
    ///
    void destinationEntitiesChanged(QVariantList entityIdList) const;

private slots:
    ///
    /// \brief The slot to be executed when add button is clicked.
    ///
    void addButtonClicked();
    ///
    /// \brief The slot to be executed when remove button is clicked.
    ///
    void removeButtonClicked();
    ///
    /// \brief The slot to be executed when reset button is clicked.
    ///
    void resetButtonClicked();

private:
    Ui::AddRemoveEntitiesForm *ui;
    /// Defines whether whis form is read only.
    bool mReadOnly;
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
    /// The list of entity ids in destination table/model.
    QList<QVariant> mDestinationIds;

    ///
    /// \brief A helper function for preparing data for emitting \ref
    /// addedEntitiesChanged() signal.
    ///
    /// \return List of entity ids from \ref mAddedEntities
    ///
    QList<quint64> addedEntitiesChangedHelper();

    ///
    /// \brief A helper function for preparing data for emitting \ref
    /// removedEntitiesChanged() signal.
    ///
    /// \return List of entity ids from \ref mRemovedEntities
    ///
    QList<quint64> removedEntitiesChangedHelper();

    ///
    /// \brief A helper function that updates \ref mDestinationIds whenever
    /// destination table/model data changes.
    ///
    void updateDestinationIds();
};
}
}

#endif // ADDREMOVEENTITIESFORM_H
