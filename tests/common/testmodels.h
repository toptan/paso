#ifndef TESTMODELS_H
#define TESTMODELS_H

#include <QTest>

class TestModels : public QObject {
    Q_OBJECT

public:
    TestModels();

private slots:
    void initTestCase();
    void init();
    void cleanup();

    void testStableRowNumberSortFilterProxyModel();
    void testRefreshableSqlQueryModel();
    void testEntityModel();
    void testEntityModelDataUpdates();

private:
    const QString dbName;
};

#endif // TESTMODELS_H
