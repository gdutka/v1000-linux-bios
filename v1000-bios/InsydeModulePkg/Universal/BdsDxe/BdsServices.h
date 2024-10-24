/** @file
  Header file for H2O BDS services

;******************************************************************************
;* Copyright (c) 2014 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  Head file for BDS Architectural Protocol implementation

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _H2O_BDS_SERVICES_H_
#define _H2O_BDS_SERVICES_H_

#include <Protocol/BootOptionPolicy.h>


/**
  Initialize gH2OBdsServicesProtocolGuid protocol data and install gH2OBdsServicesProtocolGuid protocol.

  @retval EFI_SUCCESS           Install gH2OBdsServicesProtocolGuid protocol successfully.
  @return Others                Any error occurred while installing gH2OBdsServicesProtocolGuid protocol.
**/
EFI_STATUS
InstallH2OBdsServicesProtocol (
  VOID
  );

/**
  Return the current value of the OsIndications and OsIndicationsSupported UEFI variable.

  @param[in]  This                    A Pointer to current instance of this protocol.
  @param[out] OsIndications           A pointer to contain the value of the OsIndications.
  @param[out] OsIndicationsSupported  A pointer to contain the value of the OsIndicationsSupported.

  @retval EFI_SUCCESS            Get OsIndications and OsIndicationsSupported successfully.
  @retval EFI_INVALID_PARAMETER  OsIndications or OsIndicationsSupported is NULL.
  @retval EFI_NOT_FOUND          Cannot find OsIndications or OsIndicationsSupported value.
**/
EFI_STATUS
EFIAPI
BdsServicesGetOsIndications (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  UINT64                      *OsIndications,
  OUT  UINT64                      *OsIndicationsSupported
  );

/**
  Return the current value of the OS loader timeout.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] Timeout            A pointer to contain the value of the timeout.

  @retval EFI_SUCCESS            Get Timeout value successfully.
  @retval EFI_INVALID_PARAMETER  Timeout is NULL.
  @retval EFI_NOT_FOUND          Cannot find Timeout value.
*/
EFI_STATUS
EFIAPI
BdsServicesGetTimeout (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  UINT16                      *Timeout
  );

/**
  Return the current boot mode, such as S4 resume, diagnostics, full configuration, etc.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BootMode           A pointer to contain the value of the boot mode.

  @retval EFI_SUCCESS            Get BootMode successfully.
  @retval EFI_INVALID_PARAMETER  BootMode is NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetBootMode (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  EFI_BOOT_MODE               *BootMode
  );

/**
  Return the current boot type, uch as legacy, UEFI or dual.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BootType           A pointer to contain the value of the boot mode.

  @retval EFI_SUCCESS            Get BootType successfully.
  @retval EFI_INVALID_PARAMETER  BootType is NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetBootType (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  UINT8                       *BootType
  );


/**
  Return linked list of BDS boot options derived from the BootOrder and Boot#### UEFI variables.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] BootList           Pointer to linked list of BDS boot options derived from the BootOrder and Boot#### UEFI variables.

  @retval EFI_SUCCESS            Get boot list successfully.
**/
EFI_STATUS
EFIAPI
BdsServicesGetBootList (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  LIST_ENTRY                  **BootList
  );

/**
  Return linked list of BDS boot options derived from the DriverOrder and Driver#### UEFI variables.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] DriverList         Pointer to linked list of BDS boot options derived from the DriverOrder and Driver#### UEFI variables.

  @retval EFI_SUCCESS            Get driver list successfully.
**/
EFI_STATUS
EFIAPI
BdsServicesGetDriverList (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  LIST_ENTRY                  **DriverList
  );

