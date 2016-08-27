#include "testactivityadministration.h"

#include "activity.h"
#include "activityeditorwidget.h"
#include "activityform.h"
#include "activitytablemodel.h"
#include "activityvalidator.h"

#include <QAction>
#include <QDate>
#include <QSqlError>
#include <QSqlField>
#include <QTableView>
#include <QTime>

using namespace std;
using namespace paso::data::entity;
using namespace paso::widget;
using namespace paso::admin;

TestActivityAdministration::TestActivityAdministration() : TestBase() {}

void TestActivityAdministration::testActivityValidator() {
    const QSqlRecord record;

    ActivityValidator validator(FieldTypes(), FieldEditors(), this);
    QVERIFY(validator.validate(record) == nullptr);
}

void TestActivityAdministration::testActivityEditorWidget() {
    const FieldTypes fieldTypes{{"name", FieldType::LineEdit},
                                {"type", FieldType::ComboBox},
                                {"schedule", FieldType::LineEdit},
                                {"duration", FieldType::TimeEdit},
                                {"start_date", FieldType::DateEdit},
                                {"finish_date", FieldType::DateEdit}};

    const QVariantMap columnLabels{{"name", "Name"},
                                   {"type", "Type"},
                                   {"schedule", "Schedule"},
                                   {"duration", "Duration"},
                                   {"start_date", "Start date"},
                                   {"finish_date", "Finish date"}};

    QSqlRecord record;
    record.append(QSqlField("id", QVariant::ULongLong));
    record.append(QSqlField("name", QVariant::String));
    record.append(QSqlField("type", QVariant::String));
    record.append(QSqlField("schedule", QVariant::String));
    record.append(QSqlField("duration", QVariant::Time));
    record.append(QSqlField("start_date", QVariant::Date));
    record.append(QSqlField("finish_date", QVariant::Date));
    ActivityEditorWidget editor(fieldTypes);
    editor.setupUi(columnLabels, record);
    editor.onEditNewRecord(record);
    QApplication::processEvents();
    auto nameEdit = dynamic_cast<QLineEdit *>(editor.fieldEditors()["name"]);
    auto typeEdit = dynamic_cast<QComboBox *>(editor.fieldEditors()["type"]);
    auto scheduleEdit =
        dynamic_cast<QLineEdit *>(editor.fieldEditors()["schedule"]);
    auto durationEdit =
        dynamic_cast<QTimeEdit *>(editor.fieldEditors()["duration"]);
    auto startEditor =
        dynamic_cast<QDateEdit *>(editor.fieldEditors()["start_date"]);
    auto finishEditor =
        dynamic_cast<QDateEdit *>(editor.fieldEditors()["finish_date"]);

    QVERIFY(nameEdit->isReadOnly());
    QVERIFY(!typeEdit->isEnabled());
    QVERIFY(scheduleEdit->isReadOnly());
    QVERIFY(durationEdit->isReadOnly());
    QVERIFY(startEditor->isReadOnly());
    QVERIFY(finishEditor->isReadOnly());

    editor.saveSuccessfull();
    QApplication::processEvents();
    editor.onEditExistingRecord(record);
    QApplication::processEvents();
    QVERIFY(nameEdit->isReadOnly());
    QVERIFY(!typeEdit->isEnabled());
    QVERIFY(scheduleEdit->isReadOnly());
    QVERIFY(durationEdit->isReadOnly());
    QVERIFY(startEditor->isReadOnly());
    QVERIFY(finishEditor->isReadOnly());
}

