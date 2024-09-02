# TinyContainer
#
#  Copyright (C) 2024 Orange
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


MODULE := tinycontainer_security_crypto

CRYPTO_PSEUDOMODULES :=

ifneq (,$(filter tinycontainer_security_crypto_none,$(USEMODULE)))
  CRYPTO_PSEUDOMODULES += tinycontainer_security_crypto_none
endif

ifneq (,$(filter tinycontainer_security_crypto_psa,$(USEMODULE)))
  CRYPTO_PSEUDOMODULES += tinycontainer_security_crypto_psa
endif

CRYPTO_COUNT := $(words $(CRYPTO_PSEUDOMODULES))

# if count is not 1, then there is a problem
ifeq ($(CRYPTO_COUNT),1)
  CRYPTO := $(firstword $(CRYPTO_PSEUDOMODULES))
else
  $(error "tinycontainer_security_crypto: exactly one crypto backend must be selected")
endif

PSEUDOMODULES += $(CRYPTO_PSEUDOMODULES)


include $(RIOTBASE)/Makefile.base
