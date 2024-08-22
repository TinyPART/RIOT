# TinyContainer
#
#  Copyright (C) 2023 Orange
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


MODULE := tinycontainer_runtime

# check for tinycontainer_container_wamr and tinycontainer_container_jerryscript

RUNTIME_PSEUDOMODULES :=

ifneq (,$(filter tinycontainer_container_wamr,$(USEMODULE)))
  RUNTIME_PSEUDOMODULES += tinycontainer_runtime_wamr
endif

ifneq (,$(filter tinycontainer_runtime_jerryscript,$(USEMODULE)))
  RUNTIME_PSEUDOMODULES += tinycontainer_runtime_jerryscript
endif

ifneq (,$(filter tinycontainer_runtime_rbpf,$(USEMODULE)))
  RUNTIME_PSEUDOMODULES += tinycontainer_runtime_rbpf
endif

RUNTIME_COUNT := $(words $(RUNTIME_PSEUDOMODULES))

# if count is not 1, then there is a problem
ifeq ($(RUNTIME_COUNT),1)
  RUNTIME := $(firstword $(RUNTIME_PSEUDOMODULES))
else
  $(error "tinycontainer_runtime: exactly one container runtime type must "
          "be selected")
endif


ifneq (,$(filter tinycontainer_runtime_wamr,$(RUNTIME)))
  DIRS += wamr
endif

ifneq (,$(filter tinycontainer_runtime_jerryscript,$(RUNTIME)))
  DIRS += jerryscript
endif

ifneq (,$(filter tinycontainer_runtime_rbpf,$(RUNTIME)))
  DIRS += rbpf
endif

include $(RIOTBASE)/Makefile.base
