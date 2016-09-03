-- CREATING TABLES --
CREATE TABLE SYSTEM_USER (
    ID         BIGSERIAL PRIMARY KEY,
    USERNAME   VARCHAR(16) UNIQUE NOT NULL,
    PASSWORD   VARCHAR(32)        NOT NULL,
    FIRST_NAME VARCHAR(32)        NOT NULL,
    LAST_NAME  VARCHAR(32)        NOT NULL,
    EMAIL      VARCHAR(64)        NOT NULL,
    ROLE       VARCHAR(16)        NOT NULL
);

CREATE TABLE COURSE (
    ID   BIGSERIAL PRIMARY KEY,
    CODE VARCHAR(8) UNIQUE NOT NULL,
    NAME VARCHAR(64)       NOT NULL
);

CREATE TABLE ROOM (
    ID          BIGSERIAL PRIMARY KEY,
    NAME        VARCHAR(64)       NOT NULL,
    ROOM_NUMBER VARCHAR(8) UNIQUE NOT NULL,
    ROOM_UUID   UUID UNIQUE       NOT NULL
);

CREATE TABLE PERSON (
    ID         BIGSERIAL PRIMARY KEY,
    LAST_NAME  VARCHAR(32) NOT NULL,
    FIRST_NAME VARCHAR(32) NOT NULL,
    EMAIL      VARCHAR(64) UNIQUE,
    RFID       VARCHAR(64) UNIQUE
);

