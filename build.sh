#!/bin/sh

docker run --privileged -ti -v $(pwd):/workdir docker.io/zephyrprojectrtos/zephyr-build:latest
