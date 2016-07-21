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

    mDestinationModel = new EntityTableModel(
        QStringList(), QMap<QString, QString>(), EntityVector(), this);
    mDestinationProxyModel = new StableRowNumberSortFilterProxyModel(this);
    mDestinationProxyModel->setSortLocaleAware(true);
    mDestinationProxyModel->setDynamicSortFilter(true);
    mDestinationProxyModel->setSourceModel(mDestinationModel);
    ui->destinationTableView->setModel(mDestinationProxyModel);
    connect(ui->addButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::addButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::removeButtonClicked);
    connect(ui->resetButton, &QPushButton::clicked, this,
            &AddRemoveEntitiesForm::resetButtonClicked);
}

AddRemoveEntitiesForm::~AddRemoveEntitiesForm() { delete ui; }

bool AddRemoveEntitiesForm::dirty() const {
    return !mAddedEntities.empty() || !mRemovedEntities.empty();
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
    mSourceModel->setData(sourceColumns, sourceColumnNames, sourceData);
    ui->sourceTotalLabel->setVisible(totalLabelsVisible);
    ui->sourceTotalLabel->setText(
        QString(tr("Total %1")).arg(mSourceModel->rowCount()));

    ui->destinationGroupBox->setTitle(destinationLabel);
    mDestinationData = destinationData;
    mDestinationModel->setData(destinationColumns, destinationColumnNames,
                               destinationData);
    ui->destinationTotalLabel->setVisible(totalLabelsVisible);
    ui->destinationTotalLabel->setText(
        QString(tr("Total %1")).arg(mDestinationModel->rowCount()));
}

void AddRemoveEntitiesForm::addButtonClicked() {
    auto indexes = ui->sourceTableView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return;
    }
    for (const auto &index : indexes) {
        auto modelRow = mSourceProxyModel->mapToSource(index).row();
        auto entity = mSourceModel->entity(modelRow);
        mAddedEntities.insert(entity);
        mRemovedEntities.erase(entity);
        mDestinationModel->insertEntity(-1, entity);
        mSourceModel->removeEntity(modelRow);
    }
}

void AddRemoveEntitiesForm::removeButtonClicked() {
    auto indexes = ui->destinationTableView->selectionModel()->selectedRows();
    if (indexes.isEmpty()) {
        return;
    }
    for (const auto &index : indexes) {
        auto modelRow = mDestinationProxyModel->mapToSource(index).row();
        auto entity = mDestinationModel->entity(modelRow);
        mAddedEntities.erase(entity);
        mRemovedEntities.insert(entity);
        mSourceModel->insertEntity(-1, entity);
        mDestinationModel->removeEntity(modelRow);
    }
}

void AddRemoveEntitiesForm::resetButtonClicked() {
    mAddedEntities.clear();
    mRemovedEntities.clear();
    mSourceModel->setData(mSourceData);
    mDestinationModel->setData(mDestinationData);
}
}
}
