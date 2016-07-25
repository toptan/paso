#ifndef TESTWIDGETS_H
#define TESTWIDGETS_H

#include <QTest>

class TestWidgets : public QObject {
    Q_OBJECT

public:
    TestWidgets();

private slots:
    void initTestCase();
    void init();
    void cleanup();

    void testAddRemoveEntityWidget();
    void testRecordEditorWidget();
    void testRecordValidator();

private:
    const QString dbName;
};

#endif // TESTWIDGETS_H
