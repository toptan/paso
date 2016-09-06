#!/bin/sh -xe
# This script starts docker and runs tests.

case "$1" in
        ubuntu)
            docker run --privileged -d -ti -e "container=docker" -v /sys/fs/cgroup:/sys/fs/cgroup -v `pwd`:/paso:rw ubuntu:xenial bash
            DOCKER_CONTAINER_ID=$(docker ps | grep ubuntu | awk '{print $1}')
            ;;

        centos)
            docker run --privileged -d -ti -e "container=docker" -v /sys/fs/cgroup:/sys/fs/cgroup -v `pwd`:/paso:rw centos:centos7 bash
            DOCKER_CONTAINER_ID=$(docker ps | grep centos | awk '{print $1}')
            ;;

        *)
            echo "Unsuported linux distribution $1. Only Ubuntu 16.04 and CentOS 7 are supported."
            exit 1

esac

docker logs $DOCKER_CONTAINER_ID
docker exec -ti $DOCKER_CONTAINER_ID /bin/bash -c "bash -xe /paso/tests/test_inside_docker.sh $1"
docker ps -a
# Fix coverage report paths
cat combined-coverage.info | sed "s|/paso/|`pwd`/|g" > coverage.info
docker stop $DOCKER_CONTAINER_ID
#docker rm -v $DOCKER_CONTAINER_ID
