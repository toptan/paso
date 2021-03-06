#include "recordeditorwidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QToolTip>
#include <QVariantMap>

#include "data.h"
#include "pdateedit.h"

namespace paso {
namespace widget {

using namespace paso::data;
using namespace std;

RecordEditorWidget::RecordEditorWidget(const FieldTypes &fieldTypes,
                                       QWidget *parent)
    : QWidget(parent), mNewRecord(false), mFieldTypes(fieldTypes),
      mButtonBox(nullptr), mValidator(nullptr) {}

RecordEditorWidget::~RecordEditorWidget() {}

void RecordEditorWidget::setupUi(const QVariantMap &columnLabels,
                                 const QSqlRecord &record,
                                 const QStringList &filterFields) {
    mFilterFields = filterFields;
    setLayout(new QFormLayout(this));
    layout()->setMargin(0);
    setMinimumWidth(400);
    auto l = dynamic_cast<QFormLayout *>(layout());
    l->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    for (int i = 0; i < record.count(); i++) {
        if (filterFields.contains(record.fieldName(i))) {
            continue;
        }
        if (record.fieldName(i) == "id" || record.fieldName(i) == "id_course") {
            continue;
        }
        QLabel *label =
            new QLabel(columnLabels[record.fieldName(i)].toString(), this);
        QWidget *edit = createWidgetForField(record, i);
        label->setBuddy(edit);
        mFieldEditors[record.fieldName(i)] = edit;
        auto checkBox = dynamic_cast<QCheckBox *>(edit);
        if (checkBox != nullptr) {
            checkBox->setText(columnLabels[record.fieldName(i)].toString());
            label->setVisible(false);
        }
        l->addRow(label, edit);
    }
    mButtonBox = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    mButtonBox->button(QDialogButtonBox::Save)->setText(tr("Save"));
    mButtonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    l->addWidget(mButtonBox);
    mButtonBox->setVisible(false);
    connect(mButtonBox, &QDialogButtonBox::accepted, this,
            &RecordEditorWidget::accepted);
    connect(mButtonBox, &QDialogButtonBox::rejected, this,
            &RecordEditorWidget::rejected);
}

QLineEdit *RecordEditorWidget::createLineEdit(const QString &) {
    auto retVal = new QLineEdit(this);
    retVal->setReadOnly(true);
    return retVal;
}

QLineEdit *RecordEditorWidget::createPasswordLineEdit(const QString &) {
    auto retVal = new QLineEdit(this);
    retVal->setReadOnly(true);
    retVal->setEchoMode(QLineEdit::Password);
    return retVal;
}

QLineEdit *RecordEditorWidget::createMaskedLineEdit(const QString &) {
    auto retVal = new QLineEdit(this);
    retVal->setReadOnly(true);
    return retVal;
}

QComboBox *RecordEditorWidget::createComboBox(const QString &) {
    auto retVal = new QComboBox(this);
    retVal->setCurrentIndex(-1);
    retVal->setEnabled(false);
    return retVal;
}

QSpinBox *RecordEditorWidget::createSpinBox(const QString &field) {
    auto retVal = new QSpinBox(this);
    retVal->setValue(0);
    retVal->setEnabled(false);
    return retVal;
}

QCheckBox *RecordEditorWidget::createCheckBox(const QString &field) {
    auto retVal = new QCheckBox(this);
    retVal->setChecked(false);
    retVal->setEnabled(false);
    return retVal;
}

QDateEdit *RecordEditorWidget::createDateEdit(const QString &field) {
    auto retVal = new PDateEdit(this);
    retVal->setReadOnly(true);
    return retVal;
}

QTimeEdit *RecordEditorWidget::createTimeEdit(const QString &field) {
    auto retVal = new QTimeEdit(this);
    retVal->setDisplayFormat("H.mm");
    retVal->setReadOnly(true);
    return retVal;
}

QWidget *RecordEditorWidget::createWidgetForField(const QSqlRecord &record,
                                                  int index) {
    auto fieldName = record.fieldName(index);
    auto fieldType = mFieldTypes[fieldName];
    QWidget *fieldEditor;
    switch (fieldType) {
    case FieldType::ComboBox:
        fieldEditor = createComboBox(fieldName);
        break;
    case FieldType::LineEdit:
        fieldEditor = createLineEdit(fieldName);
        break;
    case FieldType::PasswordEdit:
        fieldEditor = createPasswordLineEdit(fieldName);
        break;
    case FieldType::MaskedLineEdit:
        fieldEditor = createMaskedLineEdit(fieldName);
        break;
    case FieldType::NumberEdit:
        fieldEditor = createSpinBox(fieldName);
        break;
    case FieldType::CheckBox:
        fieldEditor = createCheckBox(fieldName);
        break;
    case FieldType::DateEdit:
        fieldEditor = createDateEdit(fieldName);
        break;
    case FieldType::TimeEdit:
        fieldEditor = createTimeEdit(fieldName);
        break;
    }
    fieldEditor->setSizePolicy(QSizePolicy::Policy::Expanding,
                               QSizePolicy::Policy::Fixed);
    return fieldEditor;
}

void RecordEditorWidget::onDisplayRecord(const QSqlRecord &record) {
    for (int i = 0; i < record.count(); i++) {
        auto fieldName = record.fieldName(i);
        if (mFilterFields.contains(fieldName)) {
            continue;
        }
        if (fieldName.toUpper() == "ID" || fieldName.toUpper() == "ID_COURSE") {
            continue;
        }
        switch (mFieldTypes[fieldName]) {
        case FieldType::ComboBox: {
            auto field = dynamic_cast<QComboBox *>(mFieldEditors[fieldName]);
            field->setCurrentIndex(
                mNewRecord ? 0 : field->findData(record.value(fieldName)));
        } break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
        case FieldType::MaskedLineEdit:
            dynamic_cast<QLineEdit *>(mFieldEditors[fieldName])
                ->setText(record.value(fieldName).toString());
            break;
        case FieldType::NumberEdit:
            dynamic_cast<QSpinBox *>(mFieldEditors[fieldName])
                ->setValue(record.value(fieldName).toLongLong());
            break;
        case FieldType::CheckBox:
            dynamic_cast<QCheckBox *>(mFieldEditors[fieldName])
                ->setChecked(record.value(fieldName).toBool());
            break;
        case FieldType::DateEdit:
            dynamic_cast<QDateEdit *>(mFieldEditors[fieldName])
                ->setDate(record.value(fieldName).toDate());
            break;
        case FieldType::TimeEdit:
            dynamic_cast<QTimeEdit *>(mFieldEditors[fieldName])
                ->setTime(record.value(fieldName).toTime());
            break;
        }
    }
}

void RecordEditorWidget::onEditExistingRecord(QSqlRecord record) {
    mRecord = record;
    mNewRecord = false;
    prepareEdit(record);
    onDisplayRecord(record);
    setFieldsEditable();
}

void RecordEditorWidget::onEditNewRecord(QSqlRecord record) {
    mRecord = record;
    mNewRecord = true;
    prepareEdit(record);
    onDisplayRecord(record);
    setFieldsEditable();
}

void RecordEditorWidget::clearData() {
    for (const auto &key : mFieldTypes.keys()) {
        switch (mFieldTypes[key]) {
        case FieldType::ComboBox: {
            auto comboBox = dynamic_cast<QComboBox *>(mFieldEditors[key]);
            if (comboBox != nullptr) {
                comboBox->setCurrentIndex(-1);
            }
        } break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
        case FieldType::MaskedLineEdit: {
            auto lineEdit = dynamic_cast<QLineEdit *>(mFieldEditors[key]);
            if (lineEdit != nullptr) {
                lineEdit->clear();
            }
        } break;
        case FieldType::NumberEdit: {
            auto spinBox = dynamic_cast<QSpinBox *>(mFieldEditors[key]);
            if (spinBox != nullptr) {
                spinBox->setValue(0);
                spinBox->clear();
            }
        } break;
        case FieldType::CheckBox: {
            auto checkBox = dynamic_cast<QCheckBox *>(mFieldEditors[key]);
            if (checkBox != nullptr) {
                checkBox->setChecked(false);
            }
        } break;
        case FieldType::DateEdit: {
            auto dateEdit = dynamic_cast<QDateEdit *>(mFieldEditors[key]);
            if (dateEdit != nullptr) {
                dateEdit->setDate(QDate::currentDate());
            }
        } break;
        case FieldType::TimeEdit: {
            auto timeEdit = dynamic_cast<QTimeEdit *>(mFieldEditors[key]);
            if (timeEdit != nullptr) {
                timeEdit->setTime(QTime(0, 0, 0));
            }
        } break;
        }
    }
}

void RecordEditorWidget::saveSuccessfull() {
    setFieldsReadOnly();
    mRecord.clear();
}

void RecordEditorWidget::setValidator(RecordValidator *validator) {
    mValidator = validator;
}

const FieldEditors &RecordEditorWidget::fieldEditors() const {
    return mFieldEditors;
}

const FieldTypes &RecordEditorWidget::fieldTypes() const { return mFieldTypes; }

void RecordEditorWidget::accepted() {
    shared_ptr<ValidationError> error;
    if (mValidator != nullptr) {
        error = mValidator->validate(mRecord);
    }

    if (error) {
        showEntryError(error->editor, error->title, error->text,
                       error->detailedText, error->icon);
        return;
    }

    for (const auto &key : mFieldEditors.keys()) {
        auto field = mFieldEditors[key];
        auto fieldType = mFieldTypes[key];
        QVariant value;
        switch (fieldType) {
        case FieldType::ComboBox:
            value = dynamic_cast<QComboBox *>(field)->currentData().toString();
            break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
        case FieldType::MaskedLineEdit:
            value = dynamic_cast<QLineEdit *>(field)->text().trimmed();
            break;
        case FieldType::NumberEdit:
            value = dynamic_cast<QSpinBox *>(field)->text().trimmed();
            break;
        case FieldType::CheckBox:
            value = dynamic_cast<QCheckBox *>(field)->isChecked();
            break;
        case FieldType::DateEdit:
            value = dynamic_cast<QDateEdit *>(field)->date();
            break;
        case FieldType::TimeEdit:
            value = dynamic_cast<QTimeEdit *>(field)->time();
            break;
        }

        mRecord.setValue(key, value);
    }
    if (mNewRecord) {
        emit requestSave(mRecord);
    } else {
        emit requestUpdate(mRecord);
    }
}

RecordValidator *RecordEditorWidget::validator() const { return mValidator; }

void RecordEditorWidget::rejected() {
    if (mNewRecord) {
        clearData();
    } else {
        onDisplayRecord(mRecord);
    }
    setFieldsReadOnly();
    emit editFinished();
}

void RecordEditorWidget::setFieldsEditable() {
    for (const auto &key : mFieldEditors.keys()) {
        auto field = mFieldEditors[key];
        auto fieldType = mFieldTypes[key];
        auto readOnly = fieldReadOnly(key);
        switch (fieldType) {
        case FieldType::ComboBox:
        case FieldType::CheckBox:
        case FieldType::NumberEdit:
            field->setEnabled(!readOnly);
            break;
        case FieldType::LineEdit:
        case FieldType::MaskedLineEdit: {
            auto edit = dynamic_cast<QLineEdit *>(field);
            // Handle special case where root username cannot be changed.
            edit->setReadOnly(readOnly);
        } break;
        case FieldType::PasswordEdit: {
            auto pwdEdit = dynamic_cast<QLineEdit *>(field);
            pwdEdit->setReadOnly(readOnly);
            pwdEdit->setEchoMode(QLineEdit::Normal);
        } break;
        case FieldType::DateEdit:
            dynamic_cast<QDateEdit *>(field)->setReadOnly(readOnly);
            break;
        case FieldType::TimeEdit:
            dynamic_cast<QTimeEdit *>(field)->setReadOnly(readOnly);
            break;
        }
    }
    mButtonBox->setVisible(true);
    this->setFocus();
    focusFirstEditable();
}

void RecordEditorWidget::setFieldsReadOnly() {
    for (const auto &key : mFieldEditors.keys()) {
        auto field = mFieldEditors[key];
        auto fieldType = mFieldTypes[key];
        switch (fieldType) {
        case FieldType::ComboBox:
        case FieldType::CheckBox:
        case FieldType::NumberEdit:
            field->setEnabled(false);
            break;
        case FieldType::LineEdit:
        case FieldType::MaskedLineEdit: {
            auto edit = dynamic_cast<QLineEdit *>(field);
            edit->setReadOnly(true);
        } break;
        case FieldType::PasswordEdit: {
            auto pwdEdit = dynamic_cast<QLineEdit *>(field);
            pwdEdit->setReadOnly(true);
            pwdEdit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        } break;
        case FieldType::DateEdit:
            dynamic_cast<QDateEdit *>(field)->setReadOnly(true);
            break;
        case FieldType::TimeEdit:
            dynamic_cast<QTimeEdit *>(field)->setReadOnly(true);
        }
    }
    mButtonBox->setVisible(false);
}

void RecordEditorWidget::focusFirstEditable() {
    for (auto child : children()) {
        auto lineEdit = dynamic_cast<QLineEdit *>(child);
        if (lineEdit != nullptr && !lineEdit->isReadOnly()) {
            lineEdit->setFocus();
            return;
        }
        auto combo = dynamic_cast<QComboBox *>(child);
        if (combo != nullptr && combo->isEnabled()) {
            combo->setFocus();
            return;
        }
        auto spinBox = dynamic_cast<QSpinBox *>(child);
        if (spinBox != nullptr && spinBox->isEnabled()) {
            spinBox->setFocus();
            return;
        }
        auto checkBox = dynamic_cast<QCheckBox *>(child);
        if (checkBox != nullptr && checkBox->isEnabled()) {
            checkBox->setFocus();
            return;
        }
        auto dateEdit = dynamic_cast<QDateEdit *>(child);
        if (dateEdit != nullptr && !dateEdit->isReadOnly()) {
            dateEdit->setFocus();
            return;
        }
        auto timeEdit = dynamic_cast<QTimeEdit *>(child);
        if (timeEdit != nullptr && !timeEdit->isReadOnly()) {
            timeEdit->setFocus();
            return;
        }
    }
}
}
}
