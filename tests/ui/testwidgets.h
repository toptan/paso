#ifndef TESTWIDGETS_H
#define TESTWIDGETS_H

#include "testbase.h"
#include <QTest>

class TestWidgets : public TestBase {
    Q_OBJECT

public:
    TestWidgets();

private slots:
    void testAddRemoveEntityWidget();
    void testRecordEditorWidget();
    void testRecordValidator();
};

#endif // TESTWIDGETS_H
