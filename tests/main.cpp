#include <QApplication>
#include <QtTest>
#include <list>

#include "testactivityadministration.h"
#include "testcommdata.h"
#include "testcommmanager.h"
#include "testcourseadministration.h"
#include "testcoursedetailsdialog.h"
#include "testdata.h"
#include "testforms.h"
#include "testlistadministration.h"
#include "testlistdetailsdialog.h"
#include "testlogindialog.h"
#include "testmainwindow.h"
#include "testmodels.h"
#include "testpasodb.h"
#include "testroomadministration.h"
#include "teststudentadministration.h"
#include "teststudentdetailsdialog.h"
#include "testsystemuseradministration.h"
#include "testwidgets.h"

using namespace std;

QT_BEGIN_NAMESPACE
QTEST_ADD_GPU_BLACKLIST_SUPPORT_DEFS
QT_END_NAMESPACE

int main(int argc, char **argv) {
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);
    QTEST_DISABLE_KEYPAD_NAVIGATION
    QTEST_ADD_GPU_BLACKLIST_SUPPORT
    QTEST_SET_MAIN_SOURCE_PATH
    qRegisterMetaType<paso::comm::LoginResponse>("LoginResponse");
    qRegisterMetaType<QList<QPersistentModelIndex>>();
    qRegisterMetaType<QAbstractItemModel::LayoutChangeHint>();
    list<QObject *> testCases;

    int status = 0;
    auto ASSERT_TEST = [&status, argc, argv](QObject *obj) {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    testCases.push_back(new TestLoginDialog);
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
    testCases.push_back(new TestStudentAdministration);
    testCases.push_back(new TestCourseDetailsDialog);
    testCases.push_back(new TestListDetailsDialog);
    testCases.push_back(new TestStudentDetailsDialog);
    testCases.push_back(new TestMainWindow);
    testCases.push_back(new TestActivityAdministration);

    for (auto testCase : testCases) {
        ASSERT_TEST(testCase);
    }

    TestBase::printTestStats();
    return status;
}
