#ifndef TESTSTUDENTDETAILSDIALOG_H
#define TESTSTUDENTDETAILSDIALOG_H

#include "testbase.h"
#include <QTest>

class TestStudentDetailsDialog : public TestBase {
    Q_OBJECT
public:
    TestStudentDetailsDialog();

private slots:
    void testDataRefresh();
};

#endif // TESTSTUDENTDETAILSDIALOG_H
