#!/bin/bash

TAB=$'\t'

function generate_skeleton() {
    local MODULE_NAME=$1
    local LICENSE=$2
    local AUTHOR=$3
    local DESCRIPTION=$4
    local VERSION=$5

    mkdir $MODULE_NAME

    cat << SOURCE > "${MODULE_NAME}/${MODULE_NAME}.c"
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("${LICENSE}");
MODULE_AUTHOR("${AUTHOR}");
MODULE_DESCRIPTION("${DESCRIPTION}");
MODULE_VERSION("${VERSION}");

static int __init ${MODULE_NAME}_init(void) {
    return 0;
}

static void __exit ${MODULE_NAME}_exit(void) {
    ;
}

module_init(${MODULE_NAME}_init);
module_exit(${MODULE_NAME}_exit);
SOURCE

    cat << MAKEFILE > "${MODULE_NAME}/Makefile"
obj-m += ${MODULE_NAME}.o

all:
${TAB}make -C /lib/modules/\$(shell uname -r)/build M=\$(PWD) modules
clean:
${TAB}make -C /lib/modules/\$(shell uname -r)/build M=\$(PWD) clean
MAKEFILE
}

read -p "Module name: " MODULE_NAME
read -p "License: " LICENSE
read -p "Author: " AUTHOR
read -p "Description: " DESCRIPTION
read -p "Version: " VERSION
generate_skeleton "$MODULE_NAME" "$LICENSE" "$AUTHOR" "$DESCRIPTION" "$VERSION"