/**
  Create a BDS load option in a buffer allocated from pool.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  DriverOrBoot       Boolean that specifies whether the Boot Manager has connected this device
                                 path (TRUE) or not (FALSE).
  @param[in]  OptionName         A Null-terminated string that is the name of the vendor's variable.
  @param[in]  OptionGuid         A unique identifier for the vendor.
  @param[in]  Attributes         The attributes for this load option entry.
  @param[in]  DevicePath         A Pointer to a packed array of UEFI device paths.
  @param[in]  Description        The user readable description for the load option.
  @param[in]  OptionalData       A Pointer to optional data for load option.
  @param[in]  OptionalDataSize   The size by bytes of optional data.
  @param[out] LoadOption         Dynamically allocated memory that contains a new created H2O_BDS_LOAD_OPTION
                                 instance. Caller is responsible freeing the buffer

  @retval EFI_SUCCESS            Create load option successfully.
  @retval EFI_INVALID_PARAMETER  DevicePath is NULL or LoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  The OptionName is correct boot#### or driver#### variable name.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory to create H2O_BDS_LOAD_OPTION failed.
**/
EFI_STATUS
EFIAPI
BdsServicesCreateLoadOption (
  IN        H2O_BDS_SERVICES_PROTOCOL    *This,
  IN        BOOLEAN                      DriverOrBoot,
  IN CONST  CHAR16                       *OptionName OPTIONAL,
  IN CONST  EFI_GUID                     *OptionGuid OPTIONAL,
  IN        UINT32                       Attributes,
  IN CONST  EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CONST  CHAR16                       *Description OPTIONAL,
  IN CONST  UINT8                        *OptionalData OPTIONAL,
  IN        UINT32                       OptionalDataSize,
  OUT       H2O_BDS_LOAD_OPTION          **LoadOption
  );

/**
  Free the memory associated with a BDS load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  LoadOption         The allocated H2O_BDS_LOAD_OPTION instance to free.

  @retval EFI_SUCCESS            Get BootType successfully.
  @retval EFI_INVALID_PARAMETER  LoadOption is NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesFreeLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *LoadOption
  );


/**
  Converts a UEFI variable formatted as a UEFI load option to a BDS load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  VariableName       A Null-terminated string that is the name of the vendor's variable.
  @param[in]  VariableGuid       A unique identifier for the vendor.
  @param[out] BdsLoadOption      Dynamically allocated memory that contains a new created H2O_BDS_LOAD_OPTION
                                 instance. Caller is responsible freeing the buffer

  @retval EFI_SUCCESS            Convert BDS load option to UEFI load option successfully.
  @retval EFI_INVALID_PARAMETER  VariableName is NULL VariableGuid is NULL or BdsLoadOption is NULL.
  @retval EFI_NOT_FOUND          Variable doesn't exist.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory to create H2O_BDS_LOAD_OPTION failed.
**/
EFI_STATUS
EFIAPI
BdsServicesConvertVarToLoadOption (
  IN         H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   CONST CHAR16                      *VariableName,
  IN   CONST EFI_GUID                    *VariableGuid,
  OUT        H2O_BDS_LOAD_OPTION         **BdsLoadOption
  );

/**
  Converts a BDS load option in a UEFI variable formatted as a UEFI load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.
  @param[out] VariableName       A Null-terminated string that is the name of the vendor's variable.
                                 Caller is responsible freeing the buffer.
  @Param[out] VariableGuid       A unique identifier for the vendor.

  @retval EFI_SUCCESS            Convert UEFI load option to BDS load option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL, VariableName is NULL or VariableGuid is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption format is incorrect.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory to contain variable name is failed.
  @retval Others                 Any other error occurred in this function.
**/
EFI_STATUS
EFIAPI
BdsServicesConvertLoadOptionToVar (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption,
  OUT  CHAR16                      **VariableName,
  OUT  EFI_GUID                    *VariableGuid
  );

/**
  Insert a BDS load option into either the Driver or Boot order.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.

  @retval EFI_SUCCESS            Insert a BDS load option into either the Driver or Boot order successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption format is incorrect.
**/
EFI_STATUS
EFIAPI
BdsServicesInsertLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption
  );

/**
  Remove a BDS load option from either the Driver or Boot order.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  DriverOrBoot       Boolean that specifies whether this load option represents a Driver load option
                                 (FALSE) or Boot load option (TRUE).
  @param[in]  LoadOrder          Unsigned integer that specifies the current boot option being booted. Corresponds to the four
                                 hexadecimal digits in the #### portion of the UEFI variable name Boot#### or Driver####.

  @retval EFI_SUCCESS            Remove a BDS load option from either the Driver or Boot order successfully.
**/
EFI_STATUS
EFIAPI
BdsServicesRemoveLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   BOOLEAN                     DriverOrBoot,
  IN   UINT16                      LoadOrder
  );

