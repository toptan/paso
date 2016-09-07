#include "testdata.h"

#include "activity.h"
#include "course.h"
#include "data.h"
#include "list.h"
#include "room.h"
#include "student.h"
#include "systemuser.h"

#include <QDebug>
#include <QJsonDocument>
#include <QUuid>
#include <memory>

using namespace std;
using namespace paso::data;
using namespace paso::data::entity;

TestData::TestData() : TestBase() {}

void TestData::testComparingObjectWithItselfIsAlwaysTrue() {
    SystemUser user("user", "user_pass", "John", "Doe", "john.doe@internet.com",
                    SystemRole::MANAGER);
    auto room = new Room(QUuid::createUuid().toString(), "Room 42", "42");
    shared_ptr<Course> course =
        make_shared<Course>("IR3SP", "Sistemsko programiranje");
    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    List list("List name", false, 4);
    Activity activity("Activity 1", ActivityType::LAB_EXCERCISE, 6);
    QCOMPARE(user, user);
    QCOMPARE(*room, *room);
    QCOMPARE(*course, *course);
    QCOMPARE(student, student);
    QCOMPARE(list, list);
    QCOMPARE(activity, activity);
    delete room;
}

void TestData::testComparingStudentWithPersonOrProfessorIsAlwaysFalse() {
    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    Person person("Toplica", "Tanaskovic", "toptan@foo.com", 123, "RRFFIIDD");
    QVERIFY(!(student == person));
}

void TestData::testRoomSerialization() {
    Room expected(QUuid::createUuid().toString(), "Room 42", "42");
    expected.setBarredIds({2, 4, 6});
    QString jsonString = expected.toJsonString();
    Room deserialized("", "", "");
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized, expected);
}

void TestData::testSystemUserSerialization() {
    SystemUser expected("user", "user_pass", "John", "Doe",
                        "john.doe@internet.com", SystemRole::MANAGER);
    QString jsonString = expected.toJsonString();
    SystemUser *deserialized = new SystemUser("");
    deserialized->fromJsonString(jsonString);
    QCOMPARE(*deserialized, expected);
    delete deserialized;
}

void TestData::testSystemRoleSerialization() {
    SystemUser expected("user", "user_pass", "John", "Doe",
                        "john.doe@internet.com", SystemRole::ADMINISTRATOR);
    auto jsonString = expected.toJsonString();
    SystemUser deserialized("");
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::ADMINISTRATOR);
    expected.setRole(SystemRole::MANAGER);
    jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::MANAGER);
    expected.setRole(SystemRole::SUPER_USER);
    jsonString = expected.toJsonString();
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized.role(), SystemRole::SUPER_USER);
}

void TestData::testActivityTypeSerialization() {
    Activity expected("Activity");
    expected.setScheduledDays(QVariantList{1, 3, 5});
    Activity deserialized;
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.type(), ActivityType::INVALID_ACTIVITY);
    expected.setType(ActivityType::LECTURE);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.type(), ActivityType::LECTURE);
    expected.setType(ActivityType::EXAM);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.type(), ActivityType::EXAM);
    expected.setType(ActivityType::COLLOQUIUM);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.type(), ActivityType::COLLOQUIUM);
    expected.setType(ActivityType::LAB_EXCERCISE);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.type(), ActivityType::LAB_EXCERCISE);
    expected.setType(ActivityType::INDIVIDUAL_WORK);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.type(), ActivityType::INDIVIDUAL_WORK);
    expected.setType(ActivityType::SPECIAL_EVENT);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.type(), ActivityType::SPECIAL_EVENT);
}

void TestData::testActivityScheduleTypeSerialization() {
    Activity expected("Activity");
    Activity deserialized;
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.scheduleType(), ActivityScheduleType::INVALID);
    expected.setScheduleType(ActivityScheduleType::ONCE);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.scheduleType(), ActivityScheduleType::ONCE);
    expected.setScheduleType(ActivityScheduleType::WEEK_DAYS);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.scheduleType(), ActivityScheduleType::WEEK_DAYS);
    expected.setScheduleType(ActivityScheduleType::MONTH_DAYS);
    deserialized.fromJsonString(expected.toJsonString());
    QCOMPARE(deserialized.scheduleType(), ActivityScheduleType::MONTH_DAYS);
}

void TestData::testCourseSerialization() {
    Course expected("IR3SP", "Sistemsko programiranje");
    QString jsonString = expected.toJsonString();
    auto deserialized = new Course("", "");
    deserialized->fromJsonString(jsonString);
    QCOMPARE(*deserialized, expected);
    delete deserialized;
}

void TestData::testStudentSerialization() {
    Student expected("Toplica", "Tanaskovic", "toptan@foo.com", "164/96", 5,
                     123, "RRFFIIDD");
    QString jsonString = expected.toJsonString();
    Student deserialized((QVariantMap()));
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized, expected);
}

