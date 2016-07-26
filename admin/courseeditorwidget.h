#ifndef COURSEEDITORWIDGET_H
#define COURSEEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class CourseEditorWidget : public widget::RecordEditorWidget {
public:
    CourseEditorWidget(const widget::FieldTypes &fieldTypes,
                       QWidget *parent = nullptr);

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createLineEdit(const QString &field) override;
};
}
}
#endif // COURSEEDITORWIDGET_H
