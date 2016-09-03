#ifndef TESTACTIVITYADMINISTRATION_H
#define TESTACTIVITYADMINISTRATION_H

#include "testbase.h"
#include <QTest>

class TestActivityAdministration : public TestBase {
    Q_OBJECT

public:
    TestActivityAdministration();

private slots:
    void testActivityValidator();
    void testActivityEditorWidget();
    void testActivityQueryModel();
    void testActivityForm();
    void testNameAndTypePage();
    void testFixedDatePage();
    void testRoomsSelectionPage();
    void testListsSelectionPage();
    void testRepetitiveDatesPage();
    void testActivityWizard();
};

#endif // TESTACTIVITYADMINISTRATION_H
