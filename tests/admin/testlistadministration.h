#ifndef TESTLISTADMINISTRATION_H
#define TESTLISTADMINISTRATION_H

#include "testbase.h"
#include <QTest>

class TestListAdministration : public TestBase {
    Q_OBJECT

public:
    TestListAdministration();

private slots:
    void testListValidator();
    void testListEditorWidget();
    void testListTableModel();
    void testListForm();
};

#endif // TESTLISTADMINISTRATION_H
