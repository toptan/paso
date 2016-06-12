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

    virtual void onEditFinished();

    virtual void onRequestSave(QSqlRecord record);
    virtual void onRequestUpdate(QSqlRecord record);
    virtual void onSelectionChanged(const QSqlRecord &record) = 0;

protected:
    ///
    /// \brief setupWidgets Sets up all necessary widgets for operation and
    /// connects signals and slots. Needs
    /// to be called from derived class.
    /// \param tableView The table view that displays data.
    ///
    void setupWidgets(QTableView *tableView);

    ///
    /// \brief prepareRecordForSaving Called before any save or update operation
    /// to do final modifications on record to be saved before actual save.
    /// \param record The record to prepare
    ///
    virtual void prepareRecordForSaving(QSqlRecord &record) = 0;

private:
    QList<QAction *> mActions;
    QAction *mNewRecordAction;
    QSqlTableModel *mModel;
    RecordEditorWidget *mRecordEditor;
    QTableView *mTableView;
};
}
}

#endif // ABSTRACTFORM_H
