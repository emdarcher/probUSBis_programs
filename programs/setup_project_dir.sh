#!/bin/bash

BUILD_THINGS_DIR="../build_things"

PROJ_DIR=$1

if [ "$PROJ_DIR" != "" ]; then
        if [ -d $PROJ_DIR ]; then
            echo "project dir $PROJ_DIR already exists! aborting script."
            exit 1
        else
            echo "project dir name = $PROJ_DIR"
        fi
else
    echo "Please specify a project dir name"
    exit 1
fi

echo  "making project dir $PROJ_DIR"
mkdir $PROJ_DIR

echo  "coping contents of $BUILD_THINGS_DIR to $PROJ_DIR"
cp -r $BUILD_THINGS_DIR/* $PROJ_DIR


exit 0
