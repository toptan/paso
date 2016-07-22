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
    EntityVector data{
        make_shared<Course>("IR3SP", "Sistemsko programiranje", 3),
        make_shared<Course>("IR3BP1", "Baze podataka 1", 5)};
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
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
    index = model.index(0, 1);
    QCOMPARE(model.data(index), QVariant("IR3SP"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
    index = model.index(1, 0);
    QCOMPARE(model.data(index), QVariant("Baze podataka 1"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
    index = model.index(1, 1);
    QCOMPARE(model.data(index), QVariant("IR3BP1"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());

    QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("Predmet"));
    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::EditRole), QVariant());
    QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
    QCOMPARE(model.headerData(0, Qt::Vertical, Qt::EditRole), QVariant());
    QCOMPARE(model.headerData(1, Qt::Horizontal), QVariant("Šifra"));
    QCOMPARE(model.headerData(1, Qt::Horizontal, Qt::EditRole), QVariant());
    QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
    QCOMPARE(model.headerData(1, Qt::Vertical, Qt::EditRole), QVariant());

    columns.clear();
    columns.push_back("CODE");
    columnNames.clear();
    columnNames["CODE"] = "Šifra";
    data.clear();
    data.emplace_back(make_shared<Course>("IR4XX", "XX", 3));
    data.emplace_back(make_shared<Course>("IR4YY", "YY", 3));
    data.emplace_back(make_shared<Course>("IR4ZZ", "ZZ", 3));
    model.setEntityData(columns, columnNames, data);
    QCOMPARE(model.columnCount(), 1);
    validIndex = model.index(1, 0);
    QCOMPARE(model.columnCount(validIndex), 0);
    QCOMPARE(model.rowCount(), 3);
    QCOMPARE(model.rowCount(validIndex), 0);
    index = model.index(0, 0);
    QCOMPARE(model.data(index), QVariant("IR4XX"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
    index = model.index(1, 0);
    QCOMPARE(model.data(index), QVariant("IR4YY"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
    index = model.index(2, 0);
    QCOMPARE(model.data(index), QVariant("IR4ZZ"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());

    QCOMPARE(model.headerData(0, Qt::Horizontal), QVariant("Šifra"));
    QCOMPARE(model.headerData(0, Qt::Horizontal, Qt::EditRole), QVariant());
    QCOMPARE(model.headerData(0, Qt::Vertical), QVariant(1));
    QCOMPARE(model.headerData(0, Qt::Vertical, Qt::EditRole), QVariant());
    QCOMPARE(model.headerData(1, Qt::Vertical), QVariant(2));
    QCOMPARE(model.headerData(1, Qt::Vertical, Qt::EditRole), QVariant());
    QCOMPARE(model.headerData(2, Qt::Vertical), QVariant(3));
    QCOMPARE(model.headerData(2, Qt::Vertical, Qt::EditRole), QVariant());
}

void TestModels::testEntityModelDataUpdates() {
    EntityVector data{
        make_shared<Course>("IR3SP", "Sistemsko programiranje", 3),
        make_shared<Course>("IR3BP1", "Baze podataka 1", 5)};
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
    model.removeEntity(make_shared<Course>());
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
    model.removeEntity(entity);
    model.removeEntity(0);
    model.removeEntity(0);
    QCOMPARE(model.rowCount(), 0);
    model.setEntityData(data);
    QCOMPARE(model.columnCount(), 2);
    QCOMPARE(model.rowCount(), 2);
    auto index = model.index(0, 0);
    QCOMPARE(model.data(index), QVariant("Sistemsko programiranje"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
    index = model.index(0, 1);
    QCOMPARE(model.data(index), QVariant("IR3SP"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
    index = model.index(1, 0);
    QCOMPARE(model.data(index), QVariant("Baze podataka 1"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
    index = model.index(1, 1);
    QCOMPARE(model.data(index), QVariant("IR3BP1"));
    QCOMPARE(model.data(index, Qt::EditRole), QVariant());
}

QTEST_MAIN(TestModels)
