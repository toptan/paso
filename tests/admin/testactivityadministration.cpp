#include "testactivityadministration.h"

#include "activity.h"
#include "activityeditorwidget.h"
#include "activityform.h"
#include "activitytablemodel.h"
#include "activityvalidator.h"
#include "activitywizardfixeddatepage.h"
#include "activitywizardlistsselectionpage.h"
#include "activitywizardnameandtypepage.h"
#include "activitywizardrepetitivedatespage.h"
#include "activitywizardroomsselectionpage.h"
#include "addremoveentitiesform.h"

#include "data.h"
#include "itemspicker.h"
#include "list.h"
#include "pasodb.h"
#include "room.h"

#include <QAction>
#include <QCheckBox>
#include <QDate>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSqlError>
#include <QSqlField>
#include <QTableView>
#include <QTime>
#include <memory>

using namespace std;
using namespace paso::db;
using namespace paso::data;
using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;
using namespace paso::model;

TestActivityAdministration::TestActivityAdministration() : TestBase() {}

void TestActivityAdministration::testActivityValidator() {
    const QSqlRecord record;

    unique_ptr<ActivityValidator> validator(
        new ActivityValidator(FieldTypes(), FieldEditors(), this));
    QVERIFY(validator->validate(record) == nullptr);
}

void TestActivityAdministration::testActivityEditorWidget() {
    const FieldTypes fieldTypes{{"name", FieldType::LineEdit},
                                {"type", FieldType::ComboBox},
                                {"schedule", FieldType::LineEdit},
                                {"duration", FieldType::TimeEdit},
                                {"start_date", FieldType::DateEdit},
                                {"finish_date", FieldType::DateEdit},
                                {"can_overlap", FieldType::CheckBox}};

    const QVariantMap columnLabels{{"name", "Name"},
                                   {"type", "Type"},
                                   {"schedule", "Schedule"},
                                   {"duration", "Duration"},
                                   {"start_date", "Start date"},
                                   {"finish_date", "Finish date"},
                                   {"can_ovarlap", "Can overlap"}};

    QSqlRecord record;
    record.append(QSqlField("id", QVariant::ULongLong));
    record.append(QSqlField("name", QVariant::String));
    record.append(QSqlField("type", QVariant::String));
    record.append(QSqlField("schedule", QVariant::String));
    record.append(QSqlField("duration", QVariant::Time));
    record.append(QSqlField("start_date", QVariant::Date));
    record.append(QSqlField("finish_date", QVariant::Date));
    record.append(QSqlField("can_overlap", QVariant::Bool));
    unique_ptr<ActivityEditorWidget> editor(
        new ActivityEditorWidget(fieldTypes));
    editor->setupUi(columnLabels, record);
    editor->show();
    QTest::qWaitForWindowExposed(editor.get());
    editor->onEditNewRecord(record);
    QApplication::processEvents();
    auto nameEdit = dynamic_cast<QLineEdit *>(editor->fieldEditors()["name"]);
    auto typeEdit = dynamic_cast<QComboBox *>(editor->fieldEditors()["type"]);
    auto scheduleEdit =
        dynamic_cast<QLineEdit *>(editor->fieldEditors()["schedule"]);
    auto durationEdit =
        dynamic_cast<QTimeEdit *>(editor->fieldEditors()["duration"]);
    auto startEditor =
        dynamic_cast<QDateEdit *>(editor->fieldEditors()["start_date"]);
    auto finishEditor =
        dynamic_cast<QDateEdit *>(editor->fieldEditors()["finish_date"]);
    auto canOverlapEditor =
        dynamic_cast<QCheckBox *>(editor->fieldEditors()["can_overlap"]);
    QVERIFY(nameEdit->isReadOnly());
    QVERIFY(!typeEdit->isEnabled());
    QVERIFY(scheduleEdit->isReadOnly());
    QVERIFY(durationEdit->isReadOnly());
    QVERIFY(startEditor->isReadOnly());
    QVERIFY(finishEditor->isReadOnly());
    QVERIFY(!canOverlapEditor->isEnabled());

    editor->saveSuccessfull();
    QApplication::processEvents();
    editor->onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(nameEdit->isReadOnly());
    QVERIFY(!typeEdit->isEnabled());
    QVERIFY(scheduleEdit->isReadOnly());
    QVERIFY(durationEdit->isReadOnly());
    QVERIFY(startEditor->isReadOnly());
    QVERIFY(finishEditor->isReadOnly());
    QVERIFY(!canOverlapEditor->isEnabled());
}

