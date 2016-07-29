#ifndef TESTSYSTEMUSERADMINISTRATION_H
#define TESTSYSTEMUSERADMINISTRATION_H

#include "testbase.h"
#include <QObject>

class TestSystemUserAdministration : public TestBase {
    Q_OBJECT
public:
    TestSystemUserAdministration();

private slots:
    void testSystemUserValidator();
};

#endif // TESTSYSTEMUSERADMINISTRATION_H
