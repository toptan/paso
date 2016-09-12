#ifndef STUDENTFORM_H
#define STUDENTFORM_H

#include "queryform.h"
#include "studenteditorwidget.h"
#include "studentvalidator.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class StudentForm;
}

namespace paso {
namespace admin {

///
/// \brief The StudentForm class is a widget for students administration.
///
class StudentForm : public widget::QueryForm {
    Q_OBJECT

public:
    ///
    /// \brief Contructs the form with given parent.
    /// \param parent The parent widget.
    ///
    explicit StudentForm(QWidget *parent = 0);
    ///
    /// \brief Destructor.
    ///
    virtual ~StudentForm();

public slots:
    ///
    /// \brief The slot that get called after user selects file for importing
    /// student data to perform actual import.
    /// \param fileName The file to use.
    ///
    void onImportFileSelected(const QString &fileName);

signals:
    ///
    /// \brief This signal is emited for each imported line during student data
    /// import to update log dialog.
    /// \param line The log line to append.
    ///
    void newLogLine(const QString &line);
    ///
    /// \brief This signal is emited when import is done.
    ///
    void importDone();

protected:
    /// \copydoc widget::AbstractForm::prepareRecordForSaving()
    virtual void prepareRecordForSaving(QSqlRecord &record) override;

    /// \copydoc widget::AbstractForm::shouldEnableEditAction()
    virtual bool
    shouldEnableEditAction(const QSqlRecord &record) const override;

    /// \copydoc widget::AbstractForm::shouldEnableDeleteAction()
    virtual bool
    shouldEnableDeleteAction(const QSqlRecord &record) const override;

    /// \copydoc widget::AbstractForm::shouldDeleteRecord()
    virtual bool shouldDeleteRecord(const QSqlRecord &record) const override;

    /// \copydoc widget::AbstractForm::updateActions()
    virtual void updateActions(const QSqlRecord &record) override;

    /// \copydoc widget::AbstractForm::insertRecord()
    virtual bool insertRecord(QSqlRecord &record, QSqlError &error) override;

    /// \copydoc widget::AbstractForm::updateRecord()
    virtual bool updateRecord(int row, const QSqlRecord &record,
                              QSqlError &error) override;

    /// \copydoc widget::AbstractForm::removeRow()
    virtual bool removeRow(int row, QSqlError &error) override;

private slots:
    ///
    /// \brief This slot is called after triggering import action to start data
    /// import process.
    ///
    void onImport();
    ///
    /// \brief This slot is called after triggering details action to display
    /// dialog with student details.
    ///
    void onDetails();
    ///
    /// \brief This slot is called after triggering report action to display
    /// student's room entry report.
    ///
    void onReport();

private:
    Ui::StudentForm *ui;     //!< The generated form object.
    QAction *mImportAction;  //!< The import student data action.
    QAction *mDetailsAction; //!< The student details action.
    QAction *mReportAction;  //!< The room entry report action.

    ///
    /// \brief Creates underlying table model, record editor and returns them as
    /// a pair.
    /// \return Pair of table model and editor widget.
    ///
    static std::pair<QSqlQueryModel *, widget::RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // STUDENTFORM_H
