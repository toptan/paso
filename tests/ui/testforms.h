#ifndef TESTFORMS_H
#define TESTFORMS_H

#include <QTest>

class TestForms : public QObject {
    Q_OBJECT

signals:
    void newRecord();

private slots:

    void init();
    void cleanup();

    void testTableForm();
    void testQueryForm();
};

#endif // TESTFORMS_H
