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

CREATE TABLE TEACHER (
  ID              BIGINT PRIMARY KEY,
  EMPLOYEE_NUMBER VARCHAR(16) UNIQUE NOT NULL,
  OFFICE          VARCHAR(32)        NOT NULL,
  FOREIGN KEY (ID) REFERENCES PERSON (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE BARRED_STUDENTS (
  ID_ROOM    BIGINT NOT NULL,
  ID_STUDENT BIGINT NOT NULL,
  PRIMARY KEY (ID_ROOM, ID_STUDENT),
  FOREIGN KEY (ID_ROOM) REFERENCES ROOM (ID) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (ID_STUDENT) REFERENCES STUDENT (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE ROOM_ENTRY (
  ID_ROOM    BIGINT    NOT NULL,
  ID_PERSON  BIGINT    NOT NULL,
  ENTRY_TIME TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (ID_ROOM, ID_PERSON, ENTRY_TIME),
  FOREIGN KEY (ID_ROOM) REFERENCES ROOM (ID) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (ID_PERSON) REFERENCES PERSON (ID) ON DELETE CASCADE ON UPDATE CASCADE
);

CREATE TABLE LIST (
  ID            BIGSERIAL PRIMARY KEY,
  NAME          VARCHAR(64) UNIQUE NOT NULL,
  SYSTEM        BOOLEAN            NOT NULL DEFAULT FALSE,
  PERMANENT     BOOLEAN            NOT NULL DEFAULT FALSE,
  DEMONSTRATORS BOOLEAN            NOT NULL DEFAULT FALSE,
  EXPIRY_DATE   DATE,
  ID_COURSE     BIGINT UNIQUE,
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

CREATE TABLE ACTIVITY_SLOTS (
  ID_ACTIVITY BIGINT    NOT NULL,
  START       TIMESTAMP NOT NULL,
  FINISH      TIMESTAMP NOT NULL,
  PRIMARY KEY (ID_ACTIVITY, START, FINISH),
  FOREIGN KEY (ID_ACTIVITY) REFERENCES ACTIVITY (ID) ON DELETE CASCADE ON UPDATE CASCADE
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

CREATE VIEW ROOM_OCCUPANCY AS
  SELECT
    R.ID          AS ROOM_ID,
    R.NAME        AS ROOM_NAME,
    R.ROOM_NUMBER AS ROOM_NUMBER,
    A.ID          AS ACTIVITY_ID,
    A.NAME        AS ACTIVITY_NAME,
    S.START,
    S.FINISH
  FROM ACTIVITY_SLOTS S
    JOIN ACTIVITY A ON S.ID_ACTIVITY = A.ID
    JOIN ACTIVITY_ROOMS AR ON A.ID = AR.ID_ACTIVITY
    JOIN ROOM R ON AR.ID_ROOM = R.ID;

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

CREATE VIEW ROOMS_VIEW AS
  SELECT
    R.*,
    (SELECT array_agg(ID_STUDENT :: TEXT)
     FROM BARRED_STUDENTS
     WHERE ID_ROOM = R.ID) AS BARRED_STUDENTS
  FROM ROOM R;

CREATE VIEW BARRED_STUDENTS_VIEW AS
  SELECT
    P.ID,
    P.LAST_NAME,
    P.FIRST_NAME,
    P.EMAIL,
    P.RFID,
    S.INDEX_NUMBER,
    S.YEAR_OF_STUDY,
    R.ID AS ID_ROOM
  FROM PERSON P
    JOIN STUDENT S USING (ID)
    LEFT OUTER JOIN BARRED_STUDENTS B ON B.ID_STUDENT = S.ID
    LEFT OUTER JOIN ROOM R ON B.ID_ROOM = R.ID;

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
    L.ID AS LIST_ID
  FROM PERSON P
    JOIN STUDENT S USING (ID)
    JOIN ENLISTED E ON S.ID = E.ID_STUDENT
    JOIN COURSE C ON E.ID_COURSE = C.ID
    JOIN LIST L ON C.ID = L.ID_COURSE
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
  WHERE S.YEAR_OF_STUDY = 5
  UNION
  SELECT
    P.ID,
    P.LAST_NAME,
    P.FIRST_NAME,
    P.EMAIL,
    P.RFID,
    S.INDEX_NUMBER,
    S.YEAR_OF_STUDY,
    6
  FROM PERSON P
    JOIN STUDENT S USING (ID)
  WHERE S.YEAR_OF_STUDY = 6
  UNION
  SELECT
    P.ID,
    P.LAST_NAME,
    P.FIRST_NAME,
    P.EMAIL,
    P.RFID,
    S.INDEX_NUMBER,
    S.YEAR_OF_STUDY,
    7
  FROM PERSON P
    JOIN STUDENT S USING (ID)
  WHERE S.YEAR_OF_STUDY = 7;

CREATE VIEW ROOM_ENTRY_DATA AS
  SELECT
    r.room_number,
    CASE WHEN s.index_number IS NULL
      THEN t.employee_number
    ELSE s.index_number END AS person_number,
    p.first_name,
    p.last_name,
    re.entry_time
  FROM person p
    LEFT OUTER JOIN student s ON p.id = s.id
    LEFT OUTER JOIN teacher t ON p.id = t.id
    JOIN room_entry re ON p.id = re.id_person
    JOIN room r ON re.id_room = r.id;

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

CREATE OR REPLACE FUNCTION insert_room(a_room_uuid UUID, a_name VARCHAR, a_number VARCHAR, a_barred_students VARCHAR)
  RETURNS BIGINT AS $$
DECLARE
  v_id BIGINT;
BEGIN
  INSERT INTO room (room_uuid, name, room_number) VALUES (a_room_uuid, a_name, a_number)
  RETURNING id
    INTO v_id;

  PERFORM set_barred_students(v_id, a_barred_students);

  RETURN v_id;
END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION update_room(an_id             BIGINT, a_room_uuid UUID, a_name VARCHAR, a_number VARCHAR,
                                       a_barred_students VARCHAR)
  RETURNS VOID AS $$
BEGIN
  UPDATE room
  SET room_uuid = a_room_uuid, name = a_name, room_number = a_number
  WHERE id = an_id;

  PERFORM set_barred_students(an_id, a_barred_students);

END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION generate_start_times(a_schedule_type VARCHAR, a_scheduled_days TEXT, a_start_date DATE,
                                                a_start_time    TIME, a_finish_date DATE)
  RETURNS TIMESTAMP [] AS $$
DECLARE
  str_days_array TEXT [];
  days_array     INTEGER [];
  array_size     INTEGER;
  tmp_counter    INTEGER;
  tmp_date       TIMESTAMP;
  tmp_day        INTEGER [];
  ret_val        TIMESTAMP [];
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

  IF a_schedule_type = 'ONCE'
  THEN
    ret_val [1] = a_start_date + a_start_time;
  END IF;
  IF a_schedule_type = 'WEEK_DAYS'
  THEN
    tmp_date := a_start_date;
    tmp_counter := 1;
    WHILE tmp_date < a_finish_date LOOP
      tmp_day [0] := extract(ISODOW FROM tmp_date);
      IF tmp_day <@ days_array
      THEN
        ret_val [tmp_counter] := tmp_date + a_start_time;
        tmp_counter := tmp_counter + 1;
      END IF;
      tmp_date := tmp_date + (INTERVAL '1 day');
    END LOOP;
  END IF;
  IF a_schedule_type = 'MONTH_DAYS'
  THEN
    tmp_date := a_start_date;
    tmp_counter := 1;
    WHILE tmp_date < a_finish_date LOOP
      tmp_day [0] := extract(DAY FROM tmp_date);
      IF tmp_day <@ days_array
      THEN
        ret_val [tmp_counter] := tmp_date + a_start_time;
        tmp_counter := tmp_counter + 1;
      END IF;
      tmp_date := tmp_date + (INTERVAL '1 day');
    END LOOP;
  END IF;

  RETURN ret_val;
END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION check_overlaps(an_activity_id   BIGINT, a_schedule_type VARCHAR,
                                          a_scheduled_days VARCHAR,
                                          a_duration       TIME, a_start_date DATE, a_start_time TIME,
                                          a_finish_date    DATE, an_activity_rooms VARCHAR)
  RETURNS TABLE(room_name VARCHAR, room_number VARCHAR, activity_name VARCHAR, start TIMESTAMP, finish TIMESTAMP) AS $$
DECLARE
  start_times            TIMESTAMP [];
  room_id_array          TEXT [];
  room_id_array_size     INTEGER;
  start_times_array_size INTEGER;
  var_r                  RECORD;
BEGIN
  room_id_array := string_to_array(an_activity_rooms, ' ');
  room_id_array_size := array_length(room_id_array, 1);
  start_times := generate_start_times(a_schedule_type, a_scheduled_days, a_start_date, a_start_time, a_finish_date);
  start_times_array_size := array_length(start_times, 1);

  IF room_id_array_size IS NOT NULL
  THEN
    FOR i IN 1 .. room_id_array_size LOOP
      FOR j IN 1 .. start_times_array_size LOOP
        FOR var_r IN (SELECT
                        RO.ROOM_NAME,
                        RO.ROOM_NUMBER,
                        RO.ACTIVITY_NAME,
                        RO.START,
                        RO.FINISH
                      FROM ROOM_OCCUPANCY RO
                        JOIN ACTIVITY A ON A.ID = RO.ACTIVITY_ID
                      WHERE A.TYPE <> 'INDIVIDUAL_WORK'
                            AND RO.ROOM_ID = CAST(room_id_array [i] AS BIGINT)
                            AND RO.ACTIVITY_ID <> an_activity_id
                            AND (start_times [j], start_times [j] + a_duration) OVERLAPS (RO.START, RO.FINISH))
        LOOP
          room_name := var_r.ROOM_NAME;
          room_number := var_r.ROOM_NUMBER;
          activity_name := var_r.ACTIVITY_NAME;
          start := var_r.START;
          finish:= var_r.FINISH;
          RETURN NEXT;
        END LOOP;
      END LOOP;
    END LOOP;
  END IF;
END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION insert_activity(a_name            VARCHAR, a_type VARCHAR, a_schedule_type VARCHAR,
                                           a_scheduled_days  VARCHAR,
                                           a_duration        TIME, a_start_date DATE, a_start_time TIME,
                                           a_finish_date     DATE,
                                           a_can_overlap     BOOLEAN, an_activity_rooms VARCHAR,
                                           an_activity_lists VARCHAR)

  RETURNS BIGINT AS $$
DECLARE
  str_days_array TEXT [];
  days_array     INTEGER [];
  array_size     INTEGER;
  v_id           BIGINT;
  tmp_date       TIMESTAMP;
  tmp_start      TIMESTAMP;
  tmp_finish     TIMESTAMP;
  tmp_day        INTEGER [];
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

  IF a_schedule_type = 'WEEK_DAYS'
  THEN
    tmp_date := a_start_date;
    WHILE tmp_date < a_finish_date LOOP
      tmp_start := tmp_date + a_start_time;
      tmp_finish := tmp_start + a_duration;
      tmp_day [0] := extract(ISODOW FROM tmp_date);
      IF tmp_day <@ days_array
      THEN
        INSERT INTO activity_slots (id_activity, start, finish) VALUES (v_id, tmp_start, tmp_finish);
      END IF;
      tmp_date := tmp_date + (INTERVAL '1 day');
    END LOOP;
  END IF;
  IF a_schedule_type = 'MONTH_DAYS'
  THEN
    tmp_date := a_start_date;
    WHILE tmp_date < a_finish_date LOOP
      tmp_start := tmp_date + a_start_time;
      tmp_finish := tmp_start + a_duration;
      tmp_day [0] := extract(DAY FROM tmp_date);
      IF tmp_day <@ days_array
      THEN
        INSERT INTO activity_slots (id_activity, start, finish) VALUES (v_id, tmp_start, tmp_finish);
      END IF;
      tmp_date := tmp_date + (INTERVAL '1 day');
    END LOOP;
  END IF;
  IF a_schedule_type = 'ONCE'
  THEN
    tmp_start := a_start_date + a_start_time;
    tmp_finish := tmp_start + a_duration;
    INSERT INTO activity_slots (id_activity, start, finish) VALUES (v_id, tmp_start, tmp_finish);
  END IF;

  PERFORM set_activity_rooms(v_id, an_activity_rooms);
  PERFORM set_activity_lists(v_id, an_activity_lists);

  RETURN v_id;
END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION update_activity(an_id             BIGINT, a_name VARCHAR, a_type VARCHAR,
                                           a_schedule_type   VARCHAR,
                                           a_scheduled_days  VARCHAR,
                                           a_duration        TIME, a_start_date DATE, a_start_time TIME,
                                           a_finish_date     DATE,
                                           a_can_overlap     BOOLEAN, an_activity_rooms VARCHAR,
                                           an_activity_lists VARCHAR)

  RETURNS VOID AS $$
DECLARE
  str_days_array TEXT [];
  days_array     INTEGER [];
  array_size     INTEGER;
  tmp_date       TIMESTAMP;
  tmp_start      TIMESTAMP;
  tmp_finish     TIMESTAMP;
  tmp_day        INTEGER [];
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
  SET name      = a_name, type = a_type, schedule_type = a_schedule_type, scheduled_days = days_array,
    duration    = a_duration, start_date = a_start_date, start_time = a_start_time, finish_date = a_finish_date,
    can_overlap = a_can_overlap
  WHERE id = an_id;

  DELETE FROM activity_slots
  WHERE id_activity = an_id;
  IF a_schedule_type = 'WEEK_DAYS'
  THEN
    tmp_date := a_start_date;
    WHILE tmp_date < a_finish_date LOOP
      tmp_start := tmp_date + a_start_time;
      tmp_finish := tmp_start + a_duration;
      tmp_day [0] = extract(ISODOW FROM tmp_date);
      IF tmp_day <@ days_array
      THEN
        INSERT INTO activity_slots (id_activity, start, finish) VALUES (an_id, tmp_start, tmp_finish);
      END IF;
      tmp_date := tmp_date + (INTERVAL '1 day');
    END LOOP;
  END IF;
  IF a_schedule_type = 'MONTH_DAYS'
  THEN
    tmp_date := a_start_date;
    WHILE tmp_date < a_finish_date LOOP
      tmp_start := tmp_date + a_start_time;
      tmp_finish := tmp_start + a_duration;
      tmp_day [0] = extract(DAY FROM tmp_date);
      IF tmp_day <@ days_array
      THEN
        INSERT INTO activity_slots (id_activity, start, finish) VALUES (an_id, tmp_start, tmp_finish);
      END IF;
      tmp_date := tmp_date + (INTERVAL '1 day');
    END LOOP;
  END IF;
  IF a_schedule_type = 'ONCE'
  THEN
    tmp_start := a_start_date + a_start_time;
    tmp_finish := tmp_start + a_duration;
    INSERT INTO activity_slots (id_activity, start, finish) VALUES (an_id, tmp_start, tmp_finish);
  END IF;

  PERFORM set_activity_rooms(an_id, an_activity_rooms);
  PERFORM set_activity_lists(an_id, an_activity_lists);

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

  IF array_size IS NOT NULL
  THEN
    FOR i IN 1 .. array_size LOOP
      INSERT INTO activity_rooms (id_activity, id_room) VALUES (activity_id, CAST(room_id_array [i] AS BIGINT));
    END LOOP;
  END IF;
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

  IF array_size IS NOT NULL
  THEN
    FOR i IN 1 .. array_size LOOP
      INSERT INTO activity_lists (id_activity, id_list) VALUES (activity_id, CAST(list_id_array [i] AS BIGINT));
    END LOOP;
  END IF;

END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION set_barred_students(room_id BIGINT, student_ids TEXT)
  RETURNS VOID AS $$
DECLARE
  student_id_array TEXT [];
  array_size       INTEGER;
BEGIN
  student_id_array := string_to_array(student_ids, ' ');
  array_size := array_length(student_id_array, 1);

  DELETE FROM barred_students
  WHERE ID_ROOM = room_id;

  IF array_size IS NOT NULL
  THEN
    FOR i IN 1 .. array_size LOOP
      INSERT INTO barred_students (id_room, id_student) VALUES (room_id, CAST(student_id_array [i] AS BIGINT));
    END LOOP;
  END IF;

END $$ LANGUAGE plpgsql VOLATILE;

CREATE OR REPLACE FUNCTION activity_priority(an_activity_id BIGINT)
  RETURNS INTEGER AS $$
DECLARE
  priority INTEGER;
  overlap  BOOLEAN;
BEGIN
  SELECT INTO priority, overlap
    (CASE type
     WHEN 'EXAM'
       THEN 1
     WHEN 'COLLOQUIUM'
       THEN 2
     WHEN 'LECTURE'
       THEN 3
     WHEN 'LAB_EXCERCISE'
       THEN 4
     WHEN 'INDIVIDUAL_WORK'
       THEN 5
     WHEN 'SPECIAL_EVENT'
       THEN 6 END),
    can_overlap
  FROM ACTIVITY
  WHERE ID = an_activity_id;

  IF overlap
  THEN
    priority := 0;
  END IF;

  RETURN priority;
END $$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION check_access(a_room_uuid UUID, a_rfid VARCHAR, a_teachers_only BOOLEAN)
  RETURNS BOOLEAN AS $$
DECLARE
  person_id         BIGINT;
  room_id           BIGINT;
  student_priority  INTEGER;
  required_priority INTEGER;
  max_priority      INTEGER;
  barred            INTEGER;
BEGIN
  person_id := NULL;

  SELECT INTO room_id r.id
  FROM room r
  WHERE r.room_uuid = a_room_uuid;

  IF room_id IS NULL
  THEN
    RETURN FALSE;
  END IF;

  SELECT INTO person_id p.id
  FROM person p
    JOIN teacher t USING (id)
  WHERE p.rfid = a_rfid;

  IF person_id IS NOT NULL
  THEN
    INSERT INTO room_entry (id_room, id_person) VALUES (room_id, person_id);
    RETURN TRUE;
  END IF;

  IF a_teachers_only
  THEN
    RETURN FALSE;
  END IF;

  SELECT INTO person_id p.id
  FROM person p
    JOIN student s USING (ID)
  WHERE p.rfid = a_rfid;

  -- Does the card exist in the system at all?
  IF person_id IS NULL
  THEN
    RETURN FALSE;
  END IF;

  -- Is student barred from entering the room?
  SELECT INTO barred COUNT(1)
  FROM barred_students
  WHERE id_room = room_id AND id_student = person_id;

  IF barred > 0
  THEN
    RETURN FALSE;
  END IF;

  SELECT INTO max_priority max(priority)
  FROM persons_priority
  WHERE room_uuid = a_room_uuid;

  -- Is there any activity at all?
  IF max_priority IS NULL
  THEN
    RETURN FALSE;
  END IF;

  -- If there is special event then everyone is allowed.
  IF max_priority = 6
  THEN
    INSERT INTO room_entry (id_room, id_person) VALUES (room_id, person_id);
    RETURN TRUE;
  END IF;

  SELECT INTO required_priority min(activity_priority(a.id))
  FROM room r
    JOIN activity_rooms ON r.id = activity_rooms.id_room
    JOIN activity a ON activity_rooms.id_activity = a.id
    JOIN activity_slots sl ON a.id = sl.id_activity
  WHERE room_uuid = a_room_uuid AND
        (CURRENT_TIMESTAMP, CURRENT_TIMESTAMP) OVERLAPS (sl.start, sl.finish);

  SELECT INTO student_priority min(priority)
  FROM persons_priority
  WHERE room_uuid = a_room_uuid AND rfid = a_rfid;

  -- Check if student has any activity.
  IF student_priority IS NULL
  THEN
    RETURN FALSE;
  END IF;

  IF student_priority > required_priority
  THEN
    RETURN FALSE;
  ELSE
    INSERT INTO room_entry (id_room, id_person) VALUES (room_id, person_id);
    RETURN TRUE;
  END IF;
END $$ LANGUAGE plpgsql VOLATILE;

CREATE VIEW PERSONS_PRIORITY AS
  SELECT
    r.room_uuid,
    a.id                             AS activity_id,
    CASE WHEN l.demonstrators
      THEN 0
    ELSE activity_priority(a.id) END AS priority,
    l.id                             AS list_id,
    lm.rfid
  FROM room r
    JOIN activity_rooms ON r.id = activity_rooms.id_room
    JOIN activity a ON activity_rooms.id_activity = a.id
    JOIN activity_slots sl ON a.id = sl.id_activity
    LEFT OUTER JOIN activity_lists al ON a.id = al.id_activity
    LEFT OUTER JOIN list l ON al.id_list = l.id
    LEFT OUTER JOIN list_members lm ON l.id = lm.list_id
  WHERE (lm.rfid IS NOT NULL OR activity_priority(a.id) = 6) AND
        (CURRENT_TIMESTAMP, CURRENT_TIMESTAMP) OVERLAPS (sl.start, sl.finish);

-- INSERTING INITIAL DATA --
INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
VALUES ('root', 'root', 'System', 'Administrator', 'root@paso.system', 'SUPER_USER');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
  'Студенти прве године основних студија',
  'true',
  'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
  'Студенти друге године основних студија',
  'true',
  'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
  'Студенти треће године основних студија',
  'true',
  'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
  'Студенти четврте године основних студија',
  'true',
  'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
  'Студенти мастер студија',
  'true',
  'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
  'Студенти прве године докторских студија',
  'true',
  'true');

INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
  'Студенти друге године докторских студија',
  'true',
  'true');

