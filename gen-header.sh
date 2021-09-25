#!/bin/bash

prefix=${1}_
fileo=$2

def() {
    name=${1}
    value=${!1}
    if [ ${value:0:1} == "$" ]; then
        eval "value="$value
    fi
    echo "#define $name \"$value\""
}

import() {
    vars='echo ${!'$prefix'*}'
    for var in $(eval $vars); do
        def $var
    done
}

file=$(basename ${2@U})
guard=_${file/./_}_

cat <<EOF > $fileo
/*
 * Automatically generated file. Do NOT edit.
 */

#ifndef $guard
#define $guard

$(import)

#endif /* $guard */
EOF
