#!/bin/sh -xe

# This script starts docker and systemd (if el7)

if [ "$1" = "centos" ]; then
    # Version of CentOS/RHEL
    el_version=$2

    docker run --privileged -d -ti -e "container=docker"  -v /sys/fs/cgroup:/sys/fs/cgroup -v `pwd`:/paso:rw  centos:centos${el_version}   bash
    DOCKER_CONTAINER_ID=$(docker ps | grep centos | awk '{print $1}')
    docker logs $DOCKER_CONTAINER_ID
    docker exec -ti $DOCKER_CONTAINER_ID /bin/bash -c "bash -xe /paso/tests/test_inside_docker_centos.sh ${el_version}"
    docker ps -a
    docker stop $DOCKER_CONTAINER_ID
    docker rm -v $DOCKER_CONTAINER_ID
fi
