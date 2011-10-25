/*
 * (C) Copyright 2003
 * Author : Pawel Dienwebel (Pelos) (www.pelos.pl)
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
 */

#include <at91rm9200_net.h>
#include <net.h>
#include <dm9161.h>
#include <rtl8201bl.h>

#if defined(CONFIG_RTL8201BL)
#if defined(CONFIG_CMD_NET)




void PhyReset(AT91PS_EMAC p_mac)
{
    static int init_done=0;
    unsigned short IntValue;
	unsigned Status;

    if (init_done)
	return;

    init_done=1;

    printf("Phy rtl8201 reset \n");

	at91rm9200_EmacEnableMDIO (p_mac);
    // first software reset the RTL8201      
	IntValue = RTL8201_CNTL_RESET;
	at91rm9200_EmacWritePhy (p_mac, RTL8201_CNTL_REG, &IntValue);
	IntValue = 0;
	at91rm9200_EmacWritePhy (p_mac, RTL8201_CNTL_REG, &IntValue);
/*	
    MiiStationWrite(RTL8201_CNTL_REG, RTL8201_ADDR, RTL8201_CNTL_RESET);
    MiiStationWrite(RTL8201_CNTL_REG, RTL8201_ADDR, 0);
*/
/*
#if 1
    // initialize auto-negotiation capabilities
    MiiStationWrite(RTL8201_ANA_REG,RTL8201_ADDR, 
                    RTL8201_ANA_100TX_FULL+
                    RTL8201_ANA_100TX+
                    RTL8201_ANA_10T_FULL+
                    RTL8201_ANA_10T+
                    RTL8201_ANA_SEL_802_3);
    // Now start an auto negotiation
    debug2_printf("Start auto negotiation\n");
    MiiStationWrite(RTL8201_CNTL_REG, RTL8201_ADDR, 
                    RTL8201_CNTL_AN+
                    RTL8201_CNTL_RSTRT_AN);
#elif 0
#warning force to 10M full duplex
    // initialize auto-negotiation capabilities
    MiiStationWrite(RTL8201_ANA_REG,RTL8201_ADDR, RTL8201_ANA_10T_FULL+RTL8201_ANA_10T+RTL8201_ANA_SEL_802_3);
    // force to 10M full duplex
    MiiStationWrite(RTL8201_CNTL_REG, RTL8201_ADDR, RTL8201_CNTL_FULL_DUP);
#else
#warning force to 10M half duplex
    // initialize auto-negotiation capabilities
    MiiStationWrite(RTL8201_ANA_REG,RTL8201_ADDR, RTL8201_ANA_10T+RTL8201_ANA_SEL_802_3);
    // force to 10M half duplex
    MiiStationWrite(RTL8201_CNTL_REG, RTL8201_ADDR, 0);
#endif
*/
}



/*
 * Name:
 *	rtl8201bl_IsPhyConnected
 * Description:
 *	Reads the 2 PHY ID registers
 * Arguments:
 *	p_mac - pointer to AT91S_EMAC struct
 * Return value:
 *	TRUE - if id read successfully
 *	FALSE- if error
 */
unsigned int rtl8201bl_IsPhyConnected (AT91PS_EMAC p_mac)
{
	unsigned short Id1, Id2;

	at91rm9200_EmacEnableMDIO (p_mac);
	at91rm9200_EmacReadPhy (p_mac, RTL8201_ID_REG1, &Id1);
	at91rm9200_EmacReadPhy (p_mac, RTL8201_ID_REG2, &Id2);
	at91rm9200_EmacDisableMDIO (p_mac);
	
	printf ("RTL8201BL id1[0x%02x] id2[0x%02x]\r\n", Id1, Id2);

	if ((Id1 == RTL8201_ID_REG1_VAL) && (Id2 == RTL8201_ID_REG2_VAL)) return TRUE;

	return FALSE;
}

/*
 * Name:
 *	rtl8201bl_GetLinkSpeed
 * Description:
 *	Link parallel detection status of MAC is checked and set in the
 *	MAC configuration registers
 * Arguments:
 *	p_mac - pointer to MAC
 * Return value:
 *	TRUE - if link status set succesfully
 *	FALSE - if link status not set
 */
UCHAR rtl8201bl_GetLinkSpeed (AT91PS_EMAC p_mac)
{
	unsigned short stat1, stat2;

	if (!at91rm9200_EmacReadPhy (p_mac, DM9161_BMSR, &stat1))
		return FALSE;

	if (!(stat1 & DM9161_LINK_STATUS))	/* link status up? */
		return FALSE;

	if (!at91rm9200_EmacReadPhy (p_mac, DM9161_DSCSR, &stat2))
		return FALSE;

	if ((stat1 & DM9161_100BASE_TX_FD) && (stat2 & DM9161_100FDX)) {
		/*set Emac for 100BaseTX and Full Duplex  */
		p_mac->EMAC_CFG |= AT91C_EMAC_SPD | AT91C_EMAC_FD;
		return TRUE;
	}

	if ((stat1 & DM9161_10BASE_T_FD) && (stat2 & DM9161_10FDX)) {
		/*set MII for 10BaseT and Full Duplex  */
		p_mac->EMAC_CFG = (p_mac->EMAC_CFG &
				~(AT91C_EMAC_SPD | AT91C_EMAC_FD))
				| AT91C_EMAC_FD;
		return TRUE;
	}

	if ((stat1 & DM9161_100BASE_TX_HD) && (stat2 & DM9161_100HDX)) {
		/*set MII for 100BaseTX and Half Duplex  */
		p_mac->EMAC_CFG = (p_mac->EMAC_CFG &
				~(AT91C_EMAC_SPD | AT91C_EMAC_FD))
				| AT91C_EMAC_SPD;
		return TRUE;
	}

	if ((stat1 & DM9161_10BASE_T_HD) && (stat2 & DM9161_10HDX)) {
		/*set MII for 10BaseT and Half Duplex  */
		p_mac->EMAC_CFG &= ~(AT91C_EMAC_SPD | AT91C_EMAC_FD);
		return TRUE;
	}
	return FALSE;
}


