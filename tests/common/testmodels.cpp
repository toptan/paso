#include "testmodels.h"

#include "course.h"
#include "entitytablemodel.h"
#include "refreshablesqlquerymodel.h"
#include "stablerownumbersortfilterproxymodel.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QTextStream>
#include <memory>

using namespace paso::model;
using namespace paso::data::entity;
using namespace std;

TestModels::TestModels() : QObject(), dbName("paso") {}

void TestModels::initTestCase() {
    auto db = QSqlDatabase::addDatabase("QSQLITE", dbName);
    db.setDatabaseName(":memory:");
    db.open();
}

void TestModels::init() {
    auto db = QSqlDatabase::database(dbName);
    QFile in_memory_sql("in_memory.sql");
    in_memory_sql.open(QIODevice::ReadOnly);
    QTextStream in(&in_memory_sql);
    QString sqlString = in.readAll();
    sqlString.replace("\n", "");
    QStringList commands = sqlString.split(";");
    for (const auto &command : commands) {
        db.exec(command);
    }
    in_memory_sql.close();
}

void TestModels::cleanup() {
    auto db = QSqlDatabase::database(dbName);
    db.exec("DROP VIEW ENLISTED_STUDENTS");
    db.exec("DROP TABLE SYSTEM_USER");
    db.exec("DROP TABLE ENLISTED");
    db.exec("DROP TABLE COURSE");
    db.exec("DROP TABLE ROOM");
    db.exec("DROP TABLE STUDENT");
    db.exec("DROP TABLE PERSON");
}

void TestModels::testRefreshableSqlQueryModel() {
    RefreshableSqlQueryModel model("SELECT * FROM COURSE", dbName);
    QCOMPARE(model.rowCount(), 1);
    auto db = QSqlDatabase::database(dbName);
    db.exec("INSERT INTO COURSE(CODE, NAME) VALUES('IR3BP1', 'Baze podataka')");
    model.select();
    QCOMPARE(model.rowCount(), 2);
}

void TestModels::testStableRowNumberSortFilterProxyModel() {
    StableRowNumberSortFilterProxyModel model;
    for (auto i = 0; i < 100; i++) {
        QCOMPARE(model.headerData(i, Qt::Vertical, Qt::DisplayRole).toInt(),
                 i + 1);
        QVERIFY(model.headerData(i, Qt::Horizontal, Qt::DisplayRole).isNull());
    }
}

void TestModels::testEntityModel() {
    EntityVector data;
    data.emplace_back(
        make_shared<Course>("IR3SP", "Sistemsko programiranje", 3));
    data.emplace_back(make_shared<Course>("IR3BP1", "Baze podataka 1", 5));
    QStringList columns{"NAME", "CODE"};
    QMap<QString, QString> columnNames{{"NAME", "Predmet"}, {"CODE", "Šifra"}};
    EntityTableModel model(columns, columnNames, data);
    QCOMPARE(model.columnCount(), 2);
    auto validIndex = model.index(1, 1);
    QCOMPARE(model.columnCount(validIndex), 0);
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.rowCount(validIndex), 0);
    auto index = model.index(0, 0);
    QCOMPARE(model.data(index), QVariant("Sistemsko programiranje"));
    index = model.index(0, 1);
    QCOMPARE(model.data(index), QVariant("IR3SP"));
    index = model.index(1, 0);
    QCOMPARE(model.data(index), QVariant("Baze podataka 1"));
    index = model.index(1, 1);
    QCOMPARE(model.data(index), QVariant("IR3BP1"));
}

void TestModels::testEntityModelDataUpdates() {
    EntityVector data;
    data.emplace_back(
        make_shared<Course>("IR3SP", "Sistemsko programiranje", 3));
    data.emplace_back(make_shared<Course>("IR3BP1", "Baze podataka 1", 5));
    QStringList columns{"NAME", "CODE"};
    QMap<QString, QString> columnNames{{"NAME", "Predmet"}, {"CODE", "Šifra"}};
    EntityTableModel model(columns, columnNames, data);

    shared_ptr<Entity> entity =
        make_shared<Course>("IR4PRS", "Performanse računarskih sistema");

    model.insertEntity(1, entity);
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.entity(1), entity);
    QCOMPARE(model.data(model.index(1, 0)),
             QVariant("Performanse računarskih sistema"));
    QCOMPARE(model.data(model.index(1, 1)), QVariant("IR4PRS"));
    model.removeEntity(1);
    QCOMPARE(model.rowCount(), 2);
    QCOMPARE(model.entity(1), data[1]);
    QCOMPARE(model.data(model.index(1, 0)), QVariant("Baze podataka 1"));
    QCOMPARE(model.data(model.index(1, 1)), QVariant("IR3BP1"));
    model.insertEntity(315, entity);
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.entity(1), data[1]);
    QCOMPARE(model.entity(model.rowCount() - 1), entity);
    QCOMPARE(model.data(model.index(1, 0)), QVariant("Baze podataka 1"));
    QCOMPARE(model.data(model.index(1, 1)), QVariant("IR3BP1"));
}

QTEST_MAIN(TestModels)
