#!/bin/bash

function uncrust(){
    find . -name "*.cc" -or -name "*.h" | 
        parallel --will-cite uncrustify \
            -c ../uncrustify/custom_config.cfg \
            -l CPP \< {} \> {/}.uncrust

    return 0
}

# danger!
function uncrust_replace(){
    echo Danger! Will delete all .cc and .h files!
    read i -p "Ok? "
    rm *.cc *.h
    rename ".uncrust" "" *.uncrust

    return 0
}
