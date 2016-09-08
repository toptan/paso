# README
[![Build Status](https://travis-ci.org/toptan/paso.svg?branch=master)](https://travis-ci.org/toptan/paso)

### Table of Contents
- [**Welcome**](#welcome)
- [**Requirements**](#requirements)
    - [**Build requirements**](#build-requirements)
    - [**Installing requirements**](#installing-requirements)
    - [**Source code access**](#source-code-access)
- [**Building**](#building)
    - [**Configuring the build**](#configuring-the-build)
    - [**Building binaries**](#building-binaries)
    - [**Important make targets**](#important-make-targets)
    - [**Coverage report**](#coverage-report)
    - [**Using docker for running tests**](#using-docker-for-running-tests)
- [**Running**](#running)
    - [**Configuring database**](#configuring-database)
    - [**Configuring server**](#configuring-server)
    - [**Running the server**](#running-the-server)
    - [**Server logging**](#server-logging)
    - [**Running administration utility**](#running-administration-utility)
    
## Welcome
Welcome to README for the practical part of my diploma thesis. It is still work in progress and will update frequently.

## Requirements
This project requires Ubuntu 16.04 to build and run, however any other modern Linux distribution that has Qt5 framework of version
5.5 or newer should work.

### Build requirements
- Qt5 Framework version 5.5 and later
- Modern C++ compiler that supports at least C++11 standard. gcc >= 4.8 or clang >= 3.4
- CMake >= 3.4.3
- Doxygen for generating API documentation.
- Python, gcov, gcovr, lcov and genhtml for code coverage report.
- cppcheck for static code analysis.

### Installing requirements
To satisfy all build and run requirements the following Ubuntu packages must be installed:
```
gcc g++ libboost-dev cmake git tar gzip make python cppcheck doxygen lcov gcovr graphviz postgresql
qttools5-dev-tools qt5-default psmisc mwm xauth sudo xvfb libqt5sql5-sqlite libqt5sql5-psql
```

If you are using CentOS the follwing packages will satisfy requirements:
```
rpm-build gcc gcc-c++ boost-devel cmake3 git tar gzip make python cppcheck doxygen lcov python-pip
graphviz qt5-linguist qt5-qtbase-devel qt5-qtbase-gui xorg-x11-server-Xvfb sysvinit-tools
glibc-static psmisc motif which xauth postgresql-server qt5-qtbase-postgresql
```
On CentOS gcovr is not available as RPM package but can be installed via pip:
```
pip install gcovr==3.2
```

## Source code access
The complete source code is available via this github repository. The repository is configured to run automated build/unit 
tests and produce code coverage report for each commit. Build reports are provided by 
[Travis CI](https://travis-ci.org) service.

Latest build and coverage reports are available by clicking on two badges at the top of this README file.

## Building
The source code is organized in such manner that the software should be built outside the source tree. To make instructions more
readable we will refer source root as **`<src_dir>`** and build directory as **`<build_dir>`**.

### Configuring the build
This project uses CMake to configure the build. The cmake command should be executed in the `<build_dir>` and should look like this
```bash
cmake -DCMAKE_BUILD_TYPE=<TYPE> -DCMAKE_ENABLE_COVERAGE=ON|OFF <src_dir>
```
Build type can be either `Debug` or `Release`. The second option defines whether build should be configured to produce code coverage report.
Producing code coverage report is only possible for `Debug` build type.

### Building binaries
After successful configuration the software is built in usual way by executing make command. Parallel builds are supported so one can 
use `-j` make option to speed up the build.

### Important make targets
Apart from standard make targets several additional ones are provided:
```
 make check          - Runs tests.
 make coverage       - Runs tests and generates code coverage report.
 make clean-coverage - Cleans/resets code coverage data.
```
To be able to run tests and coverage report, a running PostgreSQL database is needed. Also, the database user with superuser role has to exist. The user name and password both must be ```pasotest```.

### Coverage report
The coverage report, if generated, is available by opening `<build_dir>/coverage/index.html`

### Using docker for running tests
If you don't want to run tests on your physicall machine, you can use docker. To run tests inside docker position yourself in the root of the source tree and execute:
```
./tests/setup_test.sh <os_flavor>
```
The **os_flavor** argument can be either ubuntu or centos, depending whether you want to run tests on ubuntu or centos docker container.

## Running
The build produces two executables, the PaSo server and administration utility. The server is required for authentication and
it must be started before administration utility.

### Configuring database
The server requires database to be configured and set up. The following instructions are aligned with servers own configuration
that will be explained later.

1. Switch to postgres user: 
```
toptan@darkstartwo ~ $ sudo su - 
[sudo] password for toptan: 
darkstartwo ~ # su - postgres
postgres@darkstartwo ~ $
```
2. Creating database user and new database:
```
postgres@darkstartwo ~ $  echo "create user paso with createdb password 'paso';" | psql postgres 
CREATE ROLE
postgres@darkstartwo ~ $  echo "create database paso owner paso;" | psql postgres 
CREATE DATABASE
postgres@darkstartwo ~ $ 
```
3. Creating tables, views and inserting required data. You will need to enter password.
```
postgres@darkstartwo ~ $ psql -hlocalhost -U paso -W paso < <src_dir>/sql/tables.sql 
Password for user raso: 
CREATE TABLE
CREATE TABLE
CREATE TABLE
CREATE TABLE
CREATE TABLE
CREATE TABLE
CREATE TABLE
CREATE TABLE
CREATE VIEW
CREATE VIEW
CREATE FUNCTION
CREATE TRIGGER
CREATE FUNCTION
CREATE TRIGGER
CREATE FUNCTION
CREATE TRIGGER
INSERT 0 1
INSERT 0 1
INSERT 0 1
INSERT 0 1
INSERT 0 1
INSERT 0 1
```

### Configuring server
The default server configuration is provided in `<src_dir>/conf/pasoserver.conf` and has two sections.
```
[server]
port=6789
timeout=5000
keyFile=/tmp/server.key
certFile/tmp/server.csr

[database]
database=paso
server=localhost
port=5432
username=paso
password=paso
```
The first section defines on which port the server will listen as well as timeout in milliseconds for all network operations.
It also contans locations of the server private key and certificate. To generate key and certificate use openssl command like this:
```
openssl req -x509 -newkey rsa:2048 -keyout server.key -nodes -days 365 -out server.csr
```

The second section gives required info for database connection.

### Running the server
The command line options for the server:
```
toptan@darkstartwo <build_dir>/server $ ./pasoserver -h
Usage: ./pasoserver [options]

Options:
  -h, --help                  Displays this help.
  -v, --version               Displays version information.
  -c, --config <config_file>  Full path to the configuration file.
                              Defaults to: /etc/pasoserver.conf

```
If you do not want to change anything or put the configuration file under your machine's `/etc` directory you can start the
server via following command:
```
<build_dir>/server/pasoserver -c <src_dir>/conf/pasoserver.conf
```

### Server logging
The server uses system syslog facilities for logging. If you do not want to pollute your syslog, the `rsyslog` and `logrotate`
configuration files are available in `<src_dir>/conf`.

### Running administration utility
The administration utility is GUI application and has no command line options. Simply run its binary which is called `pasoadmin`
and is located in `<build_dir>/admin`.

The default super user username and password are both `root`.