void TestActivityAdministration::testActivityTableModel() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("DELETE FROM ACTIVITY");

    const QVariantList scheduledDates{1, 3, 5};
    Activity activity("A1", ActivityType::EXAM);
    activity.setScheduleType(ActivityScheduleType::ONCE);
    activity.setScheduledDays(scheduledDates);
    activity.setDuration(QTime(3, 0));
    activity.setStartDate(QDate(2016, 8, 15));
    activity.setFinishDate(QDate(2016, 8, 15));
    activity.setCanOverlap(false);
    manager.saveActivity(activity, error);
    activity.setId(0);
    activity.setName("A2");
    activity.setType(ActivityType::INDIVIDUAL_WORK);
    activity.setScheduleType(ActivityScheduleType::WEEK_DAYS);
    activity.setDuration(QTime(1, 30));
    activity.setFinishDate(QDate(2016, 9, 15));
    activity.setCanOverlap(true);
    manager.saveActivity(activity, error);
    activity.setId(0);
    activity.setName("A3");
    activity.setType(ActivityType::LECTURE);
    activity.setScheduleType(ActivityScheduleType::MONTH_DAYS);
    activity.setFinishDate(QDate(2017, 9, 15));
    activity.setCanOverlap(false);
    manager.saveActivity(activity, error);
    const QVariantMap columnLabels{{"name", "Name"},
                                   {"type", "Type"},
                                   {"schedule_type", "Repeats"},
                                   {"scheduled_days", "When"},
                                   {"duration", "Duration"},
                                   {"start_date", "Start date"},
                                   {"finish_date", "Finish date"},
                                   {"can_overlap", "Can overlap"}};

    ActivityQueryModel model(columnLabels, db);
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.columnCount(), 9);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(), QString("Name"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QString("Type"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(),
             QString("Repeats"));
    QCOMPARE(model.headerData(4, Qt::Horizontal).toString(), QString("When"));
    QCOMPARE(model.headerData(5, Qt::Horizontal).toString(),
             QString("Duration"));
    QCOMPARE(model.headerData(6, Qt::Horizontal).toString(),
             QString("Start date"));
    QCOMPARE(model.headerData(7, Qt::Horizontal).toString(),
             QString("Finish date"));
    QCOMPARE(model.headerData(8, Qt::Horizontal).toString(),
             QString("Can overlap"));
    auto index = model.index(0, 1);
    QCOMPARE(model.data(index).toString(), QString("A1"));
    index = model.index(0, 2);
    QCOMPARE(model.data(index).toString(), QString("Exam"));
    index = model.index(0, 3);
    QCOMPARE(model.data(index).toString(), QString("Never"));
    index = model.index(0, 4);
    QCOMPARE(model.data(index).toString(), QString(""));
    index = model.index(0, 5);
    QCOMPARE(model.data(index).toTime(), QTime(3, 0, 0));
    index = model.index(0, 6);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 8, 15));
    index = model.index(0, 7);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 8, 15));
    index = model.index(0, 8);
    QCOMPARE(model.data(index).toString(), QString("No"));

    index = model.index(1, 1);
    QCOMPARE(model.data(index).toString(), QString("A2"));
    index = model.index(1, 2);
    QCOMPARE(model.data(index).toString(), QString("Individual work"));
    index = model.index(1, 3);
    QCOMPARE(model.data(index).toString(), QString("Weekly"));
    index = model.index(1, 4);
    QCOMPARE(model.data(index).toString(),
             QString("Monday, Wednesday and Friday"));
    index = model.index(1, 5);
    QCOMPARE(model.data(index).toTime(), QTime(1, 30, 0));
    index = model.index(1, 6);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 8, 15));
    index = model.index(1, 7);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 9, 15));
    index = model.index(1, 8);
    QCOMPARE(model.data(index).toString(), QString("Yes"));

    index = model.index(2, 1);
    QCOMPARE(model.data(index).toString(), QString("A3"));
    index = model.index(2, 2);
    QCOMPARE(model.data(index).toString(), QString("Lecture or excercise"));
    index = model.index(2, 3);
    QCOMPARE(model.data(index).toString(), QString("Monthly"));
    index = model.index(2, 4);
    QCOMPARE(model.data(index).toString(),
             QString("Every 1, 3 and 5 in month"));
    index = model.index(2, 5);
    QCOMPARE(model.data(index).toTime(), QTime(1, 30, 0));
    index = model.index(2, 6);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 8, 15));
    index = model.index(2, 7);
    QCOMPARE(model.data(index).toDate(), QDate(2017, 9, 15));
    index = model.index(2, 8);
    QCOMPARE(model.data(index).toString(), QString("No"));
}

