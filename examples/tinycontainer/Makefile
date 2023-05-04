# name of the application
APPLICATION = tinycontainer

# If no BOARD is found in the environment, use this default:
BOARD ?= dwm1001

# Currently, TinyContainer has only been tested on native, dwm1001 and arduino-nano-33-ble
BOARD_WHITELIST := dwm1001 arduino-nano-33-ble nrf52840dk native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# required modules
USEMODULE += tinycontainer

# TinyContainer supports following container backend
TINYCONTAINER_CONTAINER ?= wamr
#TINYCONTAINER_CONTAINER ?= rbpf
#TINYCONTAINER_CONTAINER ?= jerryscript

# TinyContainer supports following memory manager
TINYCONTAINER_MEMMGR ?= ram


ifneq (,$(filter rbpf,$(TINYCONTAINER_CONTAINER)))
  BLOBS += rbpf/example_app.bin
endif

# shell
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += xtimer

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 0

include $(RIOTBASE)/Makefile.include
