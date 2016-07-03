CREATE TABLE SYSTEM_USER (
        ID         INTEGER PRIMARY KEY AUTOINCREMENT,
        USERNAME   TEXT UNIQUE NOT NULL,
        PASSWORD   TEXT NOT NULL,
        FIRST_NAME TEXT NOT NULL,
        LAST_NAME  TEXT NOT NULL,
        EMAIL      TEXT NOT NULL,
        ROLE       TEXT NOT NULL);

CREATE TABLE COURSE (
        ID   INTEGER PRIMARY KEY AUTOINCREMENT,
        CODE TEXT UNIQUE NOT NULL,
        NAME TEXT NOT NULL);

CREATE TABLE ROOM (
        ID          INTEGER PRIMARY KEY AUTOINCREMENT,
        ROOM_UUID   TEXT UNIQUE NOT NULL,
        NAME        TEXT NOT NULL,
        ROOM_NUMBER TEXT UNIQUE NOT NULL);

CREATE TABLE PERSON (
        ID          INTEGER PRIMARY KEY AUTOINCREMENT,
        FIRST_NAME  TEXT NOT NULL,
        LAST_NAME   TEXT NOT NULL,
        EMAIL       TEXT NOT NULL,
        RFID        TEXT);

CREATE TABLE STUDENT (
        ID              INTEGER PRIMARY KEY,
        INDEX_NUMBER    TEXT UNIQUE NOT NULL,
        YEAR_OF_STUDY   INTEGER NOT NULL,
        FOREIGN KEY(ID) REFERENCES PERSON(ID) ON DELETE CASCADE ON UPDATE CASCADE);

INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
       VALUES (
        'admin',
        'admin_password',
        'Admin',
        'Admin',
        'admin@paso.system',
        'ADMINISTRATOR');

INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
       VALUES (
        'room_manager',
        'room_manager_password',
        'Room',
        'Manager',
        'room.manager@paso.system',
        'ROOM_MANAGER');

INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
       VALUES (
        'manager',
        'manager_password',
        'Manager',
        'Managerovic',
        'manager@paso.system',
        'MANAGER');

INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
       VALUES (
        'scheduler',
        'scheduler_password',
        'Scheduler',
        'Schedulerovic',
        'scheduler@paso.system',
        'SCHEDULER');

INSERT INTO ROOM (ROOM_UUID, NAME, ROOM_NUMBER) VALUES (
        '{d23a502b-a567-4929-ba99-9f93f36bf4e3}',
        'Laboratorija 42',
        '42');

INSERT INTO ROOM (ROOM_UUID, NAME, ROOM_NUMBER) VALUES (
        '{7003528d-4c44-4f91-91b4-b82cb5afb009}',
        'Paviljon 26',
        'P26');

INSERT INTO COURSE (CODE, NAME) VALUES (
        'IR3SP',
        'Sistemsko programiranje');

INSERT INTO PERSON(ID, FIRST_NAME, LAST_NAME, EMAIL, RFID) VALUES (
        1,
        'Toplica',
        'Tanasković',
        'toptan@foo.com',
        'RRFFIIDD');

INSERT INTO STUDENT(ID, INDEX_NUMBER, YEAR_OF_STUDY) VALUES (
        1,
        '164/96',
        5);
