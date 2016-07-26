#ifndef TESTLISTADMINISTRATION_H
#define TESTLISTADMINISTRATION_H

#include <QTest>

class TestListAdministration : public QObject {
    Q_OBJECT

public:
    TestListAdministration();

private slots:

    void initTestCase();
    void init();
    void cleanup();

    void testListValidator();
    void testListEditorWidget();

private:
    const QString dbName;
};

#endif // TESTLISTADMINISTRATION_H
