#include <QApplication>
#include <QtTest>
#include <list>

#include "testcommdata.h"
#include "testcommmanager.h"
#include "testcourseadministration.h"
#include "testdata.h"
#include "testforms.h"
#include "testlistadministration.h"
#include "testmodels.h"
#include "testpasodb.h"
#include "testroomadministration.h"
#include "testsystemuseradministration.h"
#include "testwidgets.h"

using namespace std;

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    list<QObject *> testCases;

    int status = 0;
    auto ASSERT_TEST = [&status, argc, argv](QObject *obj) {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    testCases.push_back(new TestCommData);
    testCases.push_back(new TestCommManager);
    testCases.push_back(new TestData);
    testCases.push_back(new TestModels);
    testCases.push_back(new TestPasoDB);
    testCases.push_back(new TestForms);
    testCases.push_back(new TestWidgets);
    testCases.push_back(new TestCourseAdministration);
    testCases.push_back(new TestListAdministration);
    testCases.push_back(new TestRoomAdministration);
    testCases.push_back(new TestSystemUserAdministration);

    for (auto testCase : testCases) {
        ASSERT_TEST(testCase);
    }

    return status;
}
