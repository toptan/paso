#ifndef ACTIVITYFORM_H
#define ACTIVITYFORM_H

#include "activityeditorwidget.h"
#include "queryform.h"

namespace Ui {
class ActivityForm;
}

namespace paso {
namespace admin {

///
/// \brief The ActivityForm class is a widget for activity administration.
///
class ActivityForm : public widget::QueryForm {
    Q_OBJECT

public:
    ///
    /// \brief Contructs the form with given parent.
    /// \param parent The parent widget.
    ///
    explicit ActivityForm(QWidget *parent = 0);
    ///
    /// \brief Destructor.
    ///
    virtual ~ActivityForm();

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

private:
    Ui::ActivityForm *ui; //!< The generated form object.

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

#endif // ACTIVITYFORM_H
