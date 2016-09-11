#ifndef TESTTEACHERADMINISTRATION_H
#define TESTTEACHERADMINISTRATION_H

#include "testbase.h"
#include <QObject>

class TestTeacherAdministration : public TestBase {
    Q_OBJECT
public:
    TestTeacherAdministration();

private slots:
    void testTeacherValidator();
    void testTeacherEditorWidget();
    void testTeacherQueryModel();
    void testTeacherForm();
};

#endif // TESTTEACHERADMINISTRATION_H
