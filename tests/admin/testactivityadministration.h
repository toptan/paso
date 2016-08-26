#ifndef TESTACTIVITYADMINISTRATION_H
#define TESTACTIVITYADMINISTRATION_H

#include "testbase.h"
#include <QTest>

class TestActivityAdministration : public TestBase {
    Q_OBJECT

public:
    TestActivityAdministration();

private slots:
    void testActivityTableModel();
};

#endif // TESTACTIVITYADMINISTRATION_H