void TestData::testListSerialization() {
    List expected("Demo lista", true, 4);
    expected.setDemonstrators(true);
    QString jsonString = expected.toJsonString();
    List deserialized((QVariantMap()));
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized, expected);
}

void TestData::testActivitySerialization() {
    Activity expected("Activity 1", ActivityType::INDIVIDUAL_WORK, 6);
    expected.setCanOverlap(true);
    expected.setScheduleType(ActivityScheduleType::MONTH_DAYS);
    QString jsonString = expected.toJsonString();
    Activity deserialized((QVariantMap()));
    deserialized.fromJsonString(jsonString);
    QCOMPARE(deserialized, expected);
}

void TestData::testConversionToVariantMap() {
    SystemUser systemUser("user", "user_pass", "John", "Doe",
                          "john.doe@internet.com", SystemRole::MANAGER);
    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    Course course("IR3SP", "Sistemsko programiranje", 8);
    Room room(QUuid::createUuid().toString(), "Room 42", "42");
    List list("Demo lista", false, 4);
    Activity activity("Activity 1", ActivityType::SPECIAL_EVENT, 8);

    QStringList systemUserKeys{"ID",         "USERNAME",  "PASSWORD",
                               "FIRST_NAME", "LAST_NAME", "EMAIL",
                               "ROLE"};
    QStringList studentKeys{"ID",    "FIRST_NAME",   "LAST_NAME",
                            "EMAIL", "INDEX_NUMBER", "YEAR_OF_STUDY",
                            "RFID"};
    QStringList courseKeys{"ID", "CODE", "NAME"};
    QStringList roomKeys{"ID", "ROOM_UUID", "NAME", "ROOM_NUMBER",
                         "BARRED_STUDENTS"};
    QStringList listKeys{"ID",        "NAME",          "SYSTEM",
                         "PERMANENT", "DEMONSTRATORS", "EXPIRY_DATE"};
    QStringList activityKeys{"ID",
                             "NAME",
                             "TYPE",
                             "SCHEDULE_TYPE",
                             "SCHEDULED_DAYS",
                             "DURATION",
                             "START_DATE",
                             "START_TIME",
                             "FINISH_DATE",
                             "CAN_OVERLAP",
                             "ACTIVITY_ROOMS",
                             "ACTIVITY_LISTS"};

    QCOMPARE(systemUser.toVariantMap().keys().size(), systemUserKeys.size());
    QCOMPARE(student.toVariantMap().keys().size(), studentKeys.size());
    QCOMPARE(course.toVariantMap().keys().size(), courseKeys.size());
    QCOMPARE(room.toVariantMap().keys().size(), roomKeys.size());
    QCOMPARE(list.toVariantMap().keys().size(), listKeys.size());
    QCOMPARE(activity.toVariantMap().keys().size(), activityKeys.size());

    for (const auto &key : systemUser.toVariantMap().keys()) {
        systemUserKeys.removeOne(key);
    }
    QVERIFY(systemUserKeys.empty());
    for (const auto &key : student.toVariantMap().keys()) {
        studentKeys.removeOne(key);
    }
    QVERIFY(studentKeys.empty());
    for (const auto &key : course.toVariantMap().keys()) {
        courseKeys.removeOne(key);
    }
    QVERIFY(courseKeys.empty());
    for (const auto &key : room.toVariantMap().keys()) {
        roomKeys.removeOne(key);
    }
    QVERIFY(roomKeys.empty());
    for (const auto &key : list.toVariantMap().keys()) {
        listKeys.removeOne(key);
    }
    QVERIFY(listKeys.empty());
    for (const auto &key : activity.toVariantMap().keys()) {
        activityKeys.removeOne(key);
    }
    QVERIFY(activityKeys.empty());
}

