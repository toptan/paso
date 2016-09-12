#ifndef COURSEEDITORWIDGET_H
#define COURSEEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

///
/// \brief The CourseEditorWidget class is course editor widget.
///
class CourseEditorWidget : public widget::RecordEditorWidget {
public:
    ///
    /// \brief Constructs course editor widget for given field types.
    /// \param fieldTypes The record field types.
    /// \param parent The parent widget.
    ///
    CourseEditorWidget(const widget::FieldTypes &fieldTypes,
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
#endif // COURSEEDITORWIDGET_H
