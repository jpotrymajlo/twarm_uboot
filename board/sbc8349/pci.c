/*
 * pci.c -- WindRiver SBC8349 PCI board support.
 * Copyright (c) 2006 Wind River Systems, Inc.
 *
 * Based on MPC8349 PCI support but w/o PIB related code.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

#include <asm/mmu.h>
#include <common.h>
#include <asm/global_data.h>
#include <pci.h>
#include <asm/mpc8349_pci.h>
#include <i2c.h>
#if defined(CONFIG_OF_LIBFDT)
#include <libfdt.h>
#include <fdt_support.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_PCI

/* System RAM mapped to PCI space */
#define CONFIG_PCI_SYS_MEM_BUS	CONFIG_SYS_SDRAM_BASE
#define CONFIG_PCI_SYS_MEM_PHYS	CONFIG_SYS_SDRAM_BASE

#ifndef CONFIG_PCI_PNP
static struct pci_config_table pci_mpc8349emds_config_table[] = {
	{PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID,
	 PCI_IDSEL_NUMBER, PCI_ANY_ID,
	 pci_cfgfunc_config_device, {PCI_ENET0_IOADDR,
				     PCI_ENET0_MEMADDR,
				     PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER
		}
	},
	{}
};
#endif

static struct pci_controller pci_hose[] = {
       {
#ifndef CONFIG_PCI_PNP
       config_table:pci_mpc8349emds_config_table,
#endif
       },
       {
#ifndef CONFIG_PCI_PNP
       config_table:pci_mpc8349emds_config_table,
#endif
       }
};

/**************************************************************************
 * pci_init_board()
 *
 * NOTICE: PCI2 is not supported. There is only one
 * physical PCI slot on the board.
 *
 */