void TestData::testPropertyValues() {
    SystemUser systemUser("user", "user_pass", "John", "Doe",
                          "john.doe@internet.com", SystemRole::MANAGER, 6);
    QCOMPARE(systemUser.value("FOO"), QVariant());
    QCOMPARE(systemUser.value("ID"), QVariant(systemUser.id()));
    QCOMPARE(systemUser.value("USERNAME"), QVariant(systemUser.username()));
    QCOMPARE(systemUser.value("PASSWORD"), QVariant(systemUser.password()));
    QCOMPARE(systemUser.value("FIRST_NAME"), QVariant(systemUser.firstName()));
    QCOMPARE(systemUser.value("LAST_NAME"), QVariant(systemUser.lastName()));
    QCOMPARE(systemUser.value("EMAIL"), QVariant(systemUser.email()));
    QCOMPARE(systemUser.value("ROLE"),
             QVariant(roleToString(systemUser.role())));

    Student student("Toplica", "Tanasković", "toptan@foo.com", "164/96", 5, 123,
                    "RRFFIIDD");
    QCOMPARE(student.value("FOO"), QVariant());
    QCOMPARE(student.value("ID"), QVariant(student.id()));
    QCOMPARE(student.value("FIRST_NAME"), QVariant(student.firstName()));
    QCOMPARE(student.value("LAST_NAME"), QVariant(student.lastName()));
    QCOMPARE(student.value("EMAIL"), QVariant(student.email()));
    QCOMPARE(student.value("INDEX_NUMBER"), QVariant(student.indexNumber()));
    QCOMPARE(student.value("YEAR_OF_STUDY"), QVariant(student.yearOfStudy()));
    QCOMPARE(student.value("RFID"), QVariant(student.rfid()));

    Course course("IR3SP", "Sistemsko programiranje", 8);
    QCOMPARE(course.value("FOO"), QVariant());
    QCOMPARE(course.value("ID"), QVariant(course.id()));
    QCOMPARE(course.value("CODE"), QVariant(course.code()));
    QCOMPARE(course.value("NAME"), QVariant(course.name()));

    QString uuid = QUuid::createUuid().toString();
    Room room(uuid, "Room 42", "42");
    room.setBarredIds({1, 2, 3});
    QCOMPARE(room.value("FOO"), QVariant());
    QCOMPARE(room.value("ID"), QVariant(room.id()));
    QCOMPARE(room.value("ROOM_UUID"), QVariant(uuid));
    QCOMPARE(room.value("ROOM_UUID"), QVariant(room.roomUUID()));
    QCOMPARE(room.value("NAME"), QVariant(room.name()));
    QCOMPARE(room.value("ROOM_NUMBER"), QVariant(room.number()));
    QCOMPARE(room.value("BARRED_STUDENTS"), QVariant(room.barredIds()));

    List list;
    list.setName("Demo lista");
    list.setPermanent(true);
    list.setId(4);
    list.setExpiryDate(QDate());
    list.setDemonstrators(true);
    QCOMPARE(list.value("FOO"), QVariant());
    QCOMPARE(list.value("ID"), QVariant(list.id()));
    QCOMPARE(list.value("NAME"), QVariant(list.name()));
    QCOMPARE(list.value("SYSTEM"), QVariant(list.system()));
    QCOMPARE(list.value("PERMANENT"), QVariant(list.permanent()));
    QCOMPARE(list.value("DEMONSTRATORS"), QVariant(list.demonstrators()));
    QCOMPARE(list.value("EXPIRY_DATE"), QVariant::fromValue(QDate()));

    Activity activity;
    activity.setId(42);
    activity.setName("Activity 1");
    activity.setType(ActivityType::INDIVIDUAL_WORK);
    activity.setScheduleType(ActivityScheduleType::WEEK_DAYS);
    activity.setDuration(QTime(1, 30));
    activity.setStartDate(QDate::currentDate().addMonths(-1));
    activity.setStartTime(QTime(8, 0));
    activity.setFinishDate(QDate::currentDate().addMonths(1));
    activity.setCanOverlap(true);
    activity.setRoomIds({1, 2, 3});
    activity.setListIds({7, 8, 9});

    QCOMPARE(activity["FOO"], QVariant());
    QCOMPARE(activity["ID"], QVariant(activity.id()));
    QCOMPARE(activity["NAME"], QVariant(activity.name()));
    QCOMPARE(activity["TYPE"], QVariant(activityTypeToString(activity.type())));
    QCOMPARE(activity["SCHEDULE_TYPE"],
             QVariant(activityScheduleTypeToString(activity.scheduleType())));
    QCOMPARE(activity["SCHEDULED_DAYS"], QVariant(activity.scheduledDays()));
    QCOMPARE(activity["DURATION"], QVariant(activity.duration()));
    QCOMPARE(activity["START_DATE"], QVariant(activity.startDate()));
    QCOMPARE(activity["START_TIME"], QVariant(activity.startTime()));
    QCOMPARE(activity["FINISH_DATE"], QVariant(activity.finishDate()));
    QCOMPARE(activity["CAN_OVERLAP"], QVariant(activity.canOverlap()));
    QCOMPARE(activity["ACTIVITY_ROOMS"], QVariant(activity.roomIds()));
    QCOMPARE(activity["ACTIVITY_LISTS"], QVariant(activity.listIds()));
}

void TestData::testDifferenceInMonths() {
    QDate startDate(2016, 8, 3);
    QDate endDate(2016, 8, 24);
    QCOMPARE(differenceInMonths(startDate, endDate), 0);

    endDate.setDate(2016, 11, 24);
    QCOMPARE(differenceInMonths(endDate, startDate), 0);
    QCOMPARE(differenceInMonths(startDate, endDate), 3);

    endDate.setDate(2017, 3, 15);
    QCOMPARE(differenceInMonths(startDate, endDate), 7);

    endDate.setDate(2017, 10, 22);
    QCOMPARE(differenceInMonths(startDate, endDate), 14);
}

