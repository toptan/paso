#ifndef STUDENTEDITORWIDGET_H
#define STUDENTEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class StudentEditorWidget : public RecordEditorWidget {
public:
    StudentEditorWidget(const QSqlRecord &record, const FieldTypes &fieldTypes,
                        QWidget *parent = nullptr);

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createMaskedLineEdit(const QString &field) override;
    virtual QSpinBox *createSpinBox(const QString &field) override;
};
}
}

#endif // STUDENTEDITORWIDGET_H
