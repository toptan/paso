#!/bin/sh -xe

# Update everything
apt --yes update
apt --yes dist-upgrade

# Install needed packages
apt --yes install gcc g++ libboost-dev cmake git tar gzip make python cppcheck \
          doxygen lcov gcovr graphviz qttools5-dev-tools qt5-default psmisc \
          mwm xauth sudo xvfb libqt5sql5-sqlite

# Generate and set locale
locale-gen en_US.UTF-8
export LANG=en_US.UTF-8
export LANGUAGE=en_US.UTF-8
export LC_ALL=en_US.UTF-8

# Prepare dirs and configure build
mkdir -p /tmp/build
cd /tmp/build
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ENABLE_COVERAGE=ON /paso

# Detect number of CPUs online
CPU_COUNT="$(getconf _NPROCESSORS_ONLN | tr -d '\n')"
make -j $CPU_COUNT

# We need X to run GUI tests
xvfb-run -n 77 -s "-screen 0 1024x768x24 -ac" mwm &

# Give some time to X server to start
sleep 5
DISPLAY=:77 make coverage

# mvm ignores TERM, that's why we use KILL
killall -KILL mwm

# Copy coverage report outside
cp /tmp/build/combined-coverage.info /paso

