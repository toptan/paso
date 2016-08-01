#!/bin/sh -xe

OS_VERSION=$1

ls -l /home

# Clean the yum cache
yum -y clean all
yum -y clean expire-cache

# First, install all the needed packages.
yum -y install epel-release

# Broken mirror?
echo "exclude=mirror.beyondhosting.net" >> /etc/yum/pluginconf.d/fastestmirror.conf

# An ugly hack as check-update exit code is 100 when there are packages to update
echo `yum -y check-update`

yum -y upgrade

yum -y install rpm-build gcc gcc-c++ boost-devel cmake3 git tar gzip make \
       python cppcheck doxygen lcov python-pip graphviz qt5-linguist \
       qt5-qtbase-devel qt5-qtbase-gui xorg-x11-server-Xvfb sysvinit-tools \
       glibc-static psmisc motif which xauth

pip install gcovr

# Fix dbus inside docker
dbus-uuidgen >/etc/machine-id

# Prepare dirs and configure build
mkdir -p /build
cd /build
cmake3 -DCMAKE_BUILD_TYPE=Debug /paso

# Detect number of CPUs online
CPU_COUNT="$(getconf _NPROCESSORS_ONLN | tr -d '\n')"
make -j $CPU_COUNT

# We need X to run GUI tests
xvfb-run -n 77 -s "-screen 0 1024x768x24 -ac" mwm &
# Give some time to X server to start
sleep 5
DISPLAY=:77 make coverage

killall mwm

