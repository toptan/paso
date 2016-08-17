#!/bin/sh -xe
# This script starts docker and runs tests.

docker run --privileged -d -ti -e "container=docker" -v /sys/fs/cgroup:/sys/fs/cgroup -v `pwd`:/paso:rw ubuntu:xenial bash
DOCKER_CONTAINER_ID=$(docker ps | grep ubuntu | awk '{print $1}')
docker logs $DOCKER_CONTAINER_ID
docker exec -ti $DOCKER_CONTAINER_ID /bin/bash -c "bash -xe /paso/tests/test_inside_docker.sh"
docker ps -a
# Fix coverage report paths
cat combined-coverage.info | sed "s|/paso/|`pwd`/paso/|g" > coverage.info
docker stop $DOCKER_CONTAINER_ID
docker rm -v $DOCKER_CONTAINER_ID
