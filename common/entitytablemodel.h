#ifndef ENTITYTABLEMODEL_H
#define ENTITYTABLEMODEL_H

#include "entity.h"

#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>
#include <functional>
#include <memory>
#include <vector>

namespace paso {
namespace model {

///
/// \brief The EntityTableModel class is a model that is capable of handling
/// entities as data. Entity properties are displayed as columns.
///
class EntityTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    ///
    /// \brief Constructs model with given columns, column names and data.
    /// \param columns The properties to display.
    /// \param columnNames The column names for header.
    /// \param data The vector of entities to display.
    /// \param parent The parent object.
    ///
    EntityTableModel(const QStringList &columns,
                     const QMap<QString, QString> &columnNames,
                     const data::entity::EntityVector &data,
                     QObject *parent = nullptr);

    ///
    /// \brief Returns column count for given parent index.
    /// \param parent The parent index.
    /// \return The column count.
    ///
    virtual int
    columnCount(const QModelIndex &parent = QModelIndex()) const override;

    ///
    /// \brief Returns row count for given parent index.
    /// \param parent The parent index.
    /// \return The column count.
    ///
    virtual int
    rowCount(const QModelIndex &parent = QModelIndex()) const override;

    ///
    /// \brief Returns data neccessary to display cell defined by given index.
    /// \param index The cell index.
    /// \param role The display role.
    /// \return Variant containing neccessary data to display cell properly.
    ///
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const override;

    ///
    /// \brief Returns data neccessary to display header section.
    /// \param section The section to display.
    /// \param orientation The header orientation.
    /// \param role The display role.
    /// \return Variant containing neccessary data to display header section
    /// properly.
    ///
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;

    ///
    /// \brief Returns vector of entities that represents underlying data.
    /// \return Underlying entities data.
    ///
    data::entity::EntityVector data() const;

    ///
    /// \brief Returns entity at given position in data vector.
    /// \param position Position in the data vector.
    /// \return Entity at requested position.
    ///
    std::shared_ptr<paso::data::entity::Entity> entity(size_t position) const;

    ///
    /// \brief Inserts entity at requested position. If position is bigger than
    /// the lenght of the data vector, entity is appended.
    /// \param position The insert position.
    /// \param entity The entity to insert.
    ///
    void insertEntity(size_t position,
                      std::shared_ptr<paso::data::entity::Entity> entity);

    ///
    /// \brief Removes entity from given position. If position is not valid,
    /// nothing will happen.
    /// \param position The position from which to remove entity.
    ///
    void removeEntity(size_t position);

    ///
    /// \brief Removes given entity from underlying data vector.
    /// \param entity The entity to remove.
    ///
    void
    removeEntity(const std::shared_ptr<paso::data::entity::Entity> &entity);

    ///
    /// \brief Sets new data vector.
    /// \param newData New data.
    ///
    void setEntityData(const data::entity::EntityVector &newData);

    ///
    /// \brief Sets new data vector alongside with properties to display and
    /// column names for the header.
    /// \param columns The properties to display.
    /// \param columnNames The column names for the header.
    /// \param newData New data.
    ///
    void setEntityData(const QStringList &columns,
                       const QMap<QString, QString> &columnNames,
                       const data::entity::EntityVector &newData);

signals:
    ///
    /// \brief This signal is emited when length of undelying data vector
    /// changes.
    /// \param newRowCount The new length of the data vector.
    ///
    void rowCountChanged(size_t newRowCount) const;

private:
    QStringList mColumns;                //!< The entity properties to display.
    QMap<QString, QString> mColumnNames; //!< Column names for the header.
    data::entity::EntityVector mData;    //!< The underlying data vector.
};
}
}

#endif // ENTITYTABLEMODEL_H
