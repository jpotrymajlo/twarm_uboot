#ifndef _rtl8201bl_h_
#define  _rtl8201bl_h_

// RTL8201 register offsets
#define	RTL8201_CNTL_REG 	0x00
#define	RTL8201_STATUS_REG 	0x01
#define	RTL8201_ID_REG1    	0x02
#define	RTL8201_ID_REG2    	0x03
#define	RTL8201_ANA_REG    	0x04
#define	RTL8201_ANLPA_REG 	0x05
#define	RTL8201_ANE_REG    	0x06
#define	RTL8201_NSR_REG    	0x10
#define	RTL8201_LBREMR_REG    	0x11
#define	RTL8201_RXER_REG    	0x12
#define	RTL8201_10M_NIC_REG    	0x13
#define	RTL8201_PHY1_1_REG    	0x14
#define	RTL8201_PHY1_2_REG    	0x15
#define	RTL8201_PHY2_REG    	0x16
#define	RTL8201_TWISTER1_REG   	0x17
#define	RTL8201_TWISTER2_REG   	0x18
#define	RTL8201_TEST_REG   	0x19

// RTL8201 Control register bit defines
#define RTL8201_CNTL_RESET        0x8000
#define RTL8201_CNTL_LOOPBACK     0x4000
#define RTL8201_CNTL_SPEED        0x2000  // 1=100Meg, 0=10Meg
#define RTL8201_CNTL_AN           0x1000  // 1=Enable auto negotiation, 0=disable it
#define RTL8201_CNTL_PWRDN        0x0800  // 1=Enable power down
#define RTL8201_CNTL_RSTRT_AN     0x0200  // 1=Restart Auto Negotioation process
#define RTL8201_CNTL_FULL_DUP     0x0100  // 1=Enable full duplex mode, 0=half dup


#define	RTL8201_ID_REG1_VAL    	0x0000
#define	RTL8201_ID_REG2_VAL    	0x8201

#define Bit(n) (1<<(n))

#define RTL8201_STATUS_100T4        Bit(15)
#define RTL8201_STATUS_100TX_FULL   Bit(14)
#define RTL8201_STATUS_100TX        Bit(13)
#define RTL8201_STATUS_10T_FULL     Bit(12)
#define RTL8201_STATUS_10T          Bit(11)
#define RTL8201_STATUS_AN_COMLETE   Bit(5)
#define RTL8201_STATUS_LINKUP       Bit(2)

#define RTL8201_ANA_PAUSE_ENA    Bit(10)
#define RTL8201_ANA_100T4        Bit(9)
#define RTL8201_ANA_100TX_FULL   Bit(8)
#define RTL8201_ANA_100TX        Bit(7)
#define RTL8201_ANA_10T_FULL     Bit(6)
#define RTL8201_ANA_10T          Bit(5)
#define RTL8201_ANA_SEL_802_3    Bit(0)

#define RTL8201_TEST_LINK_10   Bit(1)
#define RTL8201_TEST_LINK_100  Bit(0)
#define RTL8201_TEST_PHY_ADR   (0x1f<<8)



unsigned int rtl8201bl_IsPhyConnected (AT91PS_EMAC p_mac);
UCHAR rtl8201bl_GetLinkSpeed (AT91PS_EMAC p_mac);
UCHAR rtl8201bl_InitPhy (AT91PS_EMAC p_mac);
UCHAR rtl8201bl_AutoNegotiate (AT91PS_EMAC p_mac, int *status);




#endif

