#include "abstractform.h"

#include "refreshablesqlquerymodel.h"
#include "stablerownumbersortfilterproxymodel.h"

#include <QAction>
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>

using namespace paso::model;
using namespace paso::db;

namespace paso {
namespace widget {

AbstractForm::AbstractForm(
    std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
    QWidget *parent)
    : QWidget(parent), mManager(DEFAULT_DB_NAME), mActions(),
      mModel(modelAndEditor.first), mProxyModel(nullptr),
      mRecordEditor(modelAndEditor.second), mTableView(nullptr) {
    mModel->setParent(this);
    mRecordEditor->setParent(this);
}

AbstractForm::~AbstractForm() {}

QSqlQueryModel *AbstractForm::model() const { return mModel; }

RecordEditorWidget *AbstractForm::recordEditor() const { return mRecordEditor; }

void AbstractForm::setupWidgets(QTableView *tableView) {
    mTableView = tableView;

    mProxyModel = new StableRowNumberSortFilterProxyModel(this);
    mProxyModel->setSourceModel(mModel);
    mProxyModel->setSortLocaleAware(true);
    mProxyModel->setDynamicSortFilter(false);

    mTableView->setModel(mProxyModel);
    mTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTableView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::ResizeToContents);
    mTableView->horizontalHeader()->setCascadingSectionResizes(true);
    mTableView->horizontalHeader()->setStretchLastSection(true);
    mTableView->sortByColumn(1, Qt::SortOrder::AscendingOrder);

    connect(mTableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &selected, const QModelIndex &) {
                auto record = mModel->record(mapRowToModel(selected.row()));
                this->onSelectionChanged(record);
                mTableView->selectRow(selected.row());
            });

    connect(mRecordEditor, &RecordEditorWidget::editFinished, this,
            &AbstractForm::onEditFinished);
    connect(mRecordEditor, &RecordEditorWidget::requestSave, this,
            &AbstractForm::onRequestSave);
    connect(mRecordEditor, &RecordEditorWidget::requestUpdate, this,
            &AbstractForm::onRequestUpdate);

    mNewRecordAction = new QAction(tr("New"), this);
    mNewRecordAction->setObjectName("NEW_RECORD_ACTION");
    connect(mNewRecordAction, &QAction::triggered, this,
            &AbstractForm::onNewRecord);
    mActions.push_back(mNewRecordAction);

    mEditRecordAction = new QAction(tr("Edit"), this);
    mEditRecordAction->setObjectName("EDIT_RECORD_ACTION");
    mEditRecordAction->setEnabled(false);
    connect(mEditRecordAction, &QAction::triggered, this,
            &AbstractForm::onEditRecord);
    mActions.push_back(mEditRecordAction);

    mDeleteRecordAction = new QAction(tr("Delete"), this);
    mDeleteRecordAction->setObjectName("DELETE_RECORD_ACTION");
    mDeleteRecordAction->setEnabled(false);
    connect(mDeleteRecordAction, &QAction::triggered, this,
            &AbstractForm::onDeleteRecord);
    mActions.push_back(mDeleteRecordAction);

    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    mActions.push_back(separator);

    mRefreshAction = new QAction(tr("Refresh data"), this);
    mRefreshAction->setObjectName("REFRESH_ACTION");
    connect(mRefreshAction, &QAction::triggered, this,
            &AbstractForm::onRefreshData);
    mActions.push_back(mRefreshAction);
}

void AbstractForm::onNewRecord() {
    mTableView->setDisabled(true);
    for (auto &action : mActions) {
        action->setEnabled(false);
    }
    mRecordEditor->onEditNewRecord(mModel->record());
}

void AbstractForm::onEditRecord() {
    auto index = mTableView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        return;
    }

    mTableView->setDisabled(true);
    for (auto &action : mActions) {
        action->setEnabled(false);
    }

    mRecordEditor->onEditExistingRecord(
        mModel->record(mapRowToModel(index.row())));
}