void TestActivityAdministration::testActivityForm() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("DELETE FROM ACTIVITY");
    const QVariantList scheduledDates{1, 3, 5};
    Activity activity("A1", ActivityType::EXAM);
    activity.setScheduleType(ActivityScheduleType::ONCE);
    activity.setScheduledDays(scheduledDates);
    activity.setDuration(QTime(3, 0));
    activity.setStartDate(QDate(2016, 8, 15));
    activity.setFinishDate(QDate(2016, 8, 15));
    activity.setCanOverlap(false);
    manager.saveActivity(activity, error);
    activity.setId(0);
    activity.setName("A2");
    activity.setType(ActivityType::INDIVIDUAL_WORK);
    activity.setScheduleType(ActivityScheduleType::WEEK_DAYS);
    activity.setDuration(QTime(1, 30));
    activity.setFinishDate(QDate(2016, 9, 15));
    activity.setCanOverlap(true);
    manager.saveActivity(activity, error);
    activity.setId(0);
    activity.setName("A3");
    activity.setType(ActivityType::LECTURE);
    activity.setScheduleType(ActivityScheduleType::MONTH_DAYS);
    activity.setFinishDate(QDate(2017, 9, 15));
    activity.setCanOverlap(false);
    manager.saveActivity(activity, error);

    unique_ptr<ActivityForm> form(new ActivityForm);
    form->show();
    QTest::qWaitForWindowExposed(form.get());
    auto tableView = form->findChild<QTableView *>();
    auto editor = form->findChild<ActivityEditorWidget *>();
    auto model = dynamic_cast<ActivityQueryModel *>(form->model());
    QAction *newAction = nullptr;
    QAction *editAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *refreshAction = nullptr;
    for (auto action : form->toolBarActions()) {
        if (action->objectName() == "NEW_RECORD_ACTION") {
            newAction = action;
            continue;
        }
        if (action->objectName() == "EDIT_RECORD_ACTION") {
            editAction = action;
            continue;
        }
        if (action->objectName() == "DELETE_RECORD_ACTION") {
            deleteAction = action;
            continue;
        }
        if (action->objectName() == "REFRESH_ACTION") {
            refreshAction = action;
            continue;
        }
    }

    QVERIFY(tableView != nullptr);
    QVERIFY(editor != nullptr);
    QCOMPARE(tableView->model()->rowCount(), 3);

    QVERIFY(newAction != nullptr);
    QVERIFY(editAction != nullptr);
    QVERIFY(deleteAction != nullptr);
    QVERIFY(refreshAction != nullptr);

    tableView->selectRow(0);
    QApplication::processEvents();
    QVERIFY(newAction->isEnabled());
    QVERIFY(editAction->isEnabled());
    QVERIFY(deleteAction->isEnabled());
    QVERIFY(refreshAction->isEnabled());
}

