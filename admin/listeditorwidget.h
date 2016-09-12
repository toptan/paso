#ifndef LISTEDITORWIDGET_H
#define LISTEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

///
/// \brief The ListEditorWidget class is list editor widget.
///
class ListEditorWidget : public widget::RecordEditorWidget {
public:
    ///
    /// \brief Constructs list editor widget for given field types.
    /// \param fieldTypes The record field types.
    /// \param parent The parent widget.
    ///
    ListEditorWidget(const widget::FieldTypes &fieldTypes,
                     QWidget *parent = nullptr);

    /// \copydoc widget::RecordEditorWidget::setupUi()
    virtual void setupUi(const QVariantMap &columnLabels,
                         const QSqlRecord &record,
                         const QStringList &filterFields = {}) override;

protected:
    /// \copydoc widget::RecordEditorWidget::prepareEdit()
    virtual void prepareEdit(QSqlRecord &record) override;
    /// \copydoc widget::RecordEditorWidget::fieldReadOnly()
    virtual bool fieldReadOnly(const QString &key) override;
    /// \copydoc widget::RecordEditorWidget::createLineEdit()
    virtual QLineEdit *createLineEdit(const QString &field) override;
    /// \copydoc widget::RecordEditorWidget::createCheckBox()
    virtual QCheckBox *createCheckBox(const QString &field) override;
    /// \copydoc widget::RecordEditorWidget::createDateEdit()
    virtual QDateEdit *createDateEdit(const QString &field) override;
};
}
}

#endif // LISTEDITORWIDGET_H
