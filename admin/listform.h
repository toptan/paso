#ifndef LISTFORM_H
#define LISTFORM_H

#include "listeditorwidget.h"
#include "tableform.h"

namespace Ui {
class ListForm;
}

namespace paso {
namespace admin {

///
/// \brief The ListForm class is a widget for list administration.
///
class ListForm : public widget::TableForm {
    Q_OBJECT

public:
    ///
    /// \brief Contructs the form with given parent.
    /// \param parent The parent widget.
    ///
    explicit ListForm(QWidget *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~ListForm();

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
    /// \brief This slot is called after triggering details action to display
    /// dialog with list details for editing list members.
    ///
    void onDetails();

private:
    Ui::ListForm *ui;        //!< The generated form object.
    QAction *mDetailsAction; //!< The list details action.

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
#endif // LISTFORM_H