void TestActivityAdministration::testNameAndTypePage() {
    auto db = QSqlDatabase::database(dbName);
    DBManager manager(dbName);
    QSqlError error;
    db.exec("DELETE FROM ACTIVITY");
    const QVariantList scheduledDates{1, 3, 5};
    Activity activity("A1", ActivityType::EXAM);
    activity.setScheduleType(ActivityScheduleType::ONCE);
    activity.setScheduledDays(scheduledDates);
    activity.setDuration(QTime(3, 0));
    activity.setStartDate(QDate(2016, 8, 15));
    activity.setFinishDate(QDate(2016, 8, 15));
    activity.setCanOverlap(false);
    manager.saveActivity(activity, error);
    activity.setId(0);
    activity.setName("A2");
    activity.setType(ActivityType::INDIVIDUAL_WORK);
    activity.setScheduleType(ActivityScheduleType::WEEK_DAYS);
    activity.setDuration(QTime(1, 30));
    activity.setFinishDate(QDate(2016, 9, 15));
    activity.setCanOverlap(true);
    manager.saveActivity(activity, error);
    activity.setId(0);
    activity.setName("A3");
    activity.setType(ActivityType::LECTURE);
    activity.setScheduleType(ActivityScheduleType::MONTH_DAYS);
    activity.setFinishDate(QDate(2017, 9, 15));
    activity.setCanOverlap(false);
    manager.saveActivity(activity, error);

    QWizard wizard;
    auto page = new ActivityWizardNameAndTypePage;
    page->setActivityId(0);
    wizard.setPage(0, page);
    wizard.show();
    QTest::qWaitForWindowExposed(&wizard);

    QCOMPARE(page->title(), QString("Activity Name and Type"));
    QCOMPARE(page->subTitle(),
             QString("Fill in activity name and select its type."));

    auto nameEdit = page->findChild<QLineEdit *>();
    auto comboBox = page->findChild<QComboBox *>();
    auto overlapCheckBox = page->findChild<QCheckBox *>("overlapCheckBox");
    auto moreThanOnceCheckBox =
        page->findChild<QCheckBox *>("moreThanOnceCheckBox");
    auto groupBox = page->findChild<QGroupBox *>();
    auto weekDaysRadioButton =
        page->findChild<QRadioButton *>("weekDaysRadioButton");
    auto specificDaysRadioButton =
        page->findChild<QRadioButton *>("specificDaysRadioButton");
    QVERIFY(nameEdit != nullptr);
    QVERIFY(comboBox != nullptr);
    QVERIFY(overlapCheckBox != nullptr);
    QVERIFY(moreThanOnceCheckBox != nullptr);
    QVERIFY(groupBox != nullptr);
    QVERIFY(weekDaysRadioButton != nullptr);
    QVERIFY(specificDaysRadioButton != nullptr);

    QCOMPARE(nameEdit->maxLength(), 64);
    for (auto i = 0; i < comboBox->count(); i++) {
        comboBox->setCurrentIndex(i);
        QApplication::processEvents();
        QCOMPARE(overlapCheckBox->isVisible(),
                 comboBox->itemData(i).toString() == "INDIVIDUAL_WORK");
    }
    nameEdit->setText("BLABLA");
    QApplication::processEvents();
    QCOMPARE(wizard.field("name").toString(), QString("BLABLA"));
    QCOMPARE(wizard.field("type").toString(), QString("SPECIAL_EVENT"));
    QVERIFY(!wizard.field("canOverlap").toBool());
    QVERIFY(!wizard.field("moreThanOnce").toBool());
    QVERIFY(wizard.field("onWeekDays").toBool());
    QVERIFY(!wizard.field("onMonthDays").toBool());
    QVERIFY(!groupBox->isVisible());

    // Select individual work
    comboBox->setCurrentIndex(4);
    QApplication::processEvents();
    overlapCheckBox->setChecked(true);
    moreThanOnceCheckBox->setChecked(true);
    QApplication::processEvents();
    QCOMPARE(wizard.field("type").toString(), QString("INDIVIDUAL_WORK"));
    QVERIFY(wizard.field("canOverlap").toBool());
    QVERIFY(wizard.field("moreThanOnce").toBool());
    QVERIFY(groupBox->isVisible());

    moreThanOnceCheckBox->setChecked(false);
    QVERIFY(!wizard.field("moreThanOnce").toBool());
    QVERIFY(!groupBox->isVisible());

    // Select colloquium
    comboBox->setCurrentIndex(2);
    QApplication::processEvents();
    overlapCheckBox->setChecked(true);
    QApplication::processEvents();
    QCOMPARE(wizard.field("type").toString(), QString("COLLOQUIUM"));
    QVERIFY(wizard.field("canOverlap").toBool());

    // Check radio buttons.
    specificDaysRadioButton->setChecked(true);
    QApplication::processEvents();
    QVERIFY(!wizard.field("onWeekDays").toBool());
    QVERIFY(wizard.field("onMonthDays").toBool());

    weekDaysRadioButton->setChecked(true);
    QApplication::processEvents();
    QVERIFY(wizard.field("onWeekDays").toBool());
    QVERIFY(!wizard.field("onMonthDays").toBool());

    wizard.hide();
    QApplication::processEvents();
    wizard.removePage(0);
    delete page;

    page = new ActivityWizardNameAndTypePage;
    page->setActivityId(1);
    wizard.setPage(0, page);
    wizard.show();
    QTest::qWaitForWindowExposed(&wizard);
    nameEdit = page->findChild<QLineEdit *>();
    comboBox = page->findChild<QComboBox *>();
    overlapCheckBox = page->findChild<QCheckBox *>("overlapCheckBox");
    moreThanOnceCheckBox = page->findChild<QCheckBox *>("moreThanOnceCheckBox");
    groupBox = page->findChild<QGroupBox *>();
    weekDaysRadioButton =
        page->findChild<QRadioButton *>("weekDaysRadioButton");
    specificDaysRadioButton =
        page->findChild<QRadioButton *>("specificDaysRadioButton");

    QCOMPARE(nameEdit->text(), QString("A1"));
    QCOMPARE(stringToActivityType(comboBox->currentData().toString()),
             ActivityType::EXAM);
    QVERIFY(!overlapCheckBox->isChecked());
    QVERIFY(!moreThanOnceCheckBox->isChecked());
    QCOMPARE(wizard.field("name").toString(), QString("A1"));
    QCOMPARE(wizard.field("type").toString(), QString("EXAM"));
    QVERIFY(!wizard.field("canOverlap").toBool());
    QVERIFY(!wizard.field("moreThanOnce").toBool());
    QVERIFY(!groupBox->isVisible());
}

