/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdSmmCommunication.h
 *
 * Contains definition for AmdSmmCommunication
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _AMD_PSP_SMM_COMMUNICATION_H_
#define _AMD_PSP_SMM_COMMUNICATION_H_

extern EFI_GUID gPspSmmCommHandleGuid;

#define PSP_SMM_COMM_ID_GET_BLOCK_SIZE       0x1     ///< ID for S3 script
#define PSP_SMM_COMM_ID_READ_FLASH           0x2     ///< ID for Read Flash
#define PSP_SMM_COMM_ID_WRITE_FALSH          0x3     ///< ID for Write Flash
#define PSP_SMM_COMM_ID_ERASE_FALSH          0x4     ///< ID for Erase Flash
#define PSP_SMM_COMM_ID_CLOSE_INTERFACE      0x5     ///< ID for close SMM communication interface
#define PSP_SMM_COMM_ID_RPMC_INCMC           0x6     ///< ID for increment the Monotonic counter for SPI
#define PSP_SMM_COMM_ID_RPMC_REQMC           0x7     ///< ID for request the Monotonic counter for SPI

#define AMD_SMM_COMMUNICATION_BUFFER_SIZE    (5 * 1024)

/// SMM communication common buffer
typedef struct _PSP_SMM_COMMUNICATION_CMN {
  UINT32                       id;         ///< ID of smm communication buffer
} PSP_SMM_COMMUNICATION_CMN;

#define PSP_SMM_COMM_GETBLKSIZE_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (PSP_SMM_COMM_GETBLKSIZE))
/// SMM communication common buffer
typedef struct _PSP_SMM_COMM_GETBLKSIZE {
  UINT32                       id;         ///< ID of smm communication buffer
  UINTN                        BlockSize;  ///< Block size of Flash Device
} PSP_SMM_COMM_GETBLKSIZE;

#define PSP_SMM_COMM_RWFLASH_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (PSP_SMM_COMM_RW_FLASH))

/// SMM communication common buffer
typedef struct _PSP_SMM_COMM_RW_FLASH {
  UINT32                       id;         ///< ID of smm communication buffer
  UINTN                        FlashAddress; ///< Flash device physical address
  UINTN                        NumBytes;     ///< Number of bytes to be access
  UINT8                        Buffer[1];    ///< Buffer start
} PSP_SMM_COMM_RW_FLASH;

#define PSP_SMM_COMM_ERASEFLASH_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (PSP_SMM_COMM_ERASEFLASH))

/// SMM communication common buffer
typedef struct _PSP_SMM_COMM_ERASEFLASH {
  UINT32                       id;         ///< ID of smm communication buffer
  UINTN                        FlashAddress; ///< Flash device physical address
  UINTN                        NumBytes;  ///< Number of bytes to be access
} PSP_SMM_COMM_ERASEFLASH;

#define PSP_CMM_COMM_CLOSE_INTERFACE_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (PSP_SMM_COMMUNICATION_CMN))


#define PSP_SMM_COMM_RPMC_INCMC_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (PSP_SMM_COMM_RPMC_INCMC))
/// SMM communication common buffer
typedef struct _PSP_SMM_COMM_RPMC_INCMC {
  UINT32                       id;         ///< ID of smm communication buffer
  UINT8   CounterAddr;                      ///< Input, CounterAddr for the "Increment Monotonic Counter" SPI command
  UINT32  CounterData;                      ///< Input, CounterData for the "Increment Monotonic Counter" SPI command
  UINT8   *Signature;                    ///< Input, Signature for the "Increment Monotonic Counter" SPI command
} PSP_SMM_COMM_RPMC_INCMC;

#define PSP_SMM_COMM_RPMC_REQMC_BUFFER_SIZE (OFFSET_OF (EFI_SMM_COMMUNICATE_HEADER, Data)  + sizeof (PSP_SMM_COMM_RPMC_REQMC))
/// SMM communication common buffer
typedef struct _PSP_SMM_COMM_RPMC_REQMC {
  UINT32                       id;         ///< ID of smm communication buffer
  UINT8   CounterAddr;                      ///< Input, CounterAddr for the "Request Monotonic Counter" SPI command
  UINT8   *Tag;                          ///< Input, Tag for the "Request Monotonic Counter" SPI command
  UINT8   *Signature;                    ///< Input, Signature for the "Request Monotonic Counter" SPI command
  UINT32  *CounterData;                  ///< Output, CounterData for the "Request Monotonic Counter" SPI command
  UINT8   *OutputSignature;              ///< Output, Returned Signature for the "Request Monotonic Counter" SPI command
} PSP_SMM_COMM_RPMC_REQMC;

#endif // _AMD_PSP_SMM_COMMUNICATION_H_