void
pci_init_board(void)
{
	volatile immap_t *	immr;
	volatile clk83xx_t *	clk;
	volatile law83xx_t *	pci_law;
	volatile pot83xx_t *	pci_pot;
	volatile pcictrl83xx_t *	pci_ctrl;
	volatile pciconf83xx_t *	pci_conf;
	u16 reg16;
	u32 reg32;
	u32 dev;
	struct	pci_controller * hose;

	immr = (immap_t *)CONFIG_SYS_IMMR;
	clk = (clk83xx_t *)&immr->clk;
	pci_law = immr->sysconf.pcilaw;
	pci_pot = immr->ios.pot;
	pci_ctrl = immr->pci_ctrl;
	pci_conf = immr->pci_conf;

	hose = &pci_hose[0];

	/*
	 * Configure PCI controller and PCI_CLK_OUTPUT both in 66M mode
	 */

	reg32 = clk->occr;
	udelay(2000);
	clk->occr = 0xff000000;
	udelay(2000);

	/*
	 * Release PCI RST Output signal
	 */
	pci_ctrl[0].gcr = 0;
	udelay(2000);
	pci_ctrl[0].gcr = 1;

#ifdef CONFIG_MPC83XX_PCI2
	pci_ctrl[1].gcr = 0;
	udelay(2000);
	pci_ctrl[1].gcr = 1;
#endif

	/* We need to wait at least a 1sec based on PCI specs */
	{
		int i;

		for (i = 0; i < 1000; ++i)
			udelay (1000);
	}

	/*
	 * Configure PCI Local Access Windows
	 */
	pci_law[0].bar = CONFIG_SYS_PCI1_MEM_PHYS & LAWBAR_BAR;
	pci_law[0].ar = LAWAR_EN | LAWAR_SIZE_1G;

	pci_law[1].bar = CONFIG_SYS_PCI1_IO_PHYS & LAWBAR_BAR;
	pci_law[1].ar = LAWAR_EN | LAWAR_SIZE_4M;

	/*
	 * Configure PCI Outbound Translation Windows
	 */

	/* PCI1 mem space - prefetch */
	pci_pot[0].potar = (CONFIG_SYS_PCI1_MEM_BASE >> 12) & POTAR_TA_MASK;
	pci_pot[0].pobar = (CONFIG_SYS_PCI1_MEM_PHYS >> 12) & POBAR_BA_MASK;
	pci_pot[0].pocmr = POCMR_EN | POCMR_PREFETCH_EN | (POCMR_CM_256M & POCMR_CM_MASK);

	/* PCI1 IO space */
	pci_pot[1].potar = (CONFIG_SYS_PCI1_IO_BASE >> 12) & POTAR_TA_MASK;
	pci_pot[1].pobar = (CONFIG_SYS_PCI1_IO_PHYS >> 12) & POBAR_BA_MASK;
	pci_pot[1].pocmr = POCMR_EN | POCMR_IO | (POCMR_CM_1M & POCMR_CM_MASK);

	/* PCI1 mmio - non-prefetch mem space */
	pci_pot[2].potar = (CONFIG_SYS_PCI1_MMIO_BASE >> 12) & POTAR_TA_MASK;
	pci_pot[2].pobar = (CONFIG_SYS_PCI1_MMIO_PHYS >> 12) & POBAR_BA_MASK;
	pci_pot[2].pocmr = POCMR_EN | (POCMR_CM_256M & POCMR_CM_MASK);

	/*
	 * Configure PCI Inbound Translation Windows
	 */

	/* we need RAM mapped to PCI space for the devices to
	 * access main memory */
	pci_ctrl[0].pitar1 = 0x0;
	pci_ctrl[0].pibar1 = 0x0;
	pci_ctrl[0].piebar1 = 0x0;
	pci_ctrl[0].piwar1 = PIWAR_EN | PIWAR_PF | PIWAR_RTT_SNOOP | PIWAR_WTT_SNOOP | (__ilog2(gd->ram_size) - 1);

	hose->first_busno = 0;
	hose->last_busno = 0xff;

	/* PCI memory prefetch space */
	pci_set_region(hose->regions + 0,
		       CONFIG_SYS_PCI1_MEM_BASE,
		       CONFIG_SYS_PCI1_MEM_PHYS,
		       CONFIG_SYS_PCI1_MEM_SIZE,
		       PCI_REGION_MEM|PCI_REGION_PREFETCH);

	/* PCI memory space */
	pci_set_region(hose->regions + 1,
		       CONFIG_SYS_PCI1_MMIO_BASE,
		       CONFIG_SYS_PCI1_MMIO_PHYS,
		       CONFIG_SYS_PCI1_MMIO_SIZE,
		       PCI_REGION_MEM);

	/* PCI IO space */
	pci_set_region(hose->regions + 2,
		       CONFIG_SYS_PCI1_IO_BASE,
		       CONFIG_SYS_PCI1_IO_PHYS,
		       CONFIG_SYS_PCI1_IO_SIZE,
		       PCI_REGION_IO);

	/* System memory space */
	pci_set_region(hose->regions + 3,
		       CONFIG_PCI_SYS_MEM_BUS,
		       CONFIG_PCI_SYS_MEM_PHYS,
		       gd->ram_size,
		       PCI_REGION_MEM | PCI_REGION_SYS_MEMORY);

	hose->region_count = 4;

	pci_setup_indirect(hose,
			   (CONFIG_SYS_IMMR+0x8300),
			   (CONFIG_SYS_IMMR+0x8304));

	pci_register_hose(hose);

	/*
	 * Write to Command register
	 */
	reg16 = 0xff;
	dev = PCI_BDF(hose->first_busno, 0, 0);
	pci_hose_read_config_word (hose, dev, PCI_COMMAND, &reg16);
	reg16 |= PCI_COMMAND_SERR | PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_hose_write_config_word(hose, dev, PCI_COMMAND, reg16);

	/*
	 * Clear non-reserved bits in status register.
	 */
	pci_hose_write_config_word(hose, dev, PCI_STATUS, 0xffff);
	pci_hose_write_config_byte(hose, dev, PCI_LATENCY_TIMER, 0x80);
	pci_hose_write_config_byte(hose, dev, PCI_CACHE_LINE_SIZE, 0x08);

#ifdef CONFIG_PCI_SCAN_SHOW
	printf("PCI:   Bus Dev VenId DevId Class Int\n");
#endif
	/*
	 * Hose scan.
	 */
	hose->last_busno = pci_hose_scan(hose);

#ifdef CONFIG_MPC83XX_PCI2
	hose = &pci_hose[1];

	/*
	 * Configure PCI Outbound Translation Windows
	 */

	/* PCI2 mem space - prefetch */
	pci_pot[3].potar = (CONFIG_SYS_PCI2_MEM_BASE >> 12) & POTAR_TA_MASK;
	pci_pot[3].pobar = (CONFIG_SYS_PCI2_MEM_PHYS >> 12) & POBAR_BA_MASK;
	pci_pot[3].pocmr = POCMR_EN | POCMR_PCI2 | POCMR_PREFETCH_EN | (POCMR_CM_256M & POCMR_CM_MASK);

	/* PCI2 IO space */
	pci_pot[4].potar = (CONFIG_SYS_PCI2_IO_BASE >> 12) & POTAR_TA_MASK;
	pci_pot[4].pobar = (CONFIG_SYS_PCI2_IO_PHYS >> 12) & POBAR_BA_MASK;
	pci_pot[4].pocmr = POCMR_EN | POCMR_PCI2 | POCMR_IO | (POCMR_CM_1M & POCMR_CM_MASK);

	/* PCI2 mmio - non-prefetch mem space */
	pci_pot[5].potar = (CONFIG_SYS_PCI2_MMIO_BASE >> 12) & POTAR_TA_MASK;
	pci_pot[5].pobar = (CONFIG_SYS_PCI2_MMIO_PHYS >> 12) & POBAR_BA_MASK;
	pci_pot[5].pocmr = POCMR_EN | POCMR_PCI2 | (POCMR_CM_256M & POCMR_CM_MASK);

	/*
	 * Configure PCI Inbound Translation Windows
	 */

	/* we need RAM mapped to PCI space for the devices to
	 * access main memory */
	pci_ctrl[1].pitar1 = 0x0;
	pci_ctrl[1].pibar1 = 0x0;
	pci_ctrl[1].piebar1 = 0x0;
	pci_ctrl[1].piwar1 = PIWAR_EN | PIWAR_PF | PIWAR_RTT_SNOOP | PIWAR_WTT_SNOOP | (__ilog2(gd->ram_size) - 1);

	hose->first_busno = pci_hose[0].last_busno + 1;
	hose->last_busno = 0xff;

	/* PCI memory prefetch space */
	pci_set_region(hose->regions + 0,
		       CONFIG_SYS_PCI2_MEM_BASE,
		       CONFIG_SYS_PCI2_MEM_PHYS,
		       CONFIG_SYS_PCI2_MEM_SIZE,
		       PCI_REGION_MEM|PCI_REGION_PREFETCH);

	/* PCI memory space */
	pci_set_region(hose->regions + 1,
		       CONFIG_SYS_PCI2_MMIO_BASE,
		       CONFIG_SYS_PCI2_MMIO_PHYS,
		       CONFIG_SYS_PCI2_MMIO_SIZE,
		       PCI_REGION_MEM);

	/* PCI IO space */
	pci_set_region(hose->regions + 2,
		       CONFIG_SYS_PCI2_IO_BASE,
		       CONFIG_SYS_PCI2_IO_PHYS,
		       CONFIG_SYS_PCI2_IO_SIZE,
		       PCI_REGION_IO);

	/* System memory space */
	pci_set_region(hose->regions + 3,
		       CONFIG_PCI_SYS_MEM_BUS,
		       CONFIG_PCI_SYS_MEM_PHYS,
		       gd->ram_size,
		       PCI_REGION_MEM | PCI_REGION_SYS_MEMORY);

	hose->region_count = 4;

	pci_setup_indirect(hose,
			   (CONFIG_SYS_IMMR+0x8380),
			   (CONFIG_SYS_IMMR+0x8384));

	pci_register_hose(hose);

	/*
	 * Write to Command register
	 */
	reg16 = 0xff;
	dev = PCI_BDF(hose->first_busno, 0, 0);
	pci_hose_read_config_word (hose, dev, PCI_COMMAND, &reg16);
	reg16 |= PCI_COMMAND_SERR | PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_hose_write_config_word(hose, dev, PCI_COMMAND, reg16);

	/*
	 * Clear non-reserved bits in status register.
	 */
	pci_hose_write_config_word(hose, dev, PCI_STATUS, 0xffff);
	pci_hose_write_config_byte(hose, dev, PCI_LATENCY_TIMER, 0x80);
	pci_hose_write_config_byte(hose, dev, PCI_CACHE_LINE_SIZE, 0x08);

	/*
	 * Hose scan.
	 */
	hose->last_busno = pci_hose_scan(hose);
#endif

}