void TestActivityAdministration::testFixedDatePage() {
    QWizard wizard;
    auto page = new ActivityWizardFixedDatePage;
    wizard.addPage(page);
    wizard.show();
    QTest::qWaitForWindowExposed(&wizard);

    QCOMPARE(page->title(), QString("Activity Date and Duration"));
    QCOMPARE(page->subTitle(),
             QString("Fill in activity date, time and duration."));

    auto dateTimeEdit = page->findChild<QDateTimeEdit *>("dateTimeEdit");
    auto durationEdit = page->findChild<QTimeEdit *>("durationEdit");
    QVERIFY(dateTimeEdit != nullptr);
    QVERIFY(durationEdit != nullptr);
    QCOMPARE(dateTimeEdit->minimumDate(), QDate::currentDate());
    QCOMPARE(durationEdit->time(), QTime(1, 0, 0));

    dateTimeEdit->setDate(QDate::currentDate().addDays(1));
    QApplication::processEvents();
    QVERIFY(page->isComplete());
    durationEdit->setTime(QTime(0, 0, 0));
    QApplication::processEvents();
    QVERIFY(!page->isComplete());
    dateTimeEdit->setDate(QDate::currentDate().addDays(-2));
    durationEdit->setTime(QTime(2, 0, 0));
    QApplication::processEvents();
    QVERIFY(!page->isComplete());

    QDateTime testDateTime = QDateTime::currentDateTime().addDays(5);
    dateTimeEdit->setDateTime(testDateTime);
    durationEdit->setTime(QTime(0, 45, 0));
    QApplication::processEvents();
    QCOMPARE(wizard.field("selectedDateTime").toDateTime(), testDateTime);
    QCOMPARE(wizard.field("duration").toTime(), QTime(0, 45, 0));
}

void TestActivityAdministration::testRoomsSelectionPage() {
    QWizard wizard;
    auto page = new ActivityWizardRoomsSelectionPage;
    wizard.addPage(page);
    wizard.show();
    QTest::qWaitForWindowExposed(&wizard);
    QCOMPARE(page->title(), QString("Activity Rooms"));
    QCOMPARE(page->subTitle(),
             QString("Select rooms where activity will happen."));

    QSqlError error;
    DBManager manager(dbName);
    auto addRemoveForm = page->findChild<AddRemoveEntitiesForm *>();
    auto sourceTable =
        addRemoveForm->findChild<QTableView *>("sourceTableView");
    auto destinationTable =
        addRemoveForm->findChild<QTableView *>("destinationTableView");
    auto addButton = addRemoveForm->findChild<QPushButton *>("addButton");
    auto removeButton = addRemoveForm->findChild<QPushButton *>("removeButton");

    QCOMPARE(sourceTable->model()->rowCount(), 2);
    QCOMPARE(destinationTable->model()->rowCount(), 0);
    QVERIFY(wizard.field("activityRooms").toList().isEmpty());
    QVERIFY(!page->isComplete());

    sourceTable->selectRow(0);
    QApplication::processEvents();
    addButton->clicked();
    QApplication::processEvents();
    QCOMPARE(wizard.field("activityRooms").toList().size(), 1);
    QVERIFY(page->isComplete());

    destinationTable->selectAll();
    QApplication::processEvents();
    removeButton->clicked();
    QApplication::processEvents();
    QVERIFY(wizard.field("activityRooms").toList().isEmpty());
    QVERIFY(!page->isComplete());
}

