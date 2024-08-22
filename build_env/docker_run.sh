#!/usr/bin/env bash

set -x
docker run -d --cap-add sys_ptrace \
-p 127.0.0.1:2222:22 \
--name app-build-env \
--rm -it \
--mount type=bind,src=$PWD,target=/workspace/ \
--workdir /workspace/ \
app-build-env