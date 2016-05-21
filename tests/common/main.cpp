#include <iostream>

#include <QtTest>

#include "testpasodb.h"
#include "testdata.h"

int main(int argc, char **argv) {
    int status = 0;
    auto ASSERT_TEST = [&status, argc, argv](QObject *obj) {
        status |= QTest::qExec(obj, argc, argv);
        delete obj;
    };

    ASSERT_TEST(new TestPasoDB());
    ASSERT_TEST(new TestData());

    return status;
}
