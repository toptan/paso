#ifndef TESTSTUDENTADMINISTRATION_H
#define TESTSTUDENTADMINISTRATION_H

#include "testbase.h"
#include <QObject>

class TestStudentAdministration : public TestBase {
    Q_OBJECT
public:
    TestStudentAdministration();

private slots:
    void testPersonValidator();
    void testStudentValidator();
    void testStudentEditorWidget();
    void testStudentQueryModel();
    void testStudentForm();
};

#endif // TESTSTUDENTADMINISTRATION_H
