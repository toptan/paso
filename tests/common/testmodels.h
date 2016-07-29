#ifndef TESTMODELS_H
#define TESTMODELS_H

#include "testbase.h"
#include <QTest>

class TestModels : public TestBase {
    Q_OBJECT

public:
    TestModels();

private slots:
    void testStableRowNumberSortFilterProxyModel();
    void testRefreshableSqlQueryModel();
    void testEntityModel();
    void testEntityModelDataUpdates();
};

#endif // TESTMODELS_H
