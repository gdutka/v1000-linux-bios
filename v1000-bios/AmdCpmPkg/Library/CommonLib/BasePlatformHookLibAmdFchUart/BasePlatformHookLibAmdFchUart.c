/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformHookLib.h>

#define FCH_IOMUXx87_UART0_CTS_L_UART2_RXD_EGPIO135 0xFED80D87ul
#define FCH_IOMUXx89_UART0_RTS_L_EGPIO137        0xFED80D89ul
#define FCH_IOMUXx8A_UART0_TXD_EGPIO138          0xFED80D8Aul

#define UART1_CTS_L_UART3_TXD_EGPIO140           0xFED80D8Cul
#define FCH_IOMUXx8E_UART1_RTS_L_EGPIO142        0xFED80D8Eul
#define FCH_IOMUXx8F_UART1_TXD_EGPIO143          0xFED80D8Ful

#define FCH_AOACx40_D3_CONTROL                   0xFED81E40ul
#define FCH_AOACx56_UART0                        0x16          // UART0
#define FCH_AOACx58_UART1                        0x18          // UART1
#define FCH_AOACx62_AMBA                         0x22          // AMBA
#define AOAC_PWR_ON_DEV                          BIT3          // PwrOnDev

/**
  Performs platform specific initialization required for the CPU to access
  the hardware associated with a SerialPortLib instance.  This function does
  not intiailzie the serial port hardware itself.  Instead, it initializes
  hardware devices that are required for the CPU to access the serial port
  hardware.  This function may be called more than once.

  @retval RETURN_SUCCESS       The platform specific initialization succeeded.
  @retval RETURN_DEVICE_ERROR  The platform specific initialization could not be completed.

**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  //
  // Initialize for FCH UART
  //

  // AOAC 'PwrOnDev' bit of AMBA & UART
  if (PcdGet8 (PcdFchUartPort) == 0) {
    if ((MmioRead8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx62_AMBA) & 0x03) != 0x03) {
      MmioWrite8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx62_AMBA, (MmioRead8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx62_AMBA) | AOAC_PWR_ON_DEV));
    }
    if ((MmioRead8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx56_UART0) & 0x03) != 0x03) {
      MmioWrite8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx56_UART0, (MmioRead8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx56_UART0) | AOAC_PWR_ON_DEV));
    }
    // Switch to UART0_CTS_L, UART0_RTS_L & UART0_TXD
    MmioWrite8 (FCH_IOMUXx87_UART0_CTS_L_UART2_RXD_EGPIO135, 0);
    MmioWrite8 (FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 0);
    MmioWrite8 (FCH_IOMUXx8A_UART0_TXD_EGPIO138, 0);
  } else if (PcdGet8 (PcdFchUartPort) == 1) {
    if ((MmioRead8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx62_AMBA) & 0x03) != 0x03) {
      MmioWrite8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx62_AMBA, (MmioRead8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx62_AMBA) | AOAC_PWR_ON_DEV));
    }
    if ((MmioRead8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx58_UART1) & 0x03) != 0x03) {
      MmioWrite8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx58_UART1, (MmioRead8 (FCH_AOACx40_D3_CONTROL + FCH_AOACx58_UART1) | AOAC_PWR_ON_DEV));
    }
    // Switch to UART1_CTS_L, UART1_RTS_L & UART1_TXD
    MmioWrite8 (UART1_CTS_L_UART3_TXD_EGPIO140, 0);
    MmioWrite8 (FCH_IOMUXx8E_UART1_RTS_L_EGPIO142, 0);
    MmioWrite8 (FCH_IOMUXx8F_UART1_TXD_EGPIO143, 0);
  } else {
    return RETURN_DEVICE_ERROR;
  }

  while (MmioRead32 ((UINTN)PcdGet64 (PcdSerialRegisterBase)) == 0xFFFFFFFF);

  return RETURN_SUCCESS;
}

