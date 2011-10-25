/*
 * File name:  main.c
 * Description:
 *
 *
 *
 */
#include <common.h>
#include <dataflash.h>

extern void AT91F_DataflashInit(void);


/* 
 * Entry point from the startup file 
 */
void start_armboot(void)
{

  unsigned long start_address = 0;
  unsigned long end_address = 0;
  unsigned long partition_length = 0;
  int i;

  AT91F_DataflashInit();
  
  for (i = 0; i<NB_DATAFLASH_AREA; i++)
  {
      /* find u-boot start and end address area*/
      if (!strcmp("U-Boot", dataflash_info[0].Device.area_list[i].label))
      {
          start_address = dataflash_info[0].Device.area_list[i].start;
          end_address   = dataflash_info[0].Device.area_list[i].end;
          break;
      }
  }
  if (i < NB_DATAFLASH_AREA)
  {
      partition_length = end_address - start_address + 1;     
      read_dataflash(start_address, partition_length, (char*)0x21F00000);
      asm("ldr pc, =0x21F00000");
  }
  
  while (1);
}

