#ifndef TESTWIDGETS_H
#define TESTWIDGETS_H

#include <QTest>

class TestWidgets : public QObject {
    Q_OBJECT

private slots:
    void testAddRemoveEntityWidget();
    void testRecordEditorWidget();
};

#endif // TESTWIDGETS_H
