#include "activityeditorwidget.h"

#include "activity.h"
#include "activitywizard.h"
#include "data.h"
#include "entitytablemodel.h"
#include "list.h"
#include "pasodb.h"
#include "room.h"

#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QFormLayout>
#include <QHeaderView>
#include <QLabel>
#include <QListView>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QTableView>
#include <QTextStream>

using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::db;
using namespace paso::model;
using namespace paso::widget;

using namespace std;

namespace paso {
namespace admin {

ActivityEditorWidget::ActivityEditorWidget(const FieldTypes &fieldTypes,
                                           QWidget *parent)
    : RecordEditorWidget(fieldTypes, parent) {}

void ActivityEditorWidget::setupUi(const QVariantMap &columnLabels,
                                   const QSqlRecord &record,
                                   const QStringList &filterFields) {
    RecordEditorWidget::setupUi(columnLabels, record, filterFields);
    QFormLayout *l = dynamic_cast<QFormLayout *>(layout());
    mActivityRoomsView = new QListView(this);
    mActivityListsView = new QListView(this);
    mActivitySlotsView = new QTableView(this);
    l->insertRow(l->rowCount() - 1, new QLabel(tr("Rooms"), this),
                 mActivityRoomsView);
    l->insertRow(l->rowCount() - 1, new QLabel(tr("Lists"), this),
                 mActivityListsView);
    l->insertRow(l->rowCount() - 1, new QLabel(tr("Time slots"), this),
                 mActivitySlotsView);
    const QStringList columns{"NAME"};
    const QMap<QString, QString> columnNames{{"NAME", tr("Name")}};
    EntityVector emptyData;
    mActivityRoomsModel =
        new EntityTableModel(columns, columnNames, emptyData, this);
    mActivityListsModel =
        new EntityTableModel(columns, columnNames, emptyData, this);
    mActivitySlotsModel = new QSqlQueryModel(this);
    auto query = Activity::timeSlotsQuery(
        QSqlDatabase::database(DEFAULT_DB_NAME), 0);
    query.exec();
    mActivitySlotsModel->setQuery(query);

    mActivityRoomsView->setModel(mActivityRoomsModel);
    mActivityListsView->setModel(mActivityListsModel);
    mActivitySlotsView->setModel(mActivitySlotsModel);
    mActivitySlotsView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mActivitySlotsView->setSelectionMode(QAbstractItemView::SingleSelection);
    mActivitySlotsView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mActivitySlotsView->horizontalHeader()->setSectionResizeMode(
        QHeaderView::Stretch);
    mActivitySlotsView->horizontalHeader()->model()->setHeaderData(
        0, Qt::Horizontal, tr("From"), Qt::DisplayRole);
    mActivitySlotsView->horizontalHeader()->model()->setHeaderData(
        1, Qt::Horizontal, tr("To"), Qt::DisplayRole);
    mActivitySlotsView->sortByColumn(1, Qt::SortOrder::AscendingOrder);
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
    wizard->setWindowTitle(tr("Edit activity wizard"));
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
    wizard->setWindowTitle(tr("New activity wizard"));
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
        case FieldType::NumberEdit:
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
    DBManager manager;
    QSqlError error;
    quint64 activityId = record.value("ID").toULongLong();
    mActivityRoomsModel->setEntityData(
        manager.activityRooms(activityId, error));
    mActivityListsModel->setEntityData(
        manager.activityLists(activityId, error));
    auto query = Activity::timeSlotsQuery(
        QSqlDatabase::database(DEFAULT_DB_NAME), activityId);
    query.exec();
    mActivitySlotsModel->setQuery(query);
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
