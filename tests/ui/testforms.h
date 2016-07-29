#ifndef TESTFORMS_H
#define TESTFORMS_H

#include "testbase.h"
#include <QTest>

class TestForms : public TestBase {
    Q_OBJECT

public:
    TestForms();

private slots:
    void testTableForm();
    void testQueryForm();
};

#endif // TESTFORMS_H
