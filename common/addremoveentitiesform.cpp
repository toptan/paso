#include "addremoveentitiesform.h"
#include "ui_addremoveentitiesform.h"

#include <QDebug>

using namespace std;
using namespace paso::data::entity;
using namespace paso::model;

namespace paso {
namespace widget {

AddRemoveEntitiesForm::AddRemoveEntitiesForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::AddRemoveEntitiesForm), mSourceModel(nullptr),
      mDestinationModel(nullptr), mSourceProxyModel(nullptr),
      mDestinationProxyModel(nullptr) {
    ui->setupUi(this);
    mSourceModel = new EntityTableModel(QStringList(), QMap<QString, QString>(),
                                        EntityVector(), this);
    mSourceProxyModel = new StableRowNumberSortFilterProxyModel(this);
    mSourceProxyModel->setSortLocaleAware(true);
    mSourceProxyModel->setDynamicSortFilter(false);
    mSourceProxyModel->setSourceModel(mSourceModel);
    ui->sourceTableView->setModel(mSourceProxyModel);
    ui->sourceTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->sourceTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    ui->sourceTableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);

    mDestinationModel = new EntityTableModel(
        QStringList(), QMap<QString, QString>(), EntityVector(), this);
    mDestinationProxyModel = new StableRowNumberSortFilterProxyModel(this);
    mDestinationProxyModel->setSortLocaleAware(true);
    mDestinationProxyModel->setDynamicSortFilter(true);
    mDestinationProxyModel->setSourceModel(mDestinationModel);
    ui->destinationTableView->setModel(mDestinationProxyModel);
    ui->destinationTableView->setEditTriggers(
        QAbstractItemView::NoEditTriggers);
    ui->destinationTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    ui->destinationTableView->sortByColumn(0, Qt::SortOrder::AscendingOrder);

    connect(mSourceModel, &EntityTableModel::rowCountChanged,
            [this](size_t rowCount) {
                ui->sourceTotalLabel->setText(tr("Total %1").arg(rowCount));
            });
    connect(mDestinationModel, &EntityTableModel::rowCountChanged,
            [this](size_t rowCount) {
                ui->destinationTotalLabel->setText(tr("Total %1").arg(rowCount));
            });

    connect(ui->addButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::addButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::removeButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::resetButtonClicked);
}

AddRemoveEntitiesForm::~AddRemoveEntitiesForm() { delete ui; }

bool AddRemoveEntitiesForm::dirty() const {
    return !(mAddedEntities.empty() && mRemovedEntities.empty());
}

set<shared_ptr<Entity>> AddRemoveEntitiesForm::addedEntities() const {
    return mAddedEntities;
}

set<shared_ptr<Entity>> AddRemoveEntitiesForm::removedEntities() const {
    return mRemovedEntities;
}

void AddRemoveEntitiesForm::setData(
    const QString &sourceLabel, const QStringList &sourceColumns,
    const QMap<QString, QString> &sourceColumnNames,
    const EntityVector &sourceData, const QString &destinationLabel,
    const QStringList &destinationColumns,
    const QMap<QString, QString> &destinationColumnNames,
    const EntityVector &destinationData, bool totalLabelsVisible) {
    ui->sourceGroupBox->setTitle(sourceLabel);
    mSourceData = sourceData;
    mSourceModel->setEntityData(sourceColumns, sourceColumnNames, sourceData);
    ui->sourceTotalLabel->setVisible(totalLabelsVisible);
    ui->sourceTotalLabel->setText(tr("Total %1").arg(mSourceModel->rowCount()));

    ui->destinationGroupBox->setTitle(destinationLabel);
    mDestinationData = destinationData;
    mDestinationModel->setEntityData(destinationColumns, destinationColumnNames,
                               destinationData);
    ui->destinationTotalLabel->setVisible(totalLabelsVisible);
    ui->destinationTotalLabel->setText(
        tr("Total %1").arg(mDestinationModel->rowCount()));
    mAddedEntities.clear();
    mRemovedEntities.clear();
}

void AddRemoveEntitiesForm::addButtonClicked() {
    auto indexes = ui->sourceTableView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return;
    }

    vector<shared_ptr<Entity>> entitiesForRemoval;

    for (const auto &index : indexes) {
        auto modelRow = mSourceProxyModel->mapToSource(index).row();
        auto entity = mSourceModel->entity(modelRow);
        entitiesForRemoval.push_back(entity);
        mAddedEntities.insert(entity);
        mRemovedEntities.erase(entity);
        mDestinationModel->insertEntity(-1, entity);
    }

    for (const auto &entity : entitiesForRemoval) {
        mSourceModel->removeEntity(entity);
    }

    ui->sourceTableView->clearSelection();
}

void AddRemoveEntitiesForm::removeButtonClicked() {
    auto indexes = ui->destinationTableView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return;
    }

    vector<shared_ptr<Entity>> entitiesForRemoval;

    for (const auto &index : indexes) {
        auto modelRow = mDestinationProxyModel->mapToSource(index).row();
        auto entity = mDestinationModel->entity(modelRow);
        entitiesForRemoval.push_back(entity);
        mAddedEntities.erase(entity);
        mRemovedEntities.insert(entity);
        mSourceModel->insertEntity(-1, entity);
    }

    for (const auto &entity : entitiesForRemoval) {
        mDestinationModel->removeEntity(entity);
    }

    ui->destinationTableView->clearSelection();
}

void AddRemoveEntitiesForm::resetButtonClicked() {
    mAddedEntities.clear();
    mRemovedEntities.clear();
    mSourceModel->setEntityData(mSourceData);
    mDestinationModel->setEntityData(mDestinationData);
    ui->sourceTableView->clearSelection();
    ui->destinationTableView->clearSelection();
}
}
}
