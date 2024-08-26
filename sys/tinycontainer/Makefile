# TinyContainer
#
#  Copyright (C) 2023-2024 Orange
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


ifneq (,$(filter tinycontainer_service,$(USEMODULE)))
  DIRS += service
endif

ifneq (,$(filter tinycontainer_runtime,$(USEMODULE)))
  DIRS += runtime
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

ifneq (,$(filter tinycontainer_security,$(USEMODULE)))
  DIRS += security
endif

ifneq (,$(filter tinycontainer_security_sthread,$(USEMODULE)))
  DIRS += security/sthread
endif

ifneq (,$(filter tinycontainer_metadata,$(USEMODULE)))
  DIRS += metadata
endif

ifneq (,$(filter tinycontainer_runtime_wamr,$(USEMODULE)))
  DIRS += runtime/wamr
endif

ifneq (,$(filter tinycontainer_runtime_jerryscript,$(USEMODULE)))
  DIRS += runtime/jerryscript
endif

#INCLUDES += -I$(RIOTBASE)/sys/tinycontainer/include

include $(RIOTBASE)/Makefile.base