/**
  Expand a partial load option to one or more fully qualified load options using the rules specified in the
  UEFI specification (USB WWID, hard disk) and the InsydeH2O boot groups.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.

  @retval EFI_SUCCESS            Expand load option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption format is incorrect.
**/
EFI_STATUS
EFIAPI
BdsServicesExpandLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption
  );

/**
  Launch a BDS load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.
  @param[out] ExitDataSize       Pointer to the size, in bytes, of ExitData.
  @param[out] ExitData           Pointer to a pointer to a data buffer that includes a Null-terminated
                                 string, optionally followed by additional binary data.

  @retval EFI_SUCCESS            Boot from the input boot option successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption format is incorrect.
  @retval EFI_NOT_FOUND          If the Device Path is not found in the system
**/
EFI_STATUS
EFIAPI
BdsServicesLaunchLoadOption (
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption,
  OUT  UINTN                       *ExitDataSize,
  OUT  CHAR16                      **ExitData      OPTIONAL
  );

/**
  Return linked list of BDS system preparation options derived from the SysPrepOrder and SysPrep#### UEFI variables.

  @param[in] This                A pointer to the protocol interface structure for this protocol.
  @param[in] SysPrepList         A pointer to a pointer to the first list entry in a linked list. Each list entry
                                 describes a single system preparation load option using a "H2O_BDS_LOAD_OPTION" structure.
                                 This structure can be retrieved from the list entry using the "BDS_OPTION_FROM_LINK()" macro.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMATER  This is set to NULL or SysPrepList is set to NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetSysPrepList (
  IN  H2O_BDS_SERVICES_PROTOCOL    *This,
  OUT LIST_ENTRY                   **SysPrepList
  );

/**
  Return linked list of BDS OS recovery load options derived from the OsRecoveryOrder and OsRecovery#### UEFI variables.

  @param[in] This                A pointer to the protocol interface structure for this protocol.
  @param[in] OsRecoveryList      A pointer to a pointer to the first list entry in a linked list. Each list entry
                                 describes a single OS recovery load option using a "H2O_BDS_LOAD_OPTION" structure.
                                 This structure can be retrieved from the list entry using the "BDS_OPTION_FROM_LINK()" macro.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMATER  This is set to NULL or OsRecoveryList is set to NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetOsRecoveryList (
  IN  H2O_BDS_SERVICES_PROTOCOL    *This,
  OUT LIST_ENTRY                   **OsRecoveryList
  );

/**
  Return linked list of BDS platform recovery load options derived from the PlatformRecovery#### UEFI variables.

  @param[in] This                A pointer to the protocol interface structure for this protocol.
  @param[in] OsRecoveryList      A pointer to a pointer to the first list entry in a linked list. Each list entry
                                 describes a single platform recovery load option using a "H2O_BDS_LOAD_OPTION" structure.
                                 This structure can be retrieved from the list entry using the "BDS_OPTION_FROM_LINK()" macro.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_INVALID_PARAMATER  This is set to NULL or PlatformRecoveryList is set to NULL.
**/
EFI_STATUS
EFIAPI
BdsServicesGetPlatformRecoveryList (
  IN  H2O_BDS_SERVICES_PROTOCOL    *This,
  OUT LIST_ENTRY                   **PlatformRecoveryList
  );

