#ifndef TESTMAINWINDOW_H
#define TESTMAINWINDOW_H

#include "testbase.h"
#include <QTest>

class TestMainWindow : public TestBase {
    Q_OBJECT
public:
    TestMainWindow();

private slots:
    void testDbConnectionCreation();
    void testRolesForms();

};

#endif // TESTMAINWINDOW_H
