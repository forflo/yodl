#!/bin/bash

##
# Currently not working!
# Cling has some issues
##

function start_cling(){
    ../../cling/cling_ubuntu14/bin/./cling \
        -I. -I.. \
        -I../libmisc \
        --std=c++11 \
        -DHAVE_CONFIG_H -Wall -Wextra \
        -Wshadow  -L \
        $(make -B --dry-run \
            | grep "g\+\+" \
            | grep -o "[a-zA-Z0-9_-]*.cc" \
            | sed -e "s/main.cc//")

    return 0
}
