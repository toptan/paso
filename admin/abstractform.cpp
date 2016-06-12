#include "abstractform.h"

#include <QAction>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlTableModel>
#include <QTableView>

namespace paso {
namespace admin {

AbstractForm::AbstractForm(
    std::pair<QSqlTableModel *, RecordEditorWidget *> modelAndEditor,
    QWidget *parent)
    : QWidget(parent), mActions(), mModel(modelAndEditor.first),
      mRecordEditor(modelAndEditor.second), mTableView(nullptr) {
    mModel->setParent(this);
    mRecordEditor->setParent(this);
}

QSqlTableModel *AbstractForm::model() const { return mModel; }

RecordEditorWidget *AbstractForm::recordEditor() const { return mRecordEditor; }

void AbstractForm::setupWidgets(QTableView *tableView) {
    mTableView = tableView;
    mTableView->setModel(mModel);
    mTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
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
}

void AbstractForm::onNewRecord() {
    mTableView->setDisabled(true);
    for (auto &action : mActions) {
        action->setEnabled(false);
    }
    mRecordEditor->onEditNewRecord(mModel->record());
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
    }
}

void AbstractForm::onRequestSave(QSqlRecord record) {
    prepareRecordForSaving(record);
    mModel->insertRecord(-1, record);
    if (!mModel->submitAll()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while saving data."));
        msgBox.setDetailedText(mModel->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        mModel->select();
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
    mModel->setRecord(index.row(), record);
    if (mModel->submitAll()) {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("There was an error while saving data."));
        msgBox.setDetailedText(model()->lastError().text());
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        mModel->select();
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
}
}
