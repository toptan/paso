#ifndef ABSTRACTFORM_H
#define ABSTRACTFORM_H

#include "pasodb.h"
#include "recordeditorwidget.h"

#include <QList>
#include <QWidget>
#include <utility>

class QAction;
class QSqlQueryModel;
class QTableView;
class QSortFilterProxyModel;
class QSqlError;

namespace paso {
namespace widget {

///
/// \brief The AbstractForm class is a base abstract class for all forms that
/// have toolbar actions.
///
class AbstractForm : public QWidget {
    Q_OBJECT
public:
    ///
    /// \brief Contructs new abstract form with given model and editor.
    /// \param modelAndEditor The data model and editor to use.
    /// \param parent The parent widget.
    ///
    explicit AbstractForm(
        std::pair<QSqlQueryModel *, RecordEditorWidget *> modelAndEditor,
        QWidget *parent = nullptr);

    ///
    /// \brief Destructor.
    ///
    virtual ~AbstractForm();

    ///
    /// \brief Returns reference to the list with actions for the toolbar.
    /// \return reference to the list with actions.
    ///
    QList<QAction *> &toolBarActions() { return mActions; }

    ///
    /// \brief Returns SQL model that the form uses.
    /// \return The SQL model.
    ///
    QSqlQueryModel *model() const;

    ///
    /// \brief Returns record editor widget attached to this form.
    /// \return The record editor widget.
    ///
    RecordEditorWidget *recordEditor() const;

protected slots:
    ///
    /// \brief Called when new record entry is requested.
    ///
    virtual void onNewRecord();
    ///
    /// \brief Called when editing of an existing record is requested.
    ///
    virtual void onEditRecord();
    ///
    /// \brief Called when deletion of the record is requested.
    ///
    virtual void onDeleteRecord();
    ///
    /// \brief Called to refresh form data.
    ///
    virtual void onRefreshData();
    ///
    /// \brief Called when record editor has finished editing.
    ///
    virtual void onEditFinished();

    ///
    /// \brief Called to save new record to the database.
    /// \param record The record to save.
    ///
    virtual void onRequestSave(QSqlRecord record);

    ///
    /// \brief Called to save updated record to the database.
    /// \param record The record to save.
    ///
    virtual void onRequestUpdate(QSqlRecord record);

    ///
    /// \brief Called whenever selection in the table changed.
    /// \param record The newly selected record.
    ///
    virtual void onSelectionChanged(const QSqlRecord &record);

protected:
    ///
    /// \brief Sets up all necessary widgets and standard actions
    /// for operation and connects signals and slots.
    /// \param tableView The table view that displays data.
    ///
    void setupWidgets(QTableView *tableView);

    ///
    /// \brief Refreshes the data model.
    ///
    void refreshModel();

    ///
    /// \brief Called before any save or update operation
    /// to do final modifications on record to be saved before actual save.
    /// \param record The record to prepare for saving.
    ///
    virtual void prepareRecordForSaving(QSqlRecord &record) = 0;

    ///
    /// \brief Called when selection is changed to
    /// determine whether edit record action should be enabled or not.
    /// \param record The newly selected record.
    /// \return Whether to enable edit action.
    ///
    virtual bool shouldEnableEditAction(const QSqlRecord &record) const = 0;

    ///
    /// \brief Called when selection is changed to
    /// determine whether delete record action should be enabled or not.
    /// \param record The newly selected record.
    /// \return Whether to enable delete action.
    ///
    virtual bool shouldEnableDeleteAction(const QSqlRecord &record) const = 0;

    ///
    /// \brief Determines whether given record should be
    /// deleted. Derived classes would usually present a user with message box.
    /// \param record The record in question.
    /// \return Whether the record should be deleted.
    ///
    virtual bool shouldDeleteRecord(const QSqlRecord &record) const = 0;

    ///
    /// \brief Called whenever selection changes to update form
    /// specific actions.
    /// \param record The newly selected record.
    ///
    virtual void updateActions(const QSqlRecord &record) = 0;

    ///
    /// \brief Deletes the row from model/table view.
    /// \param row The row to remove.
    /// \param error The SQL error if any.
    /// \return \c true if removal was successful.
    ///
    virtual bool removeRow(int row, QSqlError &error) = 0;

    ///
    /// \brief Inserts a new record. The record will contain updated values
    /// after saving like primary keys, etc.
    /// \param record The record to insert.
    /// \param error The SQL error if any.
    /// \return \c true if insert vas successfull.
    ///
    virtual bool insertRecord(QSqlRecord &record, QSqlError &error) = 0;

    ///
    /// \brief Updates the record at given row.
    /// \param row The row.
    /// \param record The record.
    /// \param error The SQL error if any.
    /// \return \c true if update was successfull.
    ///
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) = 0;

    ///
    /// \brief Returns currently selected record.
    /// \return Currently selected record.
    ///
    QSqlRecord selectedRecord() const;

    ///
    /// \brief Returns database manager to use.
    /// \return The database manager to use.
    ///
    db::DBManager &manager();

private:
    db::DBManager mManager;       //!< The database manager to use.
    QList<QAction *> mActions;    //!< List of toolbar actions.
    QAction *mNewRecordAction;    //!< New record action.
    QAction *mEditRecordAction;   //!< Edit record action.
    QAction *mDeleteRecordAction; //!< Delete record action.
    QAction *mRefreshAction;      //!< Refresh form data action.

    QSqlQueryModel *mModel;             //!< The model to use for the data.
    QSortFilterProxyModel *mProxyModel; //!< The proxy model to use for sorting.

    RecordEditorWidget *mRecordEditor; //!< The record editor widget.
    QTableView *mTableView; //!< The table view that displays the data.

    ///
    /// \brief Finds record in underlying model and returns the row for table
    /// view selection. If record is not found -1 is returned.
    /// \param record Record to find.
    /// \return Row to select in table view.
    ///
    int findRecord(const QSqlRecord &record) const;

    ///
    /// \brief Maps proxy row to model row. If proxy does not exist the same
    /// value is returned.
    /// \param proxyRow Proxy row to map to model row.
    /// \return Mapped model row.
    ///
    int mapRowToModel(int proxyRow) const;

    ///
    /// \brief Maps model row to proxy row. If proxy does not exist the same
    /// value is returned.
    /// \param modelRow Model row to map to proxy row.
    /// \return Mapped proxy row.
    ///
    int mapRowToProxy(int modelRow) const;
};
}
}

#endif // ABSTRACTFORM_H
