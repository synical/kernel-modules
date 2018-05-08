#!/bin/bash

function generate_skeleton() {
    local MODULE_NAME=$1
    local LICENSE=$2
    local AUTHOR=$3
    local DESCRIPTION=$4
    local VERSION=$5

    mkdir $MODULE_NAME

    cat << SKELETON > "${MODULE_NAME}/${MODULE_NAME}.c"
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("${LICENSE}");
MODULE_AUTHOR("${AUTHOR}");
MODULE_DESCRIPTION("${DESCRIPTION}");
MODULE_VERSION("${VERSION}");

static int __init hw_mod_init(void) {
    return 0;
}

static void __exit hw_mod_exit(void) {
    return 0
}

module_init(hw_mod_init);
module_exit(hw_mod_exit);
SKELETON

}

read -p "Module name: " MODULE_NAME
read -p "License: " LICENSE
read -p "Author: " AUTHOR
read -p "Description: " DESCRIPTION
read -p "Version: " VERSION
generate_skeleton "$MODULE_NAME" "$LICENSE" "$AUTHOR" "$DESCRIPTION" "$VERSION"