void TestActivityAdministration::testListsSelectionPage() {
    QWizard wizard;
    auto page = new ActivityWizardListsSelectionPage;
    wizard.addPage(page);
    wizard.show();
    QTest::qWaitForWindowExposed(&wizard);
    QCOMPARE(page->title(), QString("Activity Lists"));
    QCOMPARE(page->subTitle(),
             QString("Select lists that participate in this activity."));

    QSqlError error;
    DBManager manager(dbName);
    auto addRemoveForm = page->findChild<AddRemoveEntitiesForm *>();
    auto sourceTable =
        addRemoveForm->findChild<QTableView *>("sourceTableView");
    auto destinationTable =
        addRemoveForm->findChild<QTableView *>("destinationTableView");
    auto addButton = addRemoveForm->findChild<QPushButton *>("addButton");
    auto removeButton = addRemoveForm->findChild<QPushButton *>("removeButton");

    QCOMPARE(sourceTable->model()->rowCount(), 6);
    QCOMPARE(destinationTable->model()->rowCount(), 0);
    QVERIFY(wizard.field("activityLists").toList().isEmpty());
    QVERIFY(!page->isComplete());

    sourceTable->selectRow(0);
    QApplication::processEvents();
    addButton->clicked();
    QApplication::processEvents();
    QCOMPARE(wizard.field("activityLists").toList().size(), 1);
    QVERIFY(page->isComplete());

    destinationTable->selectAll();
    QApplication::processEvents();
    removeButton->clicked();
    QApplication::processEvents();
    QVERIFY(wizard.field("activityLists").toList().isEmpty());
    QVERIFY(!page->isComplete());
}

void TestActivityAdministration::testRepetitiveDatesPage() {
    std::unique_ptr<QWizard> wizard(new QWizard);
    auto page = new ActivityWizardRepetitiveDatesPage;
    wizard->addPage(page);
    wizard->show();
    QTest::qWaitForWindowExposed(wizard.get());
    QCOMPARE(page->title(), QString("Activity Dates, Days, Time and Duration"));
    QCOMPARE(
        page->subTitle(),
        QString("Pick activity days or dates and set its time and duration."));

    auto itemsPicker = page->findChild<ItemsPicker *>();
    auto startDateEdit = page->findChild<QDateEdit *>("startDateEdit");
    auto finishDateEdit = page->findChild<QDateEdit *>("finishDateEdit");
    auto durationTimeEdit = page->findChild<QTimeEdit *>("durationTimeEdit");

    // Week mode not set
    QCOMPARE(itemsPicker->findChildren<QCheckBox *>().size(), 31);
    QCOMPARE(startDateEdit->date(), QDate::currentDate());
    QCOMPARE(finishDateEdit->date(), QDate::currentDate().addMonths(1));
    QCOMPARE(durationTimeEdit->time(), QTime(1, 0));
    QVERIFY(!page->isComplete());

    auto item003 = itemsPicker->findChild<QCheckBox *>("item003");
    auto item013 = itemsPicker->findChild<QCheckBox *>("item013");
    auto item023 = itemsPicker->findChild<QCheckBox *>("item023");
    item003->setChecked(true);
    item013->setChecked(true);
    item023->setChecked(true);
    QApplication::processEvents();
    QVERIFY(page->isComplete());
    QCOMPARE(wizard->field("selectedDays").toList().size(), 3);

    finishDateEdit->setDate(QDate::currentDate().addMonths(-5));
    QApplication::processEvents();
    QVERIFY(!page->isComplete());
}

void TestActivityAdministration::testActivityWizard() {
    //    QWizard wizard;
    //    wizard.addPage(new ActivityWizardNameAndTypePage(0));
    //    wizard.addPage(new ActivityWizardFixedDatePage);
    //    wizard.addPage(new ActivityWizardRoomsSelectionPage);
    //    wizard.addPage(new ActivityWizardListsSelectionPage);
    //    wizard.show();
    //    QTest::qWaitForWindowExposed(&wizard);
    //    QTest::qWait(20000);
}