void TestActivityAdministration::testActivityTableModel() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM ACTIVITY");
    db.exec(
        "INSERT INTO ACTIVITY(NAME, TYPE, SCHEDULE, "
        "                     DURATION, START_DATE, FINISH_DATE)"
        "              VALUES('A1', 'EXAM', '0 8 15 8 *',"
        "                     '03:00:00.000', '2016-08-15', '2016-08-15');");
    db.exec(
        "INSERT INTO ACTIVITY(NAME, TYPE, SCHEDULE, "
        "                     DURATION, START_DATE, FINISH_DATE)"
        "              VALUES('A2', 'LAB_EXCERCISE', '0 8 * * 1,3',"
        "                     '01:30:00.000', '2016-09-01', '2016-09-30');");

    const QVariantMap columnLabels{{"name", "Name"},
                                   {"type", "Type"},
                                   {"schedule", "Schedule"},
                                   {"duration", "Duration"},
                                   {"start_date", "Start date"},
                                   {"finish_date", "Finish date"}};

    ActivityTableModel model(columnLabels, db);
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.columnCount(), 7);
    QCOMPARE(model.headerData(0, Qt::Horizontal).toString(), QString("id"));
    QCOMPARE(model.headerData(1, Qt::Horizontal).toString(), QString("Name"));
    QCOMPARE(model.headerData(2, Qt::Horizontal).toString(), QString("Type"));
    QCOMPARE(model.headerData(3, Qt::Horizontal).toString(),
             QString("Schedule"));
    QCOMPARE(model.headerData(4, Qt::Horizontal).toString(),
             QString("Duration"));
    QCOMPARE(model.headerData(5, Qt::Horizontal).toString(),
             QString("Start date"));
    QCOMPARE(model.headerData(6, Qt::Horizontal).toString(),
             QString("Finish date"));

    auto index = model.index(0, 1);
    QCOMPARE(model.data(index).toString(), QString("A1"));
    index = model.index(0, 2);
    QCOMPARE(model.data(index).toString(), QString("Exam"));
    index = model.index(0, 3);
    QCOMPARE(model.data(index).toString(), QString("0 8 15 8 *"));
    index = model.index(0, 4);
    QCOMPARE(model.data(index).toTime(), QTime(3, 0, 0));
    index = model.index(0, 5);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 8, 15));
    index = model.index(0, 6);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 8, 15));

    index = model.index(1, 1);
    QCOMPARE(model.data(index).toString(), QString("A2"));
    index = model.index(1, 2);
    QCOMPARE(model.data(index).toString(), QString("Lab excercise"));
    index = model.index(1, 3);
    QCOMPARE(model.data(index).toString(), QString("0 8 * * 1,3"));
    index = model.index(1, 4);
    QCOMPARE(model.data(index).toTime(), QTime(1, 30, 0));
    index = model.index(1, 5);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 9, 1));
    index = model.index(1, 6);
    QCOMPARE(model.data(index).toDate(), QDate(2016, 9, 30));
}

void TestActivityAdministration::testActivityForm() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM ACTIVITY");
    db.exec(
        "INSERT INTO ACTIVITY(NAME, TYPE, SCHEDULE, "
        "                     DURATION, START_DATE, FINISH_DATE)"
        "              VALUES('A1', 'EXAM', '0 8 15 8 *',"
        "                     '03:00:00.000', '2016-08-15', '2016-08-15');");
    db.exec(
        "INSERT INTO ACTIVITY(NAME, TYPE, SCHEDULE, "
        "                     DURATION, START_DATE, FINISH_DATE)"
        "              VALUES('A2', 'LAB_EXCERCISE', '0 8 * * 1,3',"
        "                     '01:30:00.000', '2016-09-01', '2016-09-30');");

    ActivityForm form;
    form.show();
    QTest::qWaitForWindowExposed(&form);
    auto tableView = form.findChild<QTableView *>();
    auto editor = form.findChild<ActivityEditorWidget *>();
    auto model = dynamic_cast<ActivityTableModel *>(form.model());
    QAction *newAction = nullptr;
    QAction *editAction = nullptr;
    QAction *deleteAction = nullptr;
    QAction *refreshAction = nullptr;
    for (auto action : form.toolBarActions()) {
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
    QCOMPARE(tableView->model()->rowCount(), 2);

    QVERIFY(newAction != nullptr);
    QVERIFY(editAction != nullptr);
    QVERIFY(deleteAction != nullptr);
    QVERIFY(refreshAction != nullptr);
}
