#!/usr/bin/env bash

docker build --network=host -f ./build_env/Dockerfile -t app-build-env --pull \
--build-arg MY_USER=$(id -un) \
--build-arg MY_UID=$(id -u) \
--build-arg MY_GID=$(id -g) \
.