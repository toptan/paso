#ifndef STUDENTEDITORWIDGET_H
#define STUDENTEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

///
/// \brief The StudentEditorWidget class is a student editor widget.
///
class StudentEditorWidget : public widget::RecordEditorWidget {
public:
    ///
    /// \brief Constructs student editor widget for given field types.
    /// \param fieldTypes The record field types.
    /// \param parent The parent widget.
    ///
    StudentEditorWidget(const widget::FieldTypes &fieldTypes,
                        QWidget *parent = nullptr);

protected:
    /// \copydoc widget::RecordEditorWidget::prepareEdit()
    virtual void prepareEdit(QSqlRecord &record) override;
    /// \copydoc widget::RecordEditorWidget::fieldReadOnly()
    virtual bool fieldReadOnly(const QString &key) override;
    /// \copydoc widget::RecordEditorWidget::createLineEdit()
    virtual QLineEdit *createLineEdit(const QString &field) override;
    /// \copydoc widget::RecordEditorWidget::createMaskedLineEdit()
    virtual QLineEdit *createMaskedLineEdit(const QString &field) override;
    /// \copydoc widget::RecordEditorWidget::createSpinBox()
    virtual QSpinBox *createSpinBox(const QString &field) override;
};
}
}

#endif // STUDENTEDITORWIDGET_H
