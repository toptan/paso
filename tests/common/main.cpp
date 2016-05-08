#include <iostream>

#include <QtTest>
#include "testpasodb.h"

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    TestPasoDB test;
    return QTest::qExec(&test, argc, argv);
}
