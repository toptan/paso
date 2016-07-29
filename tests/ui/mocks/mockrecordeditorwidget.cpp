#include "mockrecordeditorwidget.h"

MockRecordEditorWidget::MockRecordEditorWidget(const FieldTypes &fieldTypes,
                                               QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent), preparedEditCalled(false),
      fieldReadOnlyCalledCount(0), writableField("") {}

void MockRecordEditorWidget::prepareEdit(QSqlRecord &) {
    preparedEditCalled = true;
}

bool MockRecordEditorWidget::fieldReadOnly(const QString &field) {
    fieldReadOnlyCalledCount++;
    if (writableField.isEmpty() || field == writableField) {
        return false;
    } else {
        return true;
    }
}

QLineEdit *MockRecordEditorWidget::createMaskedLineEdit(const QString &field) {
    auto retVal = RecordEditorWidget::createMaskedLineEdit(field);
    retVal->setInputMask("AAAAAA/AAAA/AAAA");
    return retVal;
}

QSpinBox *MockRecordEditorWidget::createSpinBox(const QString &field) {
    auto retVal = RecordEditorWidget::createSpinBox(field);
    retVal->setMinimum(6);
    retVal->setMaximum(12);
    return retVal;
}

QComboBox *MockRecordEditorWidget::createComboBox(const QString &field) {
    auto retVal = RecordEditorWidget::createComboBox(field);
    retVal->addItem("Jedan", 1);
    retVal->addItem("Dva", 2);
    retVal->addItem("Tri", 3);
    return retVal;
}
