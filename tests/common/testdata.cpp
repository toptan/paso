#include "testdata.h"

#include "data.h"

#include <QUuid>

using namespace paso::data;

void TestData::testComparingObjectWithItselfIsAlwaysTrue() {
    SystemUser user("user", "user_pass", "John", "Doe", "john.doe@internet.com", SystemRole::MANAGER);
    Room room(QUuid::createUuid().toString(), "Room 42", "42");
    QVERIFY(user == user);
    QVERIFY(room == room);
}
