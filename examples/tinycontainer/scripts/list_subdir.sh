#!/bin/bash

test $# -ne 1 && exit
test ! -d "$1" && exit

find "$1" -mindepth 1 -maxdepth 1 -type d -exec basename {} \; | sort 
