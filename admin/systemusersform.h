#ifndef ADMINISTRATORFORM_H
#define ADMINISTRATORFORM_H

#include "tableform.h"

#include <QAction>
#include <QSqlRecord>
#include <QWidget>

class QSqlTableModel;

namespace Ui {
class SystemUsersForm;
}

namespace paso {
namespace admin {

///
/// \brief The SystemUsersForm class is a widget for system user administration.
///
class SystemUsersForm : public widget::TableForm {
    Q_OBJECT

public:
    ///
    /// \brief Contructs the form with given parent.
    /// \param parent The parent widget.
    ///
    explicit SystemUsersForm(QWidget *parent = nullptr);
    ///
    /// \brief Destructor.
    ///
    virtual ~SystemUsersForm();

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

private:
    Ui::SystemUsersForm *ui; //!< The generated form object.

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

#endif // ADMINISTRATORFORM_H
