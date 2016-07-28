#ifndef TESTCOURSEADMINISTRATION_H
#define TESTCOURSEADMINISTRATION_H

#include <QTest>

class TestCourseAdministration : public QObject {
    Q_OBJECT
public:
    TestCourseAdministration();

private slots:

    void initTestCase();
    void init();
    void cleanup();

    void testCourseValidator();
    void testCourseEditorWidget();

private:
    const QString dbName;
};

#endif // TESTCOURSEADMINISTRATION_H
