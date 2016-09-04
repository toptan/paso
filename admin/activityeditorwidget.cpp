#include "activityeditorwidget.h"

#include "activitywizard.h"
#include "data.h"

#include <QApplication>
#include <QCheckBox>
#include <QTextStream>

using namespace paso::data;
using namespace paso::widget;
using namespace std;

namespace paso {
namespace admin {

ActivityEditorWidget::ActivityEditorWidget(const FieldTypes &fieldTypes,
                                           QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent) {}

void ActivityEditorWidget::setupUi(const QVariantMap &columnLabels,
                                   const QSqlRecord &record) {
    RecordEditorWidget::setupUi(columnLabels, record);
}

void ActivityEditorWidget::prepareEdit(QSqlRecord &record) {
    // No action needed.
}

bool ActivityEditorWidget::fieldReadOnly(const QString &key) {
    // All fields are read only as we will use wizard dialog for all editing.
    return true;
}

QComboBox *ActivityEditorWidget::createComboBox(const QString &field) {
    auto retVal = RecordEditorWidget::createComboBox(field);
    for (const auto &type : enumeratedActivityTypes.keys()) {
        retVal->addItem(
            QApplication::instance()->translate(
                "QObject", enumeratedActivityTypes[type].toStdString().c_str()),
            activityTypeToString(type));
    }
    return retVal;
}

void ActivityEditorWidget::onEditExistingRecord(QSqlRecord record) {
    // We are read only widget, so we should set all fields read only and
    // delegate work to the wizard.
    RecordEditorWidget::onEditExistingRecord(record);
    setFieldsReadOnly();
    ActivityWizard *wizard = new ActivityWizard(record, this);
    connect(wizard, &ActivityWizard::accepted, this,
            &ActivityEditorWidget::accepted);
    connect(wizard, &ActivityWizard::rejected, this,
            &ActivityEditorWidget::rejected);
    connect(wizard, &ActivityWizard::accepted, wizard, &QObject::deleteLater);
    connect(wizard, &ActivityWizard::rejected, wizard, &QObject::deleteLater);
    wizard->setModal(true);
    wizard->show();
}

void ActivityEditorWidget::onEditNewRecord(QSqlRecord record) {
    // We are read only widget, so we should set all fields read only and
    // delegate work to the wizard.
    RecordEditorWidget::onEditNewRecord(record);
    setFieldsReadOnly();
    ActivityWizard *wizard = new ActivityWizard(record, this);
    connect(wizard, &ActivityWizard::accepted, this,
            &ActivityEditorWidget::accepted);
    connect(wizard, &ActivityWizard::rejected, this,
            &ActivityEditorWidget::rejected);
    connect(wizard, &ActivityWizard::accepted, wizard, &QObject::deleteLater);
    connect(wizard, &ActivityWizard::rejected, wizard, &QObject::deleteLater);
    wizard->setModal(true);
    wizard->show();
}

void ActivityEditorWidget::accepted() {
    if (mNewRecord) {
        emit requestSave(mRecord);
    } else {
        emit requestUpdate(mRecord);
    }
}

void ActivityEditorWidget::onDisplayRecord(const QSqlRecord &record) {
    for (int i = 0; i < record.count(); i++) {
        auto fieldName = record.fieldName(i);
        if (fieldName.toUpper() == "ID" || fieldName.toUpper() == "ID_COURSE") {
            continue;
        }
        switch (fieldTypes()[fieldName]) {
        case FieldType::ComboBox: {
            auto field = dynamic_cast<QComboBox *>(fieldEditors()[fieldName]);
            field->setCurrentIndex(
                mNewRecord ? 0 : field->findData(record.value(fieldName)));
        } break;
        case FieldType::LineEdit:
        case FieldType::PasswordEdit:
        case FieldType::MaskedLineEdit:
            if (fieldName == "scheduled_days") {
                dynamic_cast<QLineEdit *>(fieldEditors()[fieldName])
                    ->setText(generateRepetitionString(
                        record.value("schedule_type").toString(),
                        record.value(fieldName).toString()));
            } else if (fieldName == "schedule_type") {
                dynamic_cast<QLineEdit *>(fieldEditors()[fieldName])
                    ->setText(translateScheduleType(
                        record.value(fieldName).toString()));
            } else {
                dynamic_cast<QLineEdit *>(fieldEditors()[fieldName])
                    ->setText(record.value(fieldName).toString());
            }
            break;
        case FieldType::NumberEdit:
            dynamic_cast<QSpinBox *>(fieldEditors()[fieldName])
                ->setValue(record.value(fieldName).toLongLong());
            break;
        case FieldType::CheckBox:
            dynamic_cast<QCheckBox *>(fieldEditors()[fieldName])
                ->setChecked(record.value(fieldName).toBool());
            break;
        case FieldType::DateEdit:
            dynamic_cast<QDateEdit *>(fieldEditors()[fieldName])
                ->setDate(record.value(fieldName).toDate());
            break;
        case FieldType::TimeEdit:
            dynamic_cast<QTimeEdit *>(fieldEditors()[fieldName])
                ->setTime(record.value(fieldName).toTime());
            break;
        }
    }
}

QString
ActivityEditorWidget::generateRepetitionString(const QString &repetitionType,
                                               const QString &strDays) {
    QString retVal = "";
    if (repetitionType == "WEEK_DAYS") {
        const QVariantList days = jsonArrayStringToVariantList(strDays);
        QTextStream ts(&retVal);
        for (const auto &day : days) {
            auto translated = QApplication::instance()->translate(
                "QObject",
                repetitionWeekDays[day.toInt() - 1].toStdString().c_str());
            ts << translated << ", ";
        }
        retVal = retVal.trimmed();
        if (retVal.endsWith(",")) {
            retVal.chop(1);
        }
        auto lastIndex = retVal.lastIndexOf(",");
        retVal = retVal.replace(lastIndex, 1, QString(" ") + tr("and"));
    } else if (repetitionType == "MONTH_DAYS") {
        const QVariantList days = jsonArrayStringToVariantList(strDays);
        QTextStream ts(&retVal);
        for (const auto &day : days) {
            ts << day.toInt() << ", ";
        }
        retVal = retVal.trimmed();
        if (retVal.endsWith(",")) {
            retVal.chop(1);
        }
        auto lastIndex = retVal.lastIndexOf(",");
        retVal = retVal.replace(lastIndex, 1, QString(" ") + tr("and"));
        retVal = tr("Every ") + retVal + QString(" ") + tr("in month");
    }

    return retVal;
}

QString
ActivityEditorWidget::translateScheduleType(const QString &strScheduleType) {
    return QApplication::instance()->translate(
        "QObject", stringEnumeratedActivityScheduleTypes[strScheduleType]
                       .toStdString()
                       .c_str());
}
}
}
