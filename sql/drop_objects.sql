DROP VIEW IF EXISTS ENLISTED_STUDENTS;
DROP VIEW IF EXISTS LIST_MEMBERS;
DROP VIEW IF EXISTS ROOMS_VIEW;
DROP VIEW IF EXISTS BARRED_STUDENTS_VIEW;
DROP TABLE IF EXISTS ACTIVITY_ROOMS CASCADE;
DROP TABLE IF EXISTS ACTIVITY_LISTS CASCADE;
DROP TABLE IF EXISTS ACTIVITY_SLOTS CASCADE;
DROP TABLE IF EXISTS BARRED_STUDENTS CASCADE;
DROP TABLE IF EXISTS ACTIVITY CASCADE;
DROP TABLE IF EXISTS MEMBER CASCADE;
DROP TABLE IF EXISTS ENLISTED CASCADE;
DROP TABLE IF EXISTS LIST CASCADE;
DROP TABLE IF EXISTS STUDENT CASCADE;
DROP TABLE IF EXISTS PERSON CASCADE;
DROP TABLE IF EXISTS ROOM CASCADE;
DROP TABLE IF EXISTS COURSE CASCADE;
DROP TABLE IF EXISTS SYSTEM_USER CASCADE;
DROP FUNCTION IF EXISTS a_i_course_function() CASCADE;
DROP FUNCTION IF EXISTS a_u_course_function() CASCADE;
DROP FUNCTION IF EXISTS b_iu_list_function() CASCADE;
DROP SEQUENCE IF EXISTS system_user_id_seq;
DROP SEQUENCE IF EXISTS course_id_seq;
DROP SEQUENCE IF EXISTS room_id_seq;
DROP SEQUENCE IF EXISTS person_id_seq;
DROP SEQUENCE IF EXISTS list_id_seq;
DROP SEQUENCE IF EXISTS activity_id_seq;
