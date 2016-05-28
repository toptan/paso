CREATE TABLE SYSTEM_USER (
        USERNAME TEXT PRIMARY KEY,
        PASSWORD TEXT NOT NULL,
        FIRST_NAME TEXT,
        LAST_NAME TEXT,
        EMAIL TEXT,
        ROLE TEXT);

CREATE TABLE ROOM (
        ROOM_UUID TEXT PRIMARY KEY,
        NAME TEXT NOT NULL,
        NUMBER TEXT NOT NULL);

INSERT INTO SYSTEM_USER VALUES (
        'admin',
        'admin_password',
        'Admin',
        'Admin',
        'admin@paso.system',
        'ADMINISTRATOR');

INSERT INTO SYSTEM_USER VALUES (
        'room_manager',
        'room_manager_password',
        'Room',
        'Manager',
        'room.manager@paso.system',
        'ROOM_MANAGER');

INSERT INTO SYSTEM_USER VALUES (
        'manager',
        'manager_password',
        'Manager',
        'Managerovic',
        'manager@paso.system',
        'MANAGER');

INSERT INTO SYSTEM_USER VALUES (
        'scheduler',
        'scheduler_password',
        'Scheduler',
        'Schedulerovic',
        'scheduler@paso.system',
        'SCHEDULER');

INSERT INTO ROOM VALUES (
        '{d23a502b-a567-4929-ba99-9f93f36bf4e3}',
        'Laboratorija 42',
        '42');

INSERT INTO ROOM VALUES (
        '{7003528d-4c44-4f91-91b4-b82cb5afb009}',
        'Paviljon 26',
        'P26');
