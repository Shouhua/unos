#!/usr/bin/bash

export _OS_NAME=$1

cat tools/GRUB_TEMPLATE | envsubst > "$2"