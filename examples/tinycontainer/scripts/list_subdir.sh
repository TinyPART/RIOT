# Copyright (C) 2024 Orange
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# @author      Samuel legouix <samuel.legouix@orange.com>

#!/bin/bash

test $# -ne 1 && exit
test ! -d "$1" && exit

find "$1" -mindepth 1 -maxdepth 1 -type d -exec basename {} \; | sort
