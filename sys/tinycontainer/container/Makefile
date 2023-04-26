# TinyContainer
#
#  Copyright (C) 2023 Orange
#
# Please refer to the README.md and LICENSE files of tinycontainer

MODULE := tinycontainer_container

# check for tinycontainer_container_wamr and tinycontainer_container_jerryscript

CONTAINER_PSEUDOMODULES :=

ifneq (,$(filter tinycontainer_container_wamr,$(USEMODULE)))
  CONTAINER_PSEUDOMODULES += tinycontainer_container_wamr
endif

ifneq (,$(filter tinycontainer_container_jerryscript,$(USEMODULE)))
  CONTAINER_PSEUDOMODULES += tinycontainer_container_jerryscript
endif

ifneq (,$(filter tinycontainer_container_rbpf,$(USEMODULE)))
  CONTAINER_PSEUDOMODULES += tinycontainer_container_rbpf
endif

CONTAINER_COUNT := $(words $(CONTAINER_PSEUDOMODULES))

# if count is not 1, then there is a problem
ifeq ($(CONTAINER_COUNT),1)
  CONTAINER := $(firstword $(CONTAINER_PSEUDOMODULES))
else
  $(error "tinycontainer_container yyy: exactly one container type must be selected")
endif


ifneq (,$(filter tinycontainer_container_wamr,$(CONTAINER)))
  DIRS += wamr
endif

ifneq (,$(filter tinycontainer_service_jerryscript,$(CONTAINER)))
  DIRS += jerryscript
endif

ifneq (,$(filter tinycontainer_container_rbpf,$(CONTAINER)))
  DIRS += rbpf
endif

include $(RIOTBASE)/Makefile.base
