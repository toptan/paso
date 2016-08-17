#ifndef TESTLISTDETAILSDIALOG_H
#define TESTLISTDETAILSDIALOG_H

#include "testbase.h"
#include <QTest>

class TestListDetailsDialog : public TestBase {
    Q_OBJECT
public:
    TestListDetailsDialog();

private slots:
    void testReadOnly();
    void testDataLoading();
    void testDataRefresh();
    void testWarningWhenDataIsDirty();
};

#endif // TESTLISTDETAILSDIALOG_H
