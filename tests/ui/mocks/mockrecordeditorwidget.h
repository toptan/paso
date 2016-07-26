#ifndef MOCKRECORDEDITORWIDGET_H
#define MOCKRECORDEDITORWIDGET_H

#include "recordeditorwidget.h"

using namespace paso::widget;

class MockRecordEditorWidget : public RecordEditorWidget {
public:
    explicit MockRecordEditorWidget(const FieldTypes &fieldTypes,
                                    QWidget *parent = nullptr);

protected:
    virtual void prepareEdit(QSqlRecord &) override;
    virtual bool fieldReadOnly(const QString &field) override;

    virtual QLineEdit *createMaskedLineEdit(const QString &field) override;
    virtual QComboBox *createComboBox(const QString &field) override;
    virtual QSpinBox *createSpinBox(const QString &field) override;

public:
    // FOR EASIER TESTING PURPOSES
    bool preparedEditCalled;
    int fieldReadOnlyCalledCount;
    QString writableField;
};

#endif // MOCKRECORDEDITORWIDGET_H
