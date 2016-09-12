#ifndef TEACHERFORM_H
#define TEACHERFORM_H

#include "queryform.h"
#include "teachereditorwidget.h"
#include "teachervalidator.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

namespace Ui {
class TeacherForm;
}

namespace paso {
namespace admin {

///
/// \brief The TeacherForm class is a widget for teachers administration.
///
class TeacherForm : public widget::QueryForm {
    Q_OBJECT

public:
    ///
    /// \brief Contructs the form with given parent.
    /// \param parent The parent widget.
    ///
    explicit TeacherForm(QWidget *parent = 0);
    ///
    /// \brief Destructor.
    ///
    virtual ~TeacherForm();

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
    /// \brief This slot is called after triggering report action to display
    /// teacher's room entry report.
    ///
    void onReport();

private:
    Ui::TeacherForm *ui;    //!< The generated form object.
    QAction *mReportAction; //!< The room entry report action.

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
#endif // TEACHERFORM_H
