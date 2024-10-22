/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _MP2_I2C_LIB_H_
#define _MP2_I2C_LIB_H_

EFI_STATUS
Mp2I2cRead (
 IN UINT8  BusId,
 IN UINT8  SlaveAddress,
 IN UINTN  ClockSpeed,
 IN UINT32 Length,
 IN UINT8  *Buffer
 );
 
EFI_STATUS
Mp2I2cWrite (
 IN UINT8  BusId,
 IN UINT8  SlaveAddress,
 IN UINTN  ClockSpeed,
 IN UINT32 Length,
 IN UINT8  *Buffer
 ); 
 
EFI_STATUS
Mp2SetBusFrequency (
  IN UINT8  BusId,
  IN UINTN  ClockSpeed
  );

#endif
