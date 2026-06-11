

#define __FirmwareUpdate_C

#include "stm32f10x.h"
#include "FirmwareUpdate.h"

#define ADDR_FLASH_PAGE_15     ((uint32_t)0x08007800) /* Base @ of Page 15, 2 Kbytes */
void FW_update(void)
{
  FLASH_Unlock();
  FLASH_ClearFlag(FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR | FLASH_FLAG_EOP);
  
  if((*(__IO uint32_t*)ADDR_FLASH_PAGE_15) != 0xFFFFFFFF)
  {
    if(FLASH_ErasePage(ADDR_FLASH_PAGE_15) == FLASH_COMPLETE)
    {
      if(FLASH_ProgramWord(ADDR_FLASH_PAGE_15, 0xABCD) == FLASH_COMPLETE)
      {
        FLASH_Lock();
        NVIC_SystemReset();
      }
      else
      {
        FLASH_Lock();
      }
    }
  }            
  else
  {
    if(FLASH_ProgramWord(ADDR_FLASH_PAGE_15, 0xABCD) == FLASH_COMPLETE)
    {
      FLASH_Lock();
      NVIC_SystemReset();
    }
    else
    {
      FLASH_Lock();
    }
  }
  
}

