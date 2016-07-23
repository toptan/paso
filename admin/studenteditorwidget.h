#ifndef STUDENTEDITORWIDGET_H
#define STUDENTEDITORWIDGET_H

#include "recordeditorwidget.h"

namespace paso {
namespace admin {

class StudentEditorWidget : public widget::RecordEditorWidget {
public:
    StudentEditorWidget(
                        const widget::FieldTypes &fieldTypes,
                        QWidget *parent = nullptr);

public slots:
    virtual void saveError() override;

protected:
    virtual void prepareEdit(QSqlRecord &record) override;
    virtual bool fieldReadOnly(const QString &key) override;

    virtual QLineEdit *createLineEdit(const QString &field) override;
    virtual QLineEdit *createMaskedLineEdit(const QString &field) override;
    virtual QSpinBox *createSpinBox(const QString &field) override;
};
}
}

#endif // STUDENTEDITORWIDGET_H
