# TinyContainer
#
#  Copyright (C) 2023 Orange
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


ifneq (,$(filter tinycontainer_service,$(USEMODULE)))
  DIRS += service
endif

ifneq (,$(filter tinycontainer_container,$(USEMODULE)))
  DIRS += container
endif

ifneq (,$(filter tinycontainer_controller,$(USEMODULE)))
  DIRS += controller
endif

ifneq (,$(filter tinycontainer_memmgr,$(USEMODULE)))
  DIRS += memmgr
endif

ifneq (,$(filter tinycontainer_memmgr_ram,$(USEMODULE)))
  DIRS += memmgr/ram
endif

ifneq (,$(filter tinycontainer_firewall,$(USEMODULE)))
  DIRS += firewall
endif

ifneq (,$(filter tinycontainer_container_wamr,$(USEMODULE)))
  DIRS += container/wamr
endif

ifneq (,$(filter tinycontainer_container_jerryscript,$(USEMODULE)))
  DIRS += container/jerryscript
endif

#INCLUDES += -I$(RIOTBASE)/sys/tinycontainer/include

include $(RIOTBASE)/Makefile.base
