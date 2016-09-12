#ifndef COURSEFORM_H
#define COURSEFORM_H

#include "courseeditorwidget.h"
#include "coursetablemodel.h"
#include "tableform.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class CourseForm;
}

namespace paso {
namespace admin {

///
/// \brief The CourseForm class is a widget for course administration.
///
class CourseForm : public widget::TableForm {
    Q_OBJECT

public:
    ///
    /// \brief Contructs the form with given parent.
    /// \param parent The parent widget.
    ///
    explicit CourseForm(QWidget *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~CourseForm();

public slots:
    ///
    /// \brief The slot that get called after user selects file for importing
    /// courses data to perform actual import.
    /// \param fileName The file to use.
    ///
    void onImportFileSelected(const QString &fileName);

signals:
    ///
    /// \brief This signal is emited for each imported line during course data
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

private:
    Ui::CourseForm *ui;      //!< The generated form object.
    QAction *mImportAction;  //!< The import student data action.
    QAction *mDetailsAction; //!< The student details action.

    ///
    /// \brief Creates underlying table model, record editor and returns them as
    /// a pair.
    /// \return Pair of table model and editor widget.
    ///
    static std::pair<QSqlTableModel *, widget::RecordEditorWidget *>
    createModelAndEditor();
};
}
}
#endif // COURSEFORM_H
