#ifndef TESTWIDGETS_H
#define TESTWIDGETS_H

#include <QTest>

class TestWidgets : public QObject {
    Q_OBJECT

private slots:

    void initTestCase();
    void init();
    void cleanup();

    void testAddRemoveEntityWidget();
    void testRecordEditorWidget();
    void testRecordValidator();
};

#endif // TESTWIDGETS_H
