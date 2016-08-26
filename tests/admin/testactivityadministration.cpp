#include "testactivityadministration.h"

#include "activity.h"
#include "activitytablemodel.h"

#include <QDate>
#include <QSqlError>
#include <QTime>

using namespace std;
using namespace paso::data::entity;
// using namespace paso::widget;
using namespace paso::admin;

TestActivityAdministration::TestActivityAdministration() : TestBase() {}

void TestActivityAdministration::testActivityTableModel() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DELETE FROM ACTIVITY");
    db.exec("INSERT INTO ACTIVITY(NAME, TYPE, SCHEDULE, "
            "                     DURATION, START_DATE, FINISH_DATE)"
            "              VALUES('A1', 'EXAM', '0 8 15 8 *',"
            "                     '03:00:00.000', '2016-08-15', '2016-08-15');");
    db.exec("INSERT INTO ACTIVITY(NAME, TYPE, SCHEDULE, "
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
