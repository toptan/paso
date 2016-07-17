#ifndef ABSTRACTFORM_H
#define ABSTRACTFORM_H

#include "recordeditorwidget.h"

#include <utility>
#include <QList>
#include <QWidget>

class QAction;
class QSqlQueryModel;
class QTableView;
class QSortFilterProxyModel;
class QSqlError;

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
        std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);
    virtual ~AbstractForm();

    ///
    /// \brief toolBarActions returns reference to the list with actions for the
    /// toolbar.
    /// \return reference to the list with actions.
    ///
    QList<QAction *> &toolBarActions() { return mActions; }

    QSqlQueryModel *model() const;
    RecordEditorWidget *recordEditor() const;

protected slots:
    virtual void onNewRecord();
    virtual void onEditRecord();
    virtual void onDeleteRecord();
    virtual void onRefreshData();

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
    /// \brief refreshModel Refreshes the data model.
    ///
    void refreshModel();

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

    ///
    /// \brief updateActions is called whenever selection changes to update form
    /// specific actions.
    /// \param record The newly selected record.
    ///
    virtual void updateActions(const QSqlRecord &record) = 0;

    ///
    /// \brief removeRow deletes the row from model/table view.
    /// \param row row to remove.
    /// \param error the SQL error if any.
    /// \return \c true if removal was successful.
    ///
    virtual bool removeRow(int row, QSqlError &error) = 0;

    ///
    /// \brief insertRecord inserts a new record. The record will contain
    /// updated values after saving like primary keys, etc.
    /// \param record The record to insert.
    /// \param error The SQL error if any.
    /// \return \c true if insert vas successfull.
    ///
    virtual bool insertRecord(QSqlRecord &record, QSqlError &error) = 0;

    ///
    /// \brief updateRecord Updates the record at given row.
    /// \param row The row.
    /// \param record The record.
    /// \param error The SQL error if any.
    /// \return \c true if update was successfull
    ///
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) = 0;

private:
    QList<QAction *> mActions;
    QAction *mNewRecordAction;
    QAction *mEditRecordAction;
    QAction *mDeleteRecordAction;
    QAction *mRefreshAction;

    QSqlQueryModel *mModel;
    QSortFilterProxyModel *mProxyModel;

    RecordEditorWidget *mRecordEditor;
    QTableView *mTableView;

    ///
    /// \brief findRecord Finds record in underlying model and returns the row
    /// for table view selection. If record is not found -1 is returned.
    /// \param record Record to find.
    /// \return Row to select in table view.
    ///
    int findRecord(const QSqlRecord &record) const;

    ///
    /// \brief mapRowToModel Maps proxy row to model row. If proxy does not
    /// exist the same value is returned.
    /// \param proxyRow Proxy row to map to model row.
    /// \return Mapped model row.
    ///
    int mapRowToModel(int proxyRow) const;

    ///
    /// \brief mapRowToProxy Maps model row to proxy row. If proxy does not
    /// exist the same value is returned.
    /// \param modelRow Model row to map to proxy row.
    /// \return Mapped proxy row.
    ///
    int mapRowToProxy(int modelRow) const;
};
}
}

#endif // ABSTRACTFORM_H
