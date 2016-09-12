#ifndef SYSTEMUSEREDITORWIDGET_H
#define SYSTEMUSEREDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

///
/// \brief The SystemUserEditorWidget class is a system user editor widget.
///
class SystemUserEditorWidget : public widget::RecordEditorWidget {
public:
    ///
    /// \brief Constructs system user editor widget for given field types.
    /// \param fieldTypes The record field types.
    /// \param parent The parent widget.
    ///
    SystemUserEditorWidget(const widget::FieldTypes &fieldTypes,
                           QWidget *parent = nullptr);

protected:
    /// \copydoc widget::RecordEditorWidget::prepareEdit()
    virtual void prepareEdit(QSqlRecord &record) override;
    /// \copydoc widget::RecordEditorWidget::fieldReadOnly()
    virtual bool fieldReadOnly(const QString &key) override;
    /// \copydoc widget::RecordEditorWidget::createLineEdit()
    virtual QLineEdit *createLineEdit(const QString &field) override;
    /// \copydoc widget::RecordEditorWidget::createPasswordLineEdit()
    virtual QLineEdit *createPasswordLineEdit(const QString &field) override;
    /// \copydoc widget::RecordEditorWidget::createComboBox()
    virtual QComboBox *createComboBox(const QString &field) override;

private:
    bool mEditingRootSystemUser; //!< \c true if editing root user.
};
}
}

#endif // SYSTEMUSEREDITORWIDGET_H