void TestData::testScheduledDates() {
    QDate startDate(2016, 8, 10);
    QDate endDate(2016, 8, 20);

    QString cronString("10 13 6 11 *");
    QVERIFY(scheduledDates(cronString, endDate, startDate).empty());

    cronString = "asfgasfh";
    QVERIFY(scheduledDates(cronString, startDate, endDate).empty());

    cronString = "10 15 16 8 0,2";
    QVERIFY(scheduledDates(cronString, startDate, endDate).empty());

    cronString = "* 15 * * *";
    QVERIFY(scheduledDates(cronString, startDate, endDate).empty());

    cronString = "20 -1 * * *";
    QVERIFY(scheduledDates(cronString, startDate, endDate).empty());

    cronString = "20 26 * * *";
    QVERIFY(scheduledDates(cronString, startDate, endDate).empty());

    cronString = "60 15 * * *";
    QVERIFY(scheduledDates(cronString, startDate, endDate).empty());

    cronString = "20 15 * * *";
    auto dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(11));

    cronString = "15 8,16 * * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(22));

    cronString = "15 4,12,20 * * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(33));

    cronString = "15 12 3 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    cronString = "15 12 sfg * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    cronString = "15 12 33 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    cronString = "15 12 25-33 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    cronString = "15 12 14 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(1));

    cronString = "15 12 14,16,18 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(3));

    cronString = "15 12 10,16,20 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(3));

    cronString = "15 12 6,10,16,20,25 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(3));

    cronString = "15 12 10-16,20 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(8));

    cronString = "15 12,15 10-16,20 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(16));

    cronString = "0 12 5-16 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(7));

    cronString = "0 12 5-12,15,18-27 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(7));

    cronString = "0 12,16 5-12,15,18-27 * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(14));

    cronString = "0 12 15 z9 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    cronString = "0 12 15 9 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    cronString = "0 12 15 9-15 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    cronString = "0 12 15 8 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(1));

    startDate = startDate.addMonths(-2); //<! 10.06.2016.
    endDate = endDate.addMonths(2);      //<! 20.10.2016.
    cronString = "0 12 15 7,9 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(2));

    cronString = "0 12 15 7-9 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(3));

    cronString = "0 12 15 5-7 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(2));

    cronString = "0 12 15 3-5,7,8-9,13 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    cronString = "0 12 15 3-5,7,8-9,12 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(3));

    cronString = "0 12 15-18 3-5,7,8-9,12 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(12));

    cronString = "0 12,16 15-18 3-5,7,8-9,12 *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(24));

    startDate = startDate.addMonths(2); //<! 10.08.2016.
    endDate = endDate.addMonths(-2);    //<! 20.08.2016.
    cronString = "0 12 * 8 0,3";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(3));

    cronString = "0 12 * 8 0,1,3";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(4));

    endDate = endDate.addMonths(1); //<! 20.09.2016.
    cronString = "0 10 * * 1,3,5";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(18));

    cronString = "0 10,15 * * 1,3,5";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(36));

    cronString = "0 10,15 * 8-10 1,3,5";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(36));

    cronString = "0 10,15 * 8-10 1-5";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(0));

    startDate.setDate(2016, 2, 25);
    endDate.setDate(2016, 3, 3);
    cronString = "30 12 * * *";
    dates = scheduledDates(cronString, startDate, endDate);
    QCOMPARE(dates.size(), size_t(8));
}

void TestData::testJsonArrayStringToVariantList() {
    auto result = jsonArrayStringToVariantList("{1, 2, 3}");
    QCOMPARE(result.size(), 3);
    QCOMPARE(result[0].toInt(), 1);
    QCOMPARE(result[1].toInt(), 2);
    QCOMPARE(result[2].toInt(), 3);

    result = jsonArrayStringToVariantList("{\"AB\", \"CD\"}");
    QCOMPARE(result.size(), 2);
    QCOMPARE(result[0].toString(), QString("AB"));
    QCOMPARE(result[1].toString(), QString("CD"));

    result = jsonArrayStringToVariantList("1, \"AB\", 2, \"cd\"}");
    // All array members have to be of the same type.
    QCOMPARE(result.size(), 0);
}

void TestData::testVariantListToJsonArrayString() {
    QVariantList l{1, 2, 3};
    QCOMPARE(variantListToJsonArrayString(l), QString("{1,2,3}"));
    l.clear();
    l << "AB"
      << "CD";
    QCOMPARE(variantListToJsonArrayString(l), QString("{\"AB\",\"CD\"}"));
}
