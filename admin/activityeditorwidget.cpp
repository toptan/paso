#include "activityeditorwidget.h"

#include "activitywizard.h"
#include "data.h"

#include <QApplication>

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
}
}
