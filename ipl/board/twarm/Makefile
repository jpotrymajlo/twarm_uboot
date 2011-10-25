#
# (C) Copyright 2006-2007
# Stefan Roese, DENX Software Engineering, sr@denx.de.
#
# (C) Copyright 2008
# Guennadi Liakhovetki, DENX Software Engineering, <lg@denx.de>
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

CONFIG_UBOOT_IPL	= y

include $(TOPDIR)/config.mk
include $(TOPDIR)/ipl/board/$(BOARDDIR)/config.mk

LDSCRIPT= $(TOPDIR)/ipl/board/$(BOARDDIR)/u-boot.lds
LDFLAGS	= -Bstatic -T $(LDSCRIPT) -Ttext $(TEXT_BASE) $(PLATFORM_LDFLAGS)
AFLAGS	+= -DCONFIG_UBOOT_IPL
CFLAGS	+= -DCONFIG_UBOOT_IPL

#subst TEXT_BASE macro with the new one
IPL_AFLAGS = $(patsubst -DTEXT_BASE=%,-DTEXT_BASE=$(TEXT_BASE),$(AFLAGS))
IPL_CFLAGS = $(patsubst -DTEXT_BASE=%,-DTEXT_BASE=$(TEXT_BASE),$(CFLAGS))


SOBJS	= start.o _udivsi3.o _umodsi3.o
COBJS	= main.o lowlevel_init.o serial.o div0.o dataflash.o partition.o \
	  spi.o at45.o string.o interrupts.o

SRCS	:= $(addprefix $(obj),$(SOBJS:.o=.S) $(COBJS:.o=.c))
OBJS	:= $(addprefix $(obj),$(SOBJS) $(COBJS))
__OBJS	:= $(SOBJS) $(COBJS)
LNDIR	:= $(OBJTREE)/ipl/board/$(BOARDDIR)

nandobj	:= $(OBJTREE)/ipl/

ALL	= $(nandobj)u-boot-ipl $(nandobj)u-boot-ipl.bin $(nandobj)u-boot-ipl-12k.bin

all:	$(obj).depend $(ALL)

$(nandobj)u-boot-ipl-12k.bin: $(nandobj)u-boot-ipl
	$(OBJCOPY) ${OBJCFLAGS} --pad-to=$(PAD_TO) -O binary $< $@

$(nandobj)u-boot-ipl.bin:	$(nandobj)u-boot-ipl
	$(OBJCOPY) ${OBJCFLAGS} -O binary $< $@

$(nandobj)u-boot-ipl:	$(OBJS)
	cd $(LNDIR) && $(LD) $(LDFLAGS) $(__OBJS) \
		-Map $(nandobj)u-boot-ipl.map \
		-o $(nandobj)u-boot-ipl

# create symbolic links for common files


# from board directory
$(obj)serial.c:
	@rm -f $@
	@ln -s $(TOPDIR)/cpu/arm920t/at91rm9200/serial.c $@

$(obj)partition.c:
	@rm -f $@
	@ln -s $(TOPDIR)/board/twarm/partition.c $@

$(obj)at45.c:
	@rm -f $@
	@ln -s $(TOPDIR)/drivers/mtd/at45.c $@

$(obj)dataflash.c:
	@rm -f $@
	@ln -s $(TOPDIR)/drivers/mtd/dataflash.c $@

$(obj)spi.c:
	@rm -f $@
	@ln -s $(TOPDIR)/cpu/arm920t/at91rm9200/spi.c $@

$(obj)string.c:
	@rm -f $@
	@ln -s $(TOPDIR)/lib_generic/string.c $@

$(obj)_udivsi3.S:
	@rm -f $@
	@ln -s $(TOPDIR)/lib_arm/_udivsi3.S $@

$(obj)_umodsi3.S:
	@rm -f $@
	@ln -s $(TOPDIR)/lib_arm/_umodsi3.S $@

$(obj)div0.c:
	@rm -f $@
	@ln -s $(TOPDIR)/lib_arm/div0.c $@

$(obj)interrupts.c:
	@rm -f $@
	@ln -s $(TOPDIR)/cpu/arm920t/at91rm9200/interrupts.c $@

#########################################################################

$(obj)%.o:	$(obj)%.S
	$(CC) $(IPL_AFLAGS) -c -o $@ $<

$(obj)%.o:	$(obj)%.c
	$(CC) $(IPL_CFLAGS) -c -o $@ $<

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
