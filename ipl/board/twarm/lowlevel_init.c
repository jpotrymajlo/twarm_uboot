/******************************************************************************
 *  FILE NAME  init.c
 *
 *
 *  DESCRIPTION
 *
 *  AUTHOR     Jacek Potrymajlo
 *
 *  CREATED    10/16/2008
 ******************************************************************************/

#include <common.h>
#include <asm/arch/AT91RM9200.h>
#include <asm/arch/lib_AT91RM9200.h>
#include <asm/arch/hardware.h>




#define outl(addr,data) *(volatile unsigned int*)(addr) = (data)
#define inl(addr) *(volatile unsigned int*)(addr)

void INIT_SetupPll(void)
{
    AT91_REG tmp;
    AT91PS_PMC pPmc = AT91C_BASE_PMC;
    AT91PS_CKGR pCkgr = AT91C_BASE_CKGR;

    pPmc->PMC_IDR = 0xFFFFFFFF;

    /* -Setup the PLL A */
    pCkgr->CKGR_PLLAR = 0x20269004;

    while (!(*AT91C_PMC_SR & AT91C_PMC_LOCKA));

    /* - Switch Master Clock from PLLB to PLLA/3 */
    tmp = pPmc->PMC_MCKR;
    /* See Atmel Errata #27 and #28 */
    if (tmp & 0x0000001C)
    {
        tmp = (tmp & ~0x0000001C);
        pPmc->PMC_MCKR = tmp;
        while (!(*AT91C_PMC_SR & AT91C_PMC_MCKRDY));
    }
    if (tmp != 0x00000202)
    {
        pPmc->PMC_MCKR = 0x00000202;
        if ((tmp & 0x00000003) != 0x00000002)
            while (!(*AT91C_PMC_SR & AT91C_PMC_MCKRDY));
    }
}

/*
 *
 */
void INIT_SetupDbgu(void)
{
    AT91F_DBGU_CfgPIO();

    AT91F_US_ResetRx((AT91PS_USART)AT91C_BASE_DBGU);
    AT91F_US_ResetTx((AT91PS_USART)AT91C_BASE_DBGU);

    // Configure DBGU
    AT91F_US_Configure((AT91PS_USART)AT91C_BASE_DBGU, AT91C_MASTER_CLOCK,         
         AT91C_US_ASYNC_MODE, 115200, 0);

    // Enable Transmitter
    AT91F_US_EnableTx((AT91PS_USART)AT91C_BASE_DBGU);
    // Enable Receiver
    AT91F_US_EnableRx((AT91PS_USART)AT91C_BASE_DBGU);
}

/*
 *
 */
void INIT_SetupSdram(void)
{
  int i=0;

  outl(0xFFFFF804, 0xFFFF0000); //AT91F_PIO_CfgPeriph
  for(i = 0; i < 100; i++);  
  outl(AT91C_EBI_CSA, 0x2);
  for(i = 0; i < 100; i++);  
  outl(AT91C_SDRC_CR, 0x2188A159); //takie wyliczylem
  for(i = 0; i < 100; i++);
  outl(AT91C_SDRC_MR, 0x02); //All banks precharge and 32bits
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91C_SDRC_MR, 0x04); //Refresh and 32 bits
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91_SDRAM_BASE, 0);
  outl(AT91C_SDRC_MR, 0x03);  //Load Mode Register and 32bits
  for(i = 0; i < 100; i++);
  outl(0x20000080, 0); 
  outl(AT91C_SDRC_TR, 0x1A0); //1CA
  outl(AT91_SDRAM_BASE, 0); 
  outl(AT91C_SDRC_MR, 0x00);  //Normal Mode and 32bits
}

void lowlevel_init(void)
{
  INIT_SetupPll();
  INIT_SetupSdram();
  serial_init();
}

void *malloc(size_t len)
{
  return (void*)0;
}

void hang(void)
{
  while (1);
}

int sprintf(char * buf, const char *fmt, ...)
{
  return 0;
}

int setenv (char *varname, char *varvalue)
{
  return 0;
}

void printf (const char *fmt, ...)
{
  serial_puts(fmt);
}

