#ifndef TESTCOURSEDETAILSDIALOG_H
#define TESTCOURSEDETAILSDIALOG_H

#include "testbase.h"
#include <QTest>

class TestCourseDetailsDialog : public TestBase {
    Q_OBJECT
public:
    TestCourseDetailsDialog();

private slots:
    void testDataLoading();
    void testDataRefresh();
    void testWarningWhenDataIsDirty();
    void testSavingData();
    void testImportCourseStudents();
};

#endif // TESTCOURSEDETAILSDIALOG_H
