#ifndef TEACHEREDITORWIDGET_H
#define TEACHEREDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class TeacherEditorWidget : public widget::RecordEditorWidget {
public:
    TeacherEditorWidget(const widget::FieldTypes &fieldTypes,
                        QWidget *parent = nullptr);

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createLineEdit(const QString &field) override;
};
}
}
#endif // TEACHEREDITORWIDGET_H
