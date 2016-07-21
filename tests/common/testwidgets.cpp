#include "testwidgets.h"

#include "addremoveentitiesform.h"
#include "course.h"
#include "entity.h"

#include <QDebug>
#include <QPushButton>
#include <QSignalSpy>
#include <QTableView>

using namespace std;
using namespace paso::data::entity;
using namespace paso::widget;

void TestWidgets::testAddRemoveEntityWidget() {
    AddRemoveEntitiesForm form;
    QStringList columns{"NAME", "CODE"};
    QMap<QString, QString> columnNames{{"NAME", "Predmet"}, {"CODE", "Šifra"}};

    EntityVector sourceData{
        make_shared<Course>("IR3SP", "Sistemsko programiranje", 3),
        make_shared<Course>("IR3BP1", "Baze podataka 1", 5)};
    EntityVector destinationData;

    form.setData("Source", columns, columnNames, sourceData, "Destination",
                 columns, columnNames, destinationData);
    auto sourceTable = form.findChild<QTableView *>("sourceTableView");
    auto destinationTable =
        form.findChild<QTableView *>("destinationTableView");
    auto addButton = form.findChild<QPushButton *>("addButton");
    auto removeButton = form.findChild<QPushButton *>("removeButton");
    auto resetButton = form.findChild<QPushButton *>("resetButton");

    sourceTable->selectRow(0);
    QTest::mouseClick(addButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(1));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    sourceTable->clearSelection();
    QTest::mouseClick(addButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(1));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    sourceTable->selectRow(0);
    QTest::mouseClick(addButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(2));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    destinationTable->clearSelection();
    QTest::mouseClick(removeButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(2));
    QVERIFY(form.removedEntities().empty());
    QVERIFY(form.dirty());

    destinationTable->selectRow(0);
    QTest::mouseClick(removeButton, Qt::LeftButton);
    QCOMPARE(form.addedEntities().size(), size_t(1));
    QCOMPARE(form.removedEntities().size(), size_t(1));
    QVERIFY(form.dirty());

    QTest::mouseClick(resetButton, Qt::LeftButton);
    QVERIFY(form.addedEntities().empty());
    QVERIFY(form.removedEntities().empty());
    QVERIFY(!form.dirty());
}

QTEST_MAIN(TestWidgets)
