create table system_user (
        id         integer primary key autoincrement,
        username   text unique not null,
        password   text not null,
        first_name text not null,
        last_name  text not null,
        email      text not null,
        role       text not null);
--
create table course (
        id   integer primary key autoincrement,
        code text unique not null,
        name text not null);
--
create table room (
        id          integer primary key autoincrement,
        room_uuid   text unique not null,
        name        text not null,
        room_number text unique not null);
--
create table person (
        id          integer primary key autoincrement,
        first_name  text not null,
        last_name   text not null,
        email       text unique,
        rfid        text);
--
create table student (
        id              integer primary key,
        index_number    text unique not null,
        year_of_study   integer not null,
        foreign key(id) references person(id) on delete cascade on update cascade);
--
create table list (
        id          integer   primary key autoincrement,
        name        text      unique not null,
        system      boolean   not null default false,
        permanent   boolean   not null default false,
        expiry_date date,
        id_course   integer   unique,
        foreign key(id_course) references course(id) on delete cascade on update cascade);
--
create table enlisted (
        id_student      integer not null,
        id_course       integer not null,
        primary key(id_student, id_course),
        foreign key(id_student) references student(id) on delete cascade on update cascade,
        foreign key(id_course) references course(id) on delete cascade on update cascade);
--
create table member (
        id_student      integer not null,
        id_list         integer not null,
        primary key(id_student, id_list),
        foreign key(id_student) references student(id) on delete cascade on update cascade,
        foreign key(id_list) references list(id) on delete cascade on update cascade);
--
create table activity (
        id          integer primary key autoincrement,
        name        text    not null,
        type        text    not null,
        schedule    text    not null,
        duration    time    not null,
        start_date  date    not null,
        finish_date date    not null,
        can_overlap boolean not null default false);
--
create view enlisted_students as
    select distinct p.id, p.last_name, p.first_name, p.email, p.rfid, s.index_number, s.year_of_study, c.code
      from person p
      join student s using(id)
      left outer join enlisted e on e.id_student = s.id
      left outer join course c on e.id_course = c.id;
--
create view list_members as
    select p.id, p.last_name, p.first_name, p.email, p.rfid, s.index_number, s.year_of_study, l.id as list_id
      from person p
      join student s using(id)
      join member m on m.id_student = s.id
      join list l on l.id = m.id_list
    union
    select p.id, p.last_name, p.first_name, p.email, p.rfid, s.index_number, s.year_of_study, 1
      from person p
      join student s using(id)
     where s.year_of_study = 1
    union
    select p.id, p.last_name, p.first_name, p.email, p.rfid, s.index_number, s.year_of_study, 2
      from person p
      join student s using(id)
     where s.year_of_study = 2
    union
    select p.id, p.last_name, p.first_name, p.email, p.rfid, s.index_number, s.year_of_study, 3
      from person p
      join student s using(id)
     where s.year_of_study = 3
    union
    select p.id, p.last_name, p.first_name, p.email, p.rfid, s.index_number, s.year_of_study, 4
      from person p
      join student s using(id)
     where s.year_of_study = 4
    union
    select p.id, p.last_name, p.first_name, p.email, p.rfid, s.index_number, s.year_of_study, 5
      from person p
      join student s using(id)
     where s.year_of_study = 5;
--
create trigger a_i_course after insert on course
begin
    insert into list(name, system, permanent, id_course)
              values(new.code || ' студенти', 1, 1, new.id);
end;
--
create trigger a_u_course
         after update
            on course
          when old.code <> new.code
begin
    update list
       set name = new.code || ' студенти'
     where id_course = new.id;
end;
--
create trigger a_i_list
         after insert
            on list
          when new.system = 1 or new.permanent = 1
begin
    update list set expiry_date = null where id = new.id;
end;
--
create trigger a_u_list
         after update
            on list
          when new.system = 1 or new.permanent = 1
begin
    update list set expiry_date = null where id = new.id;
end;
--
INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
       VALUES (
        'admin',
        'admin_password',
        'Admin',
        'Admin',
        'admin@paso.system',
        'ADMINISTRATOR');
--
INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
       VALUES (
        'room_manager',
        'room_manager_password',
        'Room',
        'Manager',
        'room.manager@paso.system',
        'ROOM_MANAGER');
--
INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
       VALUES (
        'manager',
        'manager_password',
        'Manager',
        'Managerovic',
        'manager@paso.system',
        'MANAGER');
--
INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, LAST_NAME, EMAIL, ROLE)
       VALUES (
        'scheduler',
        'scheduler_password',
        'Scheduler',
        'Schedulerovic',
        'scheduler@paso.system',
        'SCHEDULER');
--
INSERT INTO ROOM (ROOM_UUID, NAME, ROOM_NUMBER) VALUES (
        '{d23a502b-a567-4929-ba99-9f93f36bf4e3}',
        'Laboratorija 42',
        '42');
--
INSERT INTO ROOM (ROOM_UUID, NAME, ROOM_NUMBER) VALUES (
        '{7003528d-4c44-4f91-91b4-b82cb5afb009}',
        'Paviljon 26',
        'P26');
--
INSERT INTO COURSE (CODE, NAME) VALUES (
        'IR3SP',
        'Sistemsko programiranje');
--
INSERT INTO LIST(NAME, SYSTEM, PERMANENT) VALUES (
        'Студенти прве године',
        'true',
        'true');
--
INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
        'Студенти друге године',
        'true',
        'true');
--
INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
        'Студенти треће године',
        'true',
        'true');
--
INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
        'Студенти четврте године',
        'true',
        'true');
--
INSERT INTO LIST (NAME, SYSTEM, PERMANENT) VALUES (
        'Студенти пете године',
        'true',
        'true');
--