void AbstractForm::onDeleteRecord() {
    auto index = mTableView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        return;
    }
    auto row = mapRowToModel(index.row());
    if (!shouldDeleteRecord(mModel->record(row))) {
        return;
    }
    QSqlError error;
    if (!removeRow(row, error)) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while deleting record from the "
                          "database."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }
    refreshModel();
    mRecordEditor->clearData();
    mEditRecordAction->setEnabled(false);
    mDeleteRecordAction->setEnabled(false);
    updateActions(mModel->record());
}

void AbstractForm::onRefreshData() {
    auto index = mTableView->selectionModel()->currentIndex();
    bool selectionExists = index.isValid();
    QSqlRecord selectedRecord;
    if (selectionExists) {
        selectedRecord = mModel->record(mapRowToModel(index.row()));
    }
    refreshModel();
    int row = selectionExists ? findRecord(selectedRecord) : -1;
    if (row != -1) {
        mTableView->selectRow(row);
        onSelectionChanged(selectedRecord);
    } else {
        mTableView->clearSelection();
        mEditRecordAction->setEnabled(false);
        mDeleteRecordAction->setEnabled(false);
        mRecordEditor->clearData();
        updateActions(mModel->record());
    }
}

void AbstractForm::onEditFinished() {
    for (auto &action : mActions) {
        action->setEnabled(true);
    }
    mTableView->setDisabled(false);
    mTableView->setFocus();
    mDeleteRecordAction->setEnabled(false);
    mEditRecordAction->setEnabled(false);
}

void AbstractForm::onRequestSave(QSqlRecord record) {
    prepareRecordForSaving(record);
    QSqlError error;
    if (!insertRecord(record, error)) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while saving data."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        refreshModel();
        return;
    }

    mRecordEditor->saveSuccessfull();
    mRecordEditor->clearData();
    mTableView->selectRow(findRecord(record));
    onEditFinished();
    onSelectionChanged(record);
}

void AbstractForm::onRequestUpdate(QSqlRecord record) {
    prepareRecordForSaving(record);
    auto index = mTableView->selectionModel()->currentIndex();
    QSqlError error;
    auto row = mapRowToModel(index.row());
    if (!updateRecord(row, record, error)) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while saving data."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        refreshModel();
        mTableView->selectRow(index.row());
        return;
    }

    mRecordEditor->saveSuccessfull();
    mRecordEditor->clearData();
    mTableView->selectRow(findRecord(record));
    onEditFinished();
    onSelectionChanged(record);
}

void AbstractForm::onSelectionChanged(const QSqlRecord &record) {
    mDeleteRecordAction->setEnabled(shouldEnableDeleteAction(record));
    mEditRecordAction->setEnabled(shouldEnableEditAction(record));
    mRecordEditor->onDisplayRecord(record);
    updateActions(record);
}

void AbstractForm::refreshModel() {
    auto tableModel = dynamic_cast<QSqlTableModel *>(mModel);
    if (tableModel != nullptr) {
        tableModel->select();
    }
    auto refreshableModel = dynamic_cast<RefreshableSqlQueryModel *>(mModel);
    if (refreshableModel != nullptr) {
        refreshableModel->select();
    }
}

QSqlRecord AbstractForm::selectedRecord() const {
    auto index = mTableView->currentIndex();
    if (!index.isValid()) {
        return QSqlRecord();
    }
    return mModel->record(mapRowToModel(index.row()));
}

inline int AbstractForm::mapRowToModel(int proxyRow) const {
    return mProxyModel->mapToSource(mProxyModel->index(proxyRow, 0)).row();
}

inline int AbstractForm::mapRowToProxy(int modelRow) const {
    return mProxyModel->mapFromSource(mModel->index(modelRow, 0)).row();
}

int AbstractForm::findRecord(const QSqlRecord &record) const {
    for (int i = 0; i < mModel->rowCount(); i++) {
        if (record == mModel->record(i)) {
            return mapRowToProxy(i);
        }
    }
    return -1;
}

DBManager &AbstractForm::manager() { return mManager; }
}
}
