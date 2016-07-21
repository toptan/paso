#include "addremoveentitiesform.h"
#include "ui_addremoveentitiesform.h"

#include <algorithm>

using namespace std;
using namespace paso::data::entity;
using namespace paso::model;

namespace paso {
namespace widget {

AddRemoveEntitiesForm::AddRemoveEntitiesForm(QWidget *parent)
    : QWidget(parent), ui(new Ui::AddRemoveEntitiesForm), mDirty(false),
      mSourceData(nullptr), mDestinationData(nullptr), mAddedEntities(nullptr),
      mRemovedEntities(nullptr), mSourceModel(nullptr),
      mDestinationModel(nullptr), mSourceProxyModel(nullptr),
      mDestinationProxyModel(nullptr) {
    ui->setupUi(this);

    mSourceProxyModel = new StableRowNumberSortFilterProxyModel(this);
    mSourceProxyModel->setSortLocaleAware(true);
    mSourceProxyModel->setDynamicSortFilter(false);

    mDestinationProxyModel = new StableRowNumberSortFilterProxyModel(this);
    mDestinationProxyModel->setSortLocaleAware(true);
    mDestinationProxyModel->setDynamicSortFilter(true);

    connect(ui->addButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::addButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::removeButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::resetButtonClicked);
}

AddRemoveEntitiesForm::~AddRemoveEntitiesForm() { delete ui; }

bool AddRemoveEntitiesForm::dirty() const { return mDirty; }

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
    const EntityVector &destinationData) {
    ui->sourceGroupBox->setTitle(sourceLabel);
    mSourceData = sourceData;
    mSourceModel = new EntityTableModel(sourceColumns, sourceColumnNames,
                                        sourceData, this);
    mSourceProxyModel->setSourceModel(mSourceModel);

    ui->destinationGroupBox->setTitle(sourceLabel);
    mDestinationData = destinationData;
    mDestinationModel = new EntityTableModel(
        destinationColumns, destinationColumnNames, destinationData, this);
    mDestinationProxyModel->setSourceModel(mDestinationModel);
}

void AddRemoveEntitiesForm::addButtonClicked() {
    auto indexes = ui->sourceTableView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return;
    }
    for (const auto &index : indexes) {
        auto modelRow = mSourceProxyModel->mapToSource(index).row();
        auto entity = mSourceModel->entity(modelRow);
        mAddedEntities->push_back(entity);
        mRemovedEntities->
        mDestinationModel->insertEntity(-1, entity);
        mSourceModel->removeEntity(modelRow);
        mDirty = true;
    }
}

void AddRemoveEntitiesForm::removeButtonClicked() {
    auto indexes = ui->sourceTableView->selectionModel()->selectedIndexes();
    if (indexes.isEmpty()) {
        return;
    }
    for (const auto &index : indexes) {
        auto modelRow = mSourceProxyModel->mapToSource(index).row();
        auto entity = mSourceModel->entity(modelRow);
        if (std::find(mAddedEntities->begin(), mAddedEntities->end(), entity) !=
            mAddedEntities->end()) {
            continue;
        }
        mAddedEntities->push_back(entity);
        mDestinationModel->insertEntity(-1, entity);
        mSourceModel->removeEntity(modelRow);
        mDirty = true;
    }
}

void AddRemoveEntitiesForm::resetButtonClicked() {}
}
}