/*
 * Name:
 *	rtl8201bl_InitPhy
 * Description:
 *	MAC starts checking its link by using parallel detection and
 *	Autonegotiation and the same is set in the MAC configuration registers
 * Arguments:
 *	p_mac - pointer to struct AT91S_EMAC
 * Return value:
 *	TRUE - if link status set succesfully
 *	FALSE - if link status not set
 */
UCHAR rtl8201bl_InitPhy (AT91PS_EMAC p_mac)
{
	UCHAR ret = TRUE;
	unsigned short IntValue;
#if 0
	at91rm9200_EmacEnableMDIO (p_mac);

	if (!rtl8201bl_GetLinkSpeed (p_mac)) {
		/* Try another time */
		ret = rtl8201bl_GetLinkSpeed (p_mac);
	}

	/* Disable PHY Interrupts */
	at91rm9200_EmacReadPhy (p_mac, DM9161_MDINTR, &IntValue);
	/* set FDX, SPD, Link, INTR masks */
	IntValue |= (DM9161_FDX_MASK | DM9161_SPD_MASK |
		     DM9161_LINK_MASK | DM9161_INTR_MASK);
	at91rm9200_EmacWritePhy (p_mac, DM9161_MDINTR, &IntValue);
	at91rm9200_EmacDisableMDIO (p_mac);
#endif
	return (ret);
}


/*
 * Name:
 *	rtl8201bl_AutoNegotiate
 * Description:
 *	MAC Autonegotiates with the partner status of same is set in the
 *	MAC configuration registers
 * Arguments:
 *	dev - pointer to struct net_device
 * Return value:
 *	TRUE - if link status set successfully
 *	FALSE - if link status not set
 */
UCHAR rtl8201bl_AutoNegotiate (AT91PS_EMAC p_mac, int *status)
{
	unsigned short value;
	unsigned short PhyAnar;
	unsigned short PhyAnalpar;
#if 0
	/* Set rtl8201bl control register */
	if (!at91rm9200_EmacReadPhy (p_mac, DM9161_BMCR, &value))
		return FALSE;
	value &= ~DM9161_AUTONEG;	/* remove autonegotiation enable */
	value |= DM9161_ISOLATE;	/* Electrically isolate PHY */
	if (!at91rm9200_EmacWritePhy (p_mac, DM9161_BMCR, &value))
		return FALSE;

	/* Set the Auto_negotiation Advertisement Register */
	/* MII advertising for Next page, 100BaseTxFD and HD, 10BaseTFD and HD, IEEE 802.3 */
	PhyAnar = DM9161_NP | DM9161_TX_FDX | DM9161_TX_HDX |
		  DM9161_10_FDX | DM9161_10_HDX | DM9161_AN_IEEE_802_3;
	if (!at91rm9200_EmacWritePhy (p_mac, DM9161_ANAR, &PhyAnar))
		return FALSE;

	/* Read the Control Register     */
	if (!at91rm9200_EmacReadPhy (p_mac, DM9161_BMCR, &value))
		return FALSE;

	value |= DM9161_SPEED_SELECT | DM9161_AUTONEG | DM9161_DUPLEX_MODE;
	if (!at91rm9200_EmacWritePhy (p_mac, DM9161_BMCR, &value))
		return FALSE;
	/* Restart Auto_negotiation  */
	value |= DM9161_RESTART_AUTONEG;
	value &= ~DM9161_ISOLATE;
	if (!at91rm9200_EmacWritePhy (p_mac, DM9161_BMCR, &value))
		return FALSE;

	/*check AutoNegotiate complete */
	udelay (10000);
	at91rm9200_EmacReadPhy (p_mac, DM9161_BMSR, &value);
	if (!(value & DM9161_AUTONEG_COMP))
		return FALSE;

	/* Get the AutoNeg Link partner base page */
	if (!at91rm9200_EmacReadPhy (p_mac, DM9161_ANLPAR, &PhyAnalpar))
		return FALSE;

	if ((PhyAnar & DM9161_TX_FDX) && (PhyAnalpar & DM9161_TX_FDX)) {
		/*set MII for 100BaseTX and Full Duplex  */
		p_mac->EMAC_CFG |= AT91C_EMAC_SPD | AT91C_EMAC_FD;
		return TRUE;
	}

	if ((PhyAnar & DM9161_10_FDX) && (PhyAnalpar & DM9161_10_FDX)) {
		/*set MII for 10BaseT and Full Duplex  */
		p_mac->EMAC_CFG = (p_mac->EMAC_CFG &
				~(AT91C_EMAC_SPD | AT91C_EMAC_FD))
				| AT91C_EMAC_FD;
		return TRUE;
	}
#endif	
	return FALSE;
}

#endif	/* CONFIG_COMMANDS & CFG_CMD_NET */
#endif	/* CONFIG_RTL8201BL */
