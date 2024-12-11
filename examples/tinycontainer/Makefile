# TinyContainer
#
#  Copyright (C) 2023-2024 Orange
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# name of the application
APPLICATION = tinycontainer

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
DEVELHELP ?= 1

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# load tinycontainer configuration
TINYCONTAINER_CONFIG = $(CURDIR)/config/tinycontainer.config.inc
ifeq ("$(wildcard $(TINYCONTAINER_CONFIG))","")
    $(info Check your Configuration!)
    $(info You can use the 'tinycontainerctl' command)
    $(error Invalid configuration)
else
    include $(CURDIR)/config/tinycontainer.config.inc
endif

# The configuration file should provides the following variables
#
# TinyContainer supports following container backend
#TINYCONTAINER_RUNTIME ?= wamr
#TINYCONTAINER_RUNTIME ?= rbpf
#TINYCONTAINER_RUNTIME ?= jerryscript
#
# TinyContainer supports following memory manager
#TINYCONTAINER_MEMMGR ?= ram
#
# TinyContainer supports following crypto backend
#TINYCONTAINER_CRYPTO ?= none
#TINYCONTAINER_CRYPTO ?= psa
## Configuration for PSA crypto backend:
#CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=1
#
# These parameters may be used to fine tune TinyContainer configuration
#NUMBER_OF_CONTAINERS ?= 3
#MAX_SIZE_FOR_METADATA ?= 1024
#MAX_SIZE_FOR_CONTAINER ?= 1024
#
# Uncomment to activate the networking
#TINYCONTAINER_NETWORK ?= 1

# required modules
USEMODULE += tinycontainer

# Currently, TinyContainer has only been tested on native, dwm1001 and arduino-nano-33-ble
BOARD_WHITELIST := dwm1001 arduino-nano-33-ble arduino-nano-33-ble-sense nrf52840dk nrf9160dk native

ifneq (,$(filter wamr,$(TINYCONTAINER_RUNTIME)))
export WAMR_CONFIG := $(abspath wamr_config.cmake)
endif

CFLAGS += -DTINYCONTAINER_NUMBER_OF_CONTAINERS=${NUMBER_OF_CONTAINERS}
CFLAGS += -DTINYCONTAINER_MAX_SIZE_FOR_METADATA=${MAX_SIZE_FOR_METADATA}
CFLAGS += -DTINYCONTAINER_MAX_SIZE_FOR_CONTAINER=${MAX_SIZE_FOR_CONTAINER}

ifeq (1,$(TINYCONTAINER_NETWORK))

  ifeq ($(BOARD),native)
    $(error "TinyContainerNetwork isn't supported for native")
    false;
  endif

  # network stack - ble ipv6 coap
  CFLAGS += -DCONFIG_GNRC_IPV6_NIB_SLACC=1 -DNIMBLE_AUTOCONN_NODE_ID=\"TinyContainerNetwork\"
  USEMODULE += nimble_autoconn_ipsp
  USEMODULE += netdev_default
  USEMODULE += gnrc_ipv6_default
  USEMODULE += gnrc_icmpv6_echo
  USEMODULE += gcoap
  # the next module is deactivated, because, on debug mode, it generate an error
  # message on RIOT startup "NETOPT_TX_END_IRQ not implemented by driver", and
  # the ble interface is not stable anymore
  USEMODULE += netstats_l2
  USEMODULE += netstats_ipv6
  USEMODULE += netstats_rpl

endif

# shell
USEMODULE += shell
USEMODULE += shell_cmds_default
USEMODULE += ps
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include

# For now this goes after the inclusion of Makefile.include so Kconfig symbols
# are available. Only set configuration via CFLAGS if Kconfig is not being used
# for this module.
ifeq (1,$(TINYCONTAINER_NETWORK))

  ifndef CONFIG_KCONFIG_MODULE_GCOAP
  ## Uncomment to redefine port, for example use 61616 for RFC 6282 UDP compression.
  #GCOAP_PORT = 5683
  #CFLAGS += -DCONFIG_GCOAP_PORT=$(GCOAP_PORT)

  ## Uncomment to redefine request token length, max 8.
  #GCOAP_TOKENLEN = 2
  #CFLAGS += -DCONFIG_GCOAP_TOKENLEN=$(GCOAP_TOKENLEN)

  # Increase from default for confirmable block2 follow-on requests
  GCOAP_RESEND_BUFS_MAX ?= 2
  CFLAGS += -DCONFIG_GCOAP_RESEND_BUFS_MAX=$(GCOAP_RESEND_BUFS_MAX)
  endif

  # Increase buffer used to build CoAP request or response to 1204 (64 octets for
  # metadata, 1024 for code+data, and 116 octets for CoAP structure
  CFLAGS += -DCONFIG_GCOAP_PDU_BUF_SIZE=1204

endif
