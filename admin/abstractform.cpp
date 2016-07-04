#include "abstractform.h"

#include <QAction>
#include <QDebug>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QTableView>

namespace paso {
namespace admin {

AbstractForm::AbstractForm(
    std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
    QWidget *parent)
    : QWidget(parent), mActions(), mModel(modelAndEditor.first),
      mRecordEditor(modelAndEditor.second), mTableView(nullptr) {
    mModel->setParent(this);
    mRecordEditor->setParent(this);
}

AbstractForm::~AbstractForm() {}

QSqlQueryModel *AbstractForm::model() const { return mModel; }

RecordEditorWidget *AbstractForm::recordEditor() const { return mRecordEditor; }

void AbstractForm::setupWidgets(QTableView *tableView) {
    mTableView = tableView;
    mTableView->setModel(mModel);
    mTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mTableView->sortByColumn(1, Qt::SortOrder::AscendingOrder);

    connect(mTableView->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            [this](const QModelIndex &selected, const QModelIndex &) {
                auto record = mModel->record(selected.row());
                this->onSelectionChanged(record);
            });

    connect(mRecordEditor, &RecordEditorWidget::editFinished, this,
            &AbstractForm::onEditFinished);
    connect(mRecordEditor, &RecordEditorWidget::requestSave, this,
            &AbstractForm::onRequestSave);
    connect(mRecordEditor, &RecordEditorWidget::requestUpdate, this,
            &AbstractForm::onRequestUpdate);

    mNewRecordAction = new QAction(tr("New"), this);
    connect(mNewRecordAction, &QAction::triggered, this,
            &AbstractForm::onNewRecord);
    mActions.push_back(mNewRecordAction);

    mEditRecordAction = new QAction(tr("Edit"), this);
    mEditRecordAction->setEnabled(false);
    connect(mEditRecordAction, &QAction::triggered, this,
            &AbstractForm::onEditRecord);
    mActions.push_back(mEditRecordAction);

    mDeleteRecordAction = new QAction(tr("Delete"), this);
    mDeleteRecordAction->setEnabled(false);
    connect(mDeleteRecordAction, &QAction::triggered, this,
            &AbstractForm::onDeleteRecord);
    mActions.push_back(mDeleteRecordAction);

    QAction *separator = new QAction(this);
    separator->setSeparator(true);
    mActions.push_back(separator);

    mRefreshAction = new QAction(tr("Refresh data"), this);
    connect(mRefreshAction, &QAction::triggered, [this]() { refreshModel(); });
    connect(mRefreshAction, &QAction::triggered, [this]() {
        mTableView->clearSelection();
        mEditRecordAction->setEnabled(false);
        mDeleteRecordAction->setEnabled(false);
    });
    connect(mRefreshAction, &QAction::triggered, mRecordEditor,
            &RecordEditorWidget::clearData);
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

    mRecordEditor->onEditExistingRecord(mModel->record(index.row()));
}

void AbstractForm::onDeleteRecord() {
    auto index = mTableView->selectionModel()->currentIndex();
    if (!index.isValid()) {
        return;
    }
    if (!shouldDeleteRecord(mModel->record(index.row()))) {
        return;
    }
    QSqlError error;
    if (!removeRow(index.row(), error)) {
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
}

void AbstractForm::onEditFinished() {
    for (auto &action : mActions) {
        action->setEnabled(true);
    }
    mTableView->setDisabled(false);
    mTableView->setFocus();
    auto index = mTableView->selectionModel()->currentIndex();
    if (index.isValid()) {
        onSelectionChanged(mModel->record(index.row()));
    } else {
        mDeleteRecordAction->setEnabled(false);
        mEditRecordAction->setEnabled(false);
    }
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
        mRecordEditor->saveError();
        return;
    }

    mRecordEditor->saveSuccessfull();
    mTableView->clearSelection();
    mRecordEditor->clearData();
    onEditFinished();
}

void AbstractForm::onRequestUpdate(QSqlRecord record) {
    prepareRecordForSaving(record);
    auto index = mTableView->selectionModel()->currentIndex();
    QSqlError error;
    if (!updateRecord(index.row(), record, error)) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while saving data."));
        msgBox.setDetailedText(error.text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        refreshModel();
        mTableView->selectRow(index.row());
        mRecordEditor->saveError();
        return;
    }

    recordEditor()->saveSuccessfull();
    mTableView->clearSelection();
    mTableView->selectRow(index.row());
    onEditFinished();
    onSelectionChanged(
        mModel->record(mTableView->selectionModel()->currentIndex().row()));
}

void AbstractForm::onSelectionChanged(const QSqlRecord &record) {
    mDeleteRecordAction->setEnabled(shouldEnableDeleteAction(record));
    mEditRecordAction->setEnabled(shouldEnableEditAction(record));
    mRecordEditor->onDisplayRecord(record);
}

void AbstractForm::refreshModel() {
    auto model = dynamic_cast<QSqlTableModel *>(mModel);
    if (model != nullptr) {
        model->select();
    } else {
        mModel->query().exec();
    }
}
}
}
