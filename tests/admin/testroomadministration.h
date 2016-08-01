#ifndef TESTROOMADMINISTRATION_H
#define TESTROOMADMINISTRATION_H

#include "testbase.h"
#include <QTest>

class TestRoomAdministration : public TestBase {
    Q_OBJECT
public:
    TestRoomAdministration();

private slots:
    void testRoomValidator();
    void testRoomEditorWidget();
    void testRoomTableModel();
    void testRoomForm();
};

#endif // TESTROOMADMINISTRATION_H