CREATE TABLE STUDENT (
    ID            BIGINT PRIMARY KEY,
    INDEX_NUMBER  VARCHAR(9) UNIQUE NOT NULL,
    YEAR_OF_STUDY INTEGER           NOT NULL,
    FOREIGN KEY (ID) REFERENCES PERSON (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE LIST (
    ID          BIGSERIAL PRIMARY KEY,
    NAME        VARCHAR(64) UNIQUE NOT NULL,
    SYSTEM      BOOLEAN            NOT NULL DEFAULT FALSE,
    PERMANENT   BOOLEAN            NOT NULL DEFAULT FALSE,
    EXPIRY_DATE DATE,
    ID_COURSE   BIGINT UNIQUE,
    FOREIGN KEY (ID_COURSE) REFERENCES COURSE (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE ENLISTED (
    ID_STUDENT BIGINT NOT NULL,
    ID_COURSE  BIGINT NOT NULL,
    PRIMARY KEY (ID_STUDENT, ID_COURSE),
    FOREIGN KEY (ID_STUDENT) REFERENCES STUDENT (ID) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (ID_COURSE) REFERENCES COURSE (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE MEMBER (
    ID_STUDENT INTEGER NOT NULL,
    ID_LIST    INTEGER NOT NULL,
    PRIMARY KEY (ID_STUDENT, ID_LIST),
    FOREIGN KEY (ID_STUDENT) REFERENCES STUDENT (ID) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (ID_LIST) REFERENCES LIST (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE ACTIVITY (
    ID             BIGSERIAL PRIMARY KEY,
    NAME           VARCHAR(64) NOT NULL,
    TYPE           VARCHAR(16) NOT NULL,
    SCHEDULE_TYPE  VARCHAR(16) NOT NULL,
    SCHEDULED_DAYS INTEGER []           DEFAULT NULL,
    DURATION       TIME        NOT NULL,
    START_DATE     DATE        NOT NULL,
    START_TIME     TIME        NOT NULL,
    FINISH_DATE    DATE        NOT NULL,
    CAN_OVERLAP    BOOLEAN     NOT NULL DEFAULT FALSE
);

CREATE TABLE ACTIVITY_LISTS (
    ID_ACTIVITY INTEGER NOT NULL,
    ID_LIST     INTEGER NOT NULL,
    PRIMARY KEY (ID_ACTIVITY, ID_LIST),
    FOREIGN KEY (ID_ACTIVITY) REFERENCES ACTIVITY (ID) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (ID_LIST) REFERENCES LIST (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE ACTIVITY_ROOMS (
    ID_ACTIVITY INTEGER NOT NULL,
    ID_ROOM     INTEGER NOT NULL,
    PRIMARY KEY (ID_ACTIVITY, ID_ROOM),
    FOREIGN KEY (ID_ACTIVITY) REFERENCES ACTIVITY (ID) ON DELETE CASCADE ON UPDATE CASCADE,
    FOREIGN KEY (ID_ROOM) REFERENCES ROOM (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE VIEW ENLISTED_STUDENTS AS
    SELECT
        P.ID,
        P.LAST_NAME,
        P.FIRST_NAME,
        P.EMAIL,
        P.RFID,
        S.INDEX_NUMBER,
        S.YEAR_OF_STUDY,
        C.CODE
    FROM PERSON P
        JOIN STUDENT S USING (ID)
        LEFT OUTER JOIN ENLISTED E ON E.ID_STUDENT = S.ID
        LEFT OUTER JOIN COURSE C ON E.ID_COURSE = C.ID;

CREATE VIEW LIST_MEMBERS AS
    SELECT
        P.ID,
        P.LAST_NAME,
        P.FIRST_NAME,
        P.EMAIL,
        P.RFID,
        S.INDEX_NUMBER,
        S.YEAR_OF_STUDY,
        L.ID AS LIST_ID
    FROM PERSON P
        JOIN STUDENT S USING (ID)
        JOIN MEMBER M ON M.ID_STUDENT = S.ID
        JOIN LIST L ON L.ID = M.ID_LIST
    UNION
    SELECT
        P.ID,
        P.LAST_NAME,
        P.FIRST_NAME,
        P.EMAIL,
        P.RFID,
        S.INDEX_NUMBER,
        S.YEAR_OF_STUDY,
        1
    FROM PERSON P
        JOIN STUDENT S USING (ID)
    WHERE S.YEAR_OF_STUDY = 1
    UNION
    SELECT
        P.ID,
        P.LAST_NAME,
        P.FIRST_NAME,
        P.EMAIL,
        P.RFID,
        S.INDEX_NUMBER,
        S.YEAR_OF_STUDY,
        2
    FROM PERSON P
        JOIN STUDENT S USING (ID)
    WHERE S.YEAR_OF_STUDY = 2
    UNION
    SELECT
        P.ID,
        P.LAST_NAME,
        P.FIRST_NAME,
        P.EMAIL,
        P.RFID,
        S.INDEX_NUMBER,
        S.YEAR_OF_STUDY,
        3
    FROM PERSON P
        JOIN STUDENT S USING (ID)
    WHERE S.YEAR_OF_STUDY = 3
    UNION
    SELECT
        P.ID,
        P.LAST_NAME,
        P.FIRST_NAME,
        P.EMAIL,
        P.RFID,
        S.INDEX_NUMBER,
        S.YEAR_OF_STUDY,
        4
    FROM PERSON P
        JOIN STUDENT S USING (ID)
    WHERE S.YEAR_OF_STUDY = 4
    UNION
    SELECT
        P.ID,
        P.LAST_NAME,
        P.FIRST_NAME,
        P.EMAIL,
        P.RFID,
        S.INDEX_NUMBER,
        S.YEAR_OF_STUDY,
        5
    FROM PERSON P
        JOIN STUDENT S USING (ID)
    WHERE S.YEAR_OF_STUDY = 5;

-- After insert on course
CREATE OR REPLACE FUNCTION a_i_course_function()
    RETURNS TRIGGER LANGUAGE plpgsql
AS $BODY$
BEGIN
    INSERT INTO LIST (NAME, SYSTEM, PERMANENT, ID_COURSE)
    VALUES (NEW.CODE || ' студенти', TRUE, TRUE, NEW.ID);

    RETURN NEW;
END;
$BODY$;

CREATE TRIGGER a_i_course AFTER INSERT ON COURSE FOR EACH ROW
EXECUTE PROCEDURE a_i_course_function();

-- After update on course
CREATE OR REPLACE FUNCTION A_U_COURSE_FUNCTION()
    RETURNS TRIGGER LANGUAGE plpgsql AS
$BODY$
BEGIN
    IF NEW.CODE <> OLD.CODE
    THEN
        UPDATE LIST
        SET NAME = NEW.CODE || ' студенти'
        WHERE ID_COURSE = NEW.ID;
    END IF;

    RETURN NEW;
END;
$BODY$;

CREATE TRIGGER A_U_COURSE AFTER UPDATE ON COURSE FOR EACH ROW
EXECUTE PROCEDURE A_U_COURSE_FUNCTION();

-- Before insert and update on list
CREATE OR REPLACE FUNCTION B_IU_LIST_FUNCTION()
    RETURNS TRIGGER LANGUAGE plpgsql AS
$BODY$
BEGIN
    IF NEW.SYSTEM = TRUE OR NEW.PERMANENT = TRUE
    THEN
        NEW.EXPIRY_DATE = NULL;
    END IF;

    RETURN NEW;
END;
$BODY$;

CREATE TRIGGER B_IU_LIST BEFORE INSERT OR UPDATE ON LIST FOR EACH ROW
EXECUTE PROCEDURE B_IU_LIST_FUNCTION();

CREATE OR REPLACE FUNCTION insert_activity(a_name           VARCHAR, a_type VARCHAR, a_schedule_type VARCHAR,
                                           a_scheduled_days TEXT,
                                           a_duration       TIME, a_start_date DATE, a_start_time TIME,
                                           a_finish_date    DATE,
                                           a_can_overlap    BOOLEAN)
    RETURNS BIGINT AS $$
DECLARE
    str_days_array TEXT [];
    days_array     INTEGER [];
    array_size     INTEGER;
    v_id           BIGINT;
BEGIN
    str_days_array := string_to_array(a_scheduled_days, ' ');
    array_size := array_length(str_days_array, 1);
    IF array_size IS NOT NULL
    THEN
        FOR i IN 1 .. array_size LOOP
            days_array [i] = CAST(str_days_array [i] AS INTEGER);
        END LOOP;
    ELSE
        days_array := NULL;
    END IF;

    INSERT INTO activity (name, type, schedule_type, scheduled_days, duration, start_date, start_time, finish_date, can_overlap)
    VALUES
        (a_name, a_type, a_schedule_type, days_array, a_duration, a_start_date, a_start_time, a_finish_date,
         a_can_overlap)
    RETURNING id
        INTO v_id;

    RETURN v_id;

END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION update_activity(an_id            BIGINT, a_name VARCHAR, a_type VARCHAR,
                                           a_schedule_type  VARCHAR,
                                           a_scheduled_days TEXT,
                                           a_duration       TIME, a_start_date DATE, a_start_time TIME,
                                           a_finish_date    DATE,
                                           a_can_overlap    BOOLEAN)
    RETURNS VOID AS $$
DECLARE
    str_days_array TEXT [];
    days_array     INTEGER [];
    array_size     INTEGER;
BEGIN
    str_days_array := string_to_array(a_scheduled_days, ' ');
    array_size := array_length(str_days_array, 1);

    IF array_size IS NOT NULL
    THEN
        FOR i IN 1 .. array_size LOOP
            days_array [i] = CAST(str_days_array [i] AS INTEGER);
        END LOOP;
    ELSE
        days_array := NULL;
    END IF;

    UPDATE activity
    SET name        = a_name, type = a_type, schedule_type = a_schedule_type, scheduled_days = days_array,
        duration    = a_duration, start_date = a_start_date, start_time = a_start_time, finish_date = a_finish_date,
        can_overlap = a_can_overlap
    WHERE id = an_id;

END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION set_activity_rooms(activity_id BIGINT, room_ids TEXT)
    RETURNS VOID AS $$
DECLARE
    room_id_array TEXT [];
    array_size    INTEGER;
BEGIN
    room_id_array := string_to_array(room_ids, ' ');
    array_size := array_length(room_id_array, 1);

    DELETE FROM activity_rooms
    WHERE id_activity = activity_id;

    FOR i IN 1 .. array_size LOOP
        INSERT INTO activity_rooms (id_activity, id_room) VALUES (activity_id, CAST(room_id_array [i] AS BIGINT));
    END LOOP;
END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION set_activity_lists(activity_id BIGINT, list_ids TEXT)
    RETURNS VOID AS $$
DECLARE
    list_id_array TEXT [];
    array_size    INTEGER;
BEGIN
    list_id_array := string_to_array(list_ids, ' ');
    array_size := array_length(list_id_array, 1);

    DELETE FROM activity_lists
    WHERE id_activity = activity_id;

    FOR i IN 1 .. array_size LOOP
        INSERT INTO activity_lists (id_activity, id_list) VALUES (activity_id, CAST(list_id_array [i] AS BIGINT));
    END LOOP;
END $$ LANGUAGE plpgsql VOLATILE;

-- INSERTING INITIAL DATA --
INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
VALUES ('root', 'root', 'System', 'Administrator', 'root@paso.system', 'SUPER_USER');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
    'Студенти прве године',
    'true',
    'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
    'Студенти друге године',
    'true',
    'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
    'Студенти треће године',
    'true',
    'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
    'Студенти четврте године',
    'true',
    'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
    'Студенти пете године',
    'true',
    'true');

