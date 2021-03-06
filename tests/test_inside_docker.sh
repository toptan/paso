#!/bin/sh -xe

case "$1" in
        ubuntu)
            # Update everything
            apt --yes update
            apt --yes dist-upgrade

            # Generate locale
            locale-gen en_US.UTF-8

            # Set locale
            export LANG=en_US.UTF-8
            export LANGUAGE=en_US.UTF-8
            export LC_ALL=en_US.UTF-8

            # Install needed packages
            apt --yes install gcc g++ libboost-dev cmake git tar gzip make python cppcheck \
                      doxygen lcov gcovr graphviz qttools5-dev-tools qt5-default psmisc \
                      mwm xauth sudo xvfb libqt5sql5-sqlite libqt5sql5-psql postgresql

            # Setup database for tests
            service postgresql start
            su - postgres -c "echo \"CREATE ROLE pasotest SUPERUSER LOGIN UNENCRYPTED PASSWORD 'pasotest';\" | psql"

            # Set cmake command
            CMAKE_COMMAND=cmake
            ;;

        centos)
            # Clean all caches
            yum -y clean all
            yum -y clean expire-cache

            # Enable EPEL
            yum -y install epel-release

            # An ugly hack as check-update exit code is 100 when there are packages to update
            echo `yum -y check-update`

            # Upgrade
            yum -y upgrade

            # Install needed packages
            yum -y install rpm-build gcc gcc-c++ boost-devel cmake3 git tar gzip make \
                   python cppcheck doxygen lcov python-pip graphviz qt5-linguist openssl \
                   qt5-qtbase-devel qt5-qtbase-gui xorg-x11-server-Xvfb sysvinit-tools \
                   glibc-static psmisc motif which xauth postgresql-server qt5-qtbase-postgresql

            # Install gcovr
            pip install gcovr==3.2

            # Fix dbus inside docker
            dbus-uuidgen > /etc/machine-id

            # Set locale
            export LANG=en_US.UTF-8
            export LANGUAGE=en_US.UTF-8
            export LC_ALL=en_US.UTF-8

            # Setup database for tests
            su - postgres -c "initdb --pgdata=/var/lib/pgsql/data --auth-host=md5 --encoding=UTF8"
            su - postgres -c "pg_ctl -D /var/lib/pgsql/data -l logfile start"
            sleep 5
            su - postgres -c "echo \"CREATE ROLE pasotest SUPERUSER LOGIN UNENCRYPTED PASSWORD 'pasotest';\" | psql"

            # Set cmake command
            CMAKE_COMMAND=cmake3
            ;;

        *)
            echo $"Unsupported linux distribution $1. Only Ubuntu 16.04 and CentOS 7 are supported."
            exit 1

esac

# Prepare dirs and configure build
mkdir -p /tmp/build
cd /tmp/build
$CMAKE_COMMAND -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ENABLE_COVERAGE=ON /paso

# Build everything
make

# We need X to run GUI tests
xvfb-run -n 77 -s "-screen 0 1024x768x24 -ac" mwm &

# Give some time to X server to start
sleep 5
DISPLAY=:77 make coverage

# mvm ignores TERM, that's why we use KILL
killall -KILL mwm

# Copy coverage report outside
cp /tmp/build/combined-coverage.info /paso

