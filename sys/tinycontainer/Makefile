# TinyContainer
#
#  Copyright (C) 2023 Orange
#
# Please refer to the README.md and LICENSE files of tinycontainer


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
