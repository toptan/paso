#ifndef ABSTRACTFORM_H
#define ABSTRACTFORM_H

#include "recordeditorwidget.h"

#include <utility>
#include <QList>
#include <QWidget>

class QAction;
class QSqlTableModel;
class QTableView;

namespace paso {
namespace admin {

///
/// \brief The AbstractForm class is a base abstract class for all forms that
/// have toolbar actions.
///
class AbstractForm : public QWidget {
    Q_OBJECT
public:
    explicit AbstractForm(
        std::pair<QSqlTableModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);
    virtual ~AbstractForm() {}

    ///
    /// \brief toolBarActions returns reference to the list with actions for the
    /// toolbar.
    /// \return reference to the list with actions.
    ///
    QList<QAction *> &toolBarActions() { return mActions; }

    QSqlTableModel *model() const;
    RecordEditorWidget *recordEditor() const;

protected slots:
    virtual void onNewRecord();
    virtual void onEditRecord();
    virtual void onDeleteRecord();

    virtual void onEditFinished();

    virtual void onRequestSave(QSqlRecord record);
    virtual void onRequestUpdate(QSqlRecord record);
    virtual void onSelectionChanged(const QSqlRecord &record);

protected:
    ///
    /// \brief setupWidgets Sets up all necessary widgets and standard actions
    /// for operation and connects signals and slots.
    /// \param tableView The table view that displays data.
    ///
    void setupWidgets(QTableView *tableView);

    ///
    /// \brief prepareRecordForSaving Called before any save or update operation
    /// to do final modifications on record to be saved before actual save.
    /// \param record The record to prepare
    ///
    virtual void prepareRecordForSaving(QSqlRecord &record) = 0;

    ///
    /// \brief shouldEnableEditAction Is called when selection is changed to
    /// determine whether edit record action should be enabled or not.
    /// \param record The newly selected record.
    /// \return Whether to enable edit action.
    ///
    virtual bool shouldEnableEditAction(const QSqlRecord &record) const = 0;

    ///
    /// \brief shouldEnableDeleteAction is called when selection is changed to
    /// determine whether delete record action should be enabled or not.
    /// \param record The newly selected record.
    /// \return Whether to enable delete action.
    ///
    virtual bool shouldEnableDeleteAction(const QSqlRecord &record) const = 0;

    ///
    /// \brief shouldDeleteRecord determines whether given record should be
    /// deleted. Derived classes would usually present a user with message box.
    /// \param record The record in question.
    /// \return Whether the record should be deleted.
    ///
    virtual bool shouldDeleteRecord(const QSqlRecord &record) const = 0;

private:
    QList<QAction *> mActions;
    QAction *mNewRecordAction;
    QAction *mEditRecordAction;
    QAction *mDeleteRecordAction;
    QAction *mRefreshAction;

    QSqlTableModel *mModel;
    RecordEditorWidget *mRecordEditor;
    QTableView *mTableView;
};
}
}

#endif // ABSTRACTFORM_H