/**
  Create a BDS load option in a buffer allocated from pool.

  These functions allocation and initialize a BDS load option structure. They do not create a UEFI variable nor do
  they add the load option to one of the BDS load option lists.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  OptionType         Enumerated value that specifies the type of load option. Valid values are:
                                 H2O_BDS_LOAD_OPTION_TYPE_DRIVER (0)
                                 H2O_BDS_LOAD_OPTION_TYPE_BOOT (1)
                                 H2O_BDS_LOAD_OPTION_TYPE_SYSPREP (2)
                                 H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY (3)
                                 H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY (4)
  @param[in]  OptionName         A Null-terminated string that is the name of the vendor's variable.
  @param[in]  OptionGuid         A unique identifier for the vendor.
  @param[in]  Attributes         The attributes for this load option entry.
  @param[in]  DevicePath         A Pointer to a packed array of UEFI device paths.
  @param[in]  Description        The user readable description for the load option.
  @param[in]  OptionalData       A Pointer to optional data for load option.
  @param[in]  OptionalDataSize   The size by bytes of optional data.

  @retval EFI_SUCCESS            Create load option successfully.
  @retval EFI_INVALID_PARAMETER  DevicePath is NULL or LoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  The OptionName is correct boot#### or driver#### variable name.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory to create H2O_BDS_LOAD_OPTION failed.
**/
EFI_STATUS
EFIAPI
BdsServicesCreateLoadOption2 (
  IN        H2O_BDS_SERVICES_PROTOCOL    *This,
  IN        UINT8                        OptionType,
  IN CONST  CHAR16                       *OptionName OPTIONAL,
  IN CONST  EFI_GUID                     *OptionGuid OPTIONAL,
  IN        UINT32                       Attributes,
  IN CONST  EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN CONST  CHAR16                       *Description OPTIONAL,
  IN CONST  UINT8                        *OptionalData OPTIONAL,
  IN        UINT32                       OptionalDataSize,
  OUT       H2O_BDS_LOAD_OPTION          **LoadOption
  );

/**
  Insert a BDS load option into the Driver, Boot, SysPrep, OS Recovery or Platform Recovery orders.

  This function inserts the specified load option into one of the BDS load option lists, depending on the value of
  the OptionType member of the load option.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  BdsLoadOption      A pointer to BDS load option.
  @param[in]  BdsLoadOptionAfter An optional pointer to the load option which must precede the load option in the load
                                 option list. If NULL, then the load option will be inserted at the beginning of the
                                 load option list.

  @retval EFI_SUCCESS            Insert a BDS load option into either the Driver or Boot order successfully.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption is NULL.
  @retval EFI_INVALID_PARAMETER  BdsLoadOptionAfter is non-NULL, but the OptionType member of the load option does not
                                 match that of BdsLoadOption.
  @retval EFI_INVALID_PARAMETER  BdsLoadOption or BdsLoadOptionAfter format is incorrect.
**/
EFI_STATUS
EFIAPI
BdsServicesInsertLoadOption2 (
  IN H2O_BDS_SERVICES_PROTOCOL         *This,
  IN H2O_BDS_LOAD_OPTION               *BdsLoadOption,
  IN H2O_BDS_LOAD_OPTION               *BdsLoadOptionAfter OPTIONAL
  );

/**
  Remove a BDS load option from either the Driver or Boot order.

  This function removes the load option associated with the value LoadOrder from the load option list specified by OptionType.
  If OptionType is H2O_LOAD_OPTION_TYPE_OS_RECOVERY or H2O_LOAD_OPTION_TYPE_PLATFORM_RECOVERY, the load option list is
  further qualified by the OptionVendorGuid.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  OptionType         Enumerated value that specifies the type of load option to remove. Valid values are:
                                 H2O_BDS_LOAD_OPTION_TYPE_DRIVER (0)
                                 H2O_BDS_LOAD_OPTION_TYPE_BOOT (1)
                                 H2O_BDS_LOAD_OPTION_TYPE_SYSPREP (2)
                                 H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY (3)
                                 H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY (4)
  @param[in]  LoadOrder          Unsigned integer that specifies the current boot option being booted. Corresponds to the four
                                 hexadecimal digits in the #### portion of the UEFI variable name Boot#### or Driver####.
  @param[in]  OptionVendorGuid   Pointer to an optional GUID that specifies the group of OS Recovery or Platform Recovery
                                 load options to search for the specified load option. For Driver, Boot or SysPrep load
                                 options, this must be NULL.

  @retval EFI_SUCCESS            Remove a BDS load option from either the Driver or Boot order successfully.
**/
EFI_STATUS
EFIAPI
BdsServicesRemoveLoadOption2 (
  IN   H2O_BDS_SERVICES_PROTOCOL       *This,
  IN   UINT8                           OptionType,
  IN   UINT16                          LoadOrder,
  IN   CONST EFI_GUID                  *OptionVendorGuid OPTIONAL
  );

#endif
