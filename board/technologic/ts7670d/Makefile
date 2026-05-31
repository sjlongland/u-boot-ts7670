#
# (C) Copyright 2000-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# SPDX-License-Identifier:	GPL-2.0+
#

obj-y	:= ts7670d.o

ifndef	CONFIG_SPL_BUILD
obj-y   += led.o
else
obj-y	+= iomux.o
endif

