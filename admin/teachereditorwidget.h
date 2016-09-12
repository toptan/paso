#ifndef TEACHEREDITORWIDGET_H
#define TEACHEREDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

///
/// \brief The TeacherEditorWidget class is teacher editor widget.
///
class TeacherEditorWidget : public widget::RecordEditorWidget {
public:
    ///
    /// \brief Constructs teacher editor widget for given field types.
    /// \param fieldTypes The record field types.
    /// \param parent The parent widget.
    ///
    TeacherEditorWidget(const widget::FieldTypes &fieldTypes,
                        QWidget *parent = nullptr);

protected:
    /// \copydoc widget::RecordEditorWidget::prepareEdit()
    virtual void prepareEdit(QSqlRecord &record) override;
    /// \copydoc widget::RecordEditorWidget::fieldReadOnly()
    virtual bool fieldReadOnly(const QString &key) override;
    /// \copydoc widget::RecordEditorWidget::createLineEdit()
    virtual QLineEdit *createLineEdit(const QString &field) override;
};
}
}
#endif // TEACHEREDITORWIDGET_H