#if defined(CONFIG_OF_LIBFDT)
void ft_pci_setup(void *blob, bd_t *bd)
{
	int nodeoffset;
	int tmp[2];
	const char *path;

	nodeoffset = fdt_path_offset(blob, "/aliases");
	if (nodeoffset >= 0) {
		path = fdt_getprop(blob, nodeoffset, "pci0", NULL);
		if (path) {
			tmp[0] = cpu_to_be32(pci_hose[0].first_busno);
			tmp[1] = cpu_to_be32(pci_hose[0].last_busno);
			do_fixup_by_path(blob, path, "bus-range",
				&tmp, sizeof(tmp), 1);

			tmp[0] = cpu_to_be32(gd->pci_clk);
			do_fixup_by_path(blob, path, "clock-frequency",
				&tmp, sizeof(tmp[0]), 1);
		}
#ifdef CONFIG_MPC83XX_PCI2
		path = fdt_getprop(blob, nodeoffset, "pci1", NULL);
		if (path) {
			tmp[0] = cpu_to_be32(pci_hose[0].first_busno);
			tmp[1] = cpu_to_be32(pci_hose[0].last_busno);
			do_fixup_by_path(blob, path, "bus-range",
				&tmp, sizeof(tmp), 1);

			tmp[0] = cpu_to_be32(gd->pci_clk);
			do_fixup_by_path(blob, path, "clock-frequency",
				&tmp, sizeof(tmp[0]), 1);
		}
#endif
	}
}
#endif /* CONFIG_OF_LIBFDT */
#endif /* CONFIG_PCI */
