#ifndef TESTCOURSEADMINISTRATION_H
#define TESTCOURSEADMINISTRATION_H

#include "testbase.h"
#include <QTest>

class TestCourseAdministration : public TestBase {
    Q_OBJECT
public:
    TestCourseAdministration();

private slots:
    void testCourseValidator();
    void testCourseEditorWidget();
    void testCourseTableModel();
};

#endif // TESTCOURSEADMINISTRATION_H
