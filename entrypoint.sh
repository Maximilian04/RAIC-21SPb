#!/usr/bin/env bash

PROJECT_NAME="spb_ai_champ"
SOLUTION_CODE_ENTRYPOINT="MyStrategy.cpp"
function compile() (
    set -e
    cmake -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_VERBOSE_MAKEFILE=ON .
    cmake --build . --config Release
)
COMPILED_FILE_PATH="$SOLUTION_CODE_PATH/$PROJECT_NAME"
function run() (
    set -e
    cp $MOUNT_POINT /tmp/$PROJECT_NAME
    chmod +x /tmp/$PROJECT_NAME
    /tmp/$PROJECT_NAME "$@"
)

. codegame.sh