/** @file
  This protocol is produced by the Boot Manager during the BDS phase, prior to
  launching any hot key handlers or boot loaders. It is designed to give access
  to the current working state of the Boot Manager, such as the current working
  boot order, the boot mode, and the screen condition. It also allows certain
  behaviors to be triggered, such as boot-fail or boot-success.

;******************************************************************************
;* Copyright (c) 2014 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __H2O_BDS_SERVICES_PROTOCOL_H__
#define __H2O_BDS_SERVICES_PROTOCOL_H__

#include <Protocol/H2OBdsHotKey.h>
#include <Protocol/GraphicsOutput.h>

#define H2O_BDS_SERVICES_PROTOCOL_GUID \
  { \
    0x8bfff2c1, 0xbb8, 0x47fd, {0x9f, 0x6a, 0xae, 0xc1, 0x1, 0xac, 0xbe, 0x6e} \
  }

typedef struct _H2O_BDS_SERVICES_PROTOCOL  H2O_BDS_SERVICES_PROTOCOL;


#define DRIVER_OPTION          FALSE
#define BOOT_OPTION            TRUE

#define BOOT_PREVIOUS_LOAD_ORDER  0xFFFD
#define BOOT_CURRENT_LOAD_ORDER   0xFFFE
#define BOOT_NEXT_LOAD_ORDER      0xFFFF

#define H2O_BDS_BOOT_DISPLAY_MODE_NORMAL 0x00
#define H2O_BDS_BOOT_DISPLAY_MODE_QUIET  0x01
#define H2O_BDS_BOOT_DISPLAY_MODE_ALL    0xff

#define H2O_BDS_DISPLAY_UNIT_CHAR    0
#define H2O_BDS_DISPLAY_UNIT_PIXEL   1
#define H2O_BDS_DISPLAY_UNIT_PERCENT 2

#define H2O_BDS_DISPLAY_ORIGIN_X_DEFAULT_POSN 0x00000000
#define H2O_BDS_DISPLAY_ORIGIN_X_LEFT         0x01000000
#define H2O_BDS_DISPLAY_ORIGIN_X_CENTER       0x02000000
#define H2O_BDS_DISPLAY_ORIGIN_X_RIGHT        0x03000000
#define H2O_BDS_DISPLAY_ORIGIN_X_DEFAULT      0x04000000
#define H2O_BDS_DISPLAY_ORIGIN_X_LEFT_PERCENT(x) \
          (H2O_BDS_DISPLAY_ORIGIN_X_LEFT+(x))
#define H2O_BDS_DISPLAY_ORIGIN_X_RIGHT_PERCENT(x) \
          (H2O_BDS_DISPLAY_ORIGIN_X_RIGHT+(x))

#define H2O_BDS_DISPLAY_ORIGIN_Y_DEFAULT_POSN 0x00000000
#define H2O_BDS_DISPLAY_ORIGIN_Y_TOP          0x01000000
#define H2O_BDS_DISPLAY_ORIGIN_Y_CENTER       0x02000000
#define H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM       0x03000000
#define H2O_BDS_DISPLAY_ORIGIN_Y_DEFAULT      0x04000000
#define H2O_BDS_DISPLAY_ORIGIN_Y_TOP_PERCENT(x) \
          (H2O_BDS_DISPLAY_ORIGIN_Y_TOP+(x))
#define H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM_PERCENT(x) \
          (H2O_BDS_DISPLAY_ORIGIN_Y_BOTTOM+(x))

#define H2O_BOOT_PREVIOUS_VARIABLE_NAME                L"BootPrev"

//
// H2O_BDS_LOAD_OPTION related definitions
//
typedef enum {
  H2O_BDS_LOAD_OPTION_TYPE_DRIVER            = 0,
  H2O_BDS_LOAD_OPTION_TYPE_BOOT              = 1,
  H2O_BDS_LOAD_OPTION_TYPE_SYSPREP           = 2,
  H2O_BDS_LOAD_OPTION_TYPE_OS_RECOVERY       = 3,
  H2O_BDS_LOAD_OPTION_TYPE_PLATFORM_RECOVERY = 4,
  H2O_BDS_LOAD_OPTION_TYPE_MAX
} H2O_BDS_LOAD_OPTION_TYPE;

#define H2O_BDS_LOAD_OPTION_SIGNATURE SIGNATURE_32 ('H', 'B', 'L', 'O')

typedef struct _H2O_BDS_LOAD_OPTION {
  UINT32                    Signature;              ///< Particular statement signature. This signature must be 'H', 'B', 'L', 'O'
  LIST_ENTRY                Link;                   ///< Linked entry to the next and previous option in the list.
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;            ///< Pointer to a device path. The device path may be multi-instance.
  BOOLEAN                   Connected;              ///< Boolean that specifies whether the Boot Manager has connected this device path (TRUE) or not (FALSE).
  BOOLEAN                   DriverOrBoot;           ///< Boolean that specifies whether this load option represents a Driver load option (FALSE) or Boot load option (TRUE).
  UINT8                     OptionType;             ///< Enumerated value that specifies the type of load option.
  UINT8                     Reserved;
  UINT16                    LoadOrder;              ///< Unsigned integer that specifies the current boot option being booted. Corresponds to the four hexadecimal digits in
                                                    ///< the #### portion of the UEFI variable name Boot#### or Driver####. For BootNext, this will be initialized 0xFFFF.
  CHAR16                    LoadOrderVarName[21];   ///< Null-terminated string that contains the Boot#### or Driver#### variable name for this boot option (or BootNext).
  EFI_GUID                  LoadOrderVarGuid;       ///< A unique identifier for the load order variable.
  BOOLEAN                   Expanded;               ///< Boolean that specifies whether the Boot Manager has tried to expand this device path (TRUE) or not (FALSE).
  LIST_ENTRY                ExpandedLoadOptions;    ///< If Expanded is TRUE, then this linked list specifies zero or more H2O_BDS_LOAD_OPTION structures that represent the
                                                    ///< expanded boot options. Each of the boot options in this list will refer to a single device.
  UINT32                    Attributes;             ///< Bitmask that specifies load option attributes. Values are specified in section 3.1.3 of the UEFI specification.
  CHAR16                    *Description;           ///< Pointer to a null-terminated string associated with the load option.
  VOID                      *LoadOptionalData;      ///< Pointer to optional data associated with the load option.
  UINT32                    LoadOptionalDataSize;   ///< Unsigned integer that specifies the number of bytes pointed to by LoadOptionalData.
  CHAR16                    *StatusString;          ///< Pointer to null-terminated user-readable string that indicates the status of an attempt to launch this boot option or
                                                    ///< NULL if launch has not been tried.
} H2O_BDS_LOAD_OPTION;

#define BDS_OPTION_FROM_EXPAND_LINK(a)  CR (a, H2O_BDS_LOAD_OPTION, ExpandedLoadOptions, H2O_BDS_LOAD_OPTION_SIGNATURE)
#define BDS_OPTION_FROM_LINK(a)         CR (a, H2O_BDS_LOAD_OPTION, Link, H2O_BDS_LOAD_OPTION_SIGNATURE)
#define BDS_OPTION_HAVE_LOAD_ORDER_VAR_NAME(LoadOption)  ((BOOLEAN) (*(((H2O_BDS_LOAD_OPTION *) LoadOption)->LoadOrderVarName) != 0))

/**
  Return the current value of the OsIndications and OsIndicationsSupported UEFI variable.

  @param[in]  This                    A Pointer to current instance of this protocol.
  @param[out] OsIndications           A pointer to contain the value of the OsIndications.
  @param[out] OsIndicationsSupported  A pointer to contain the value of the OsIndicationsSupported.

  @retval EFI_SUCCESS            Get OsIndications and OsIndicationsSupported successfully.
  @retval EFI_INVALID_PARAMETER  OsIndications or OsIndicationsSupported is NULL.
  @retval EFI_NOT_FOUND          Cannot find OsIndications or OsIndicationsSupported value.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_OS_INDICATIONS)(
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
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_TIMEOUT)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_BOOT_MODE)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_BOOT_TYPE)(
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  UINT8                       *BootType
  );

/**
  Return linked list of BDS boot options derived from the BootOrder and Boot#### UEFI variables.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] BootList           Pointer to linked list of BDS boot options derived from the BootOrder and Boot#### UEFI variables.

  @retval EFI_SUCCESS            Get boot list successfully.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_BOOT_LIST)(
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  LIST_ENTRY                  **BootList
  );

/**
  Return linked list of BDS boot options derived from the DriverOrder and Driver#### UEFI variables.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] DriverList         Pointer to linked list of BDS boot options derived from the DriverOrder and Driver#### UEFI variables.

  @retval EFI_SUCCESS            Get driver list successfully.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_DRIVER_LIST)(
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  LIST_ENTRY                  **DriverList
  );

/**
  Create a BDS load option in a buffer allocated from pool.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[in]  DriverOrBoot       Boolean that specifies whether this load option represents a Driver load option
                                 (FALSE) or Boot load option (TRUE).
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_CREATE_LOAD_OPTION)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_FREE_LOAD_OPTION)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_VAR_TO_LOAD_OPTION)(
  IN        H2O_BDS_SERVICES_PROTOCOL   *This,
  IN  CONST CHAR16                      *VariableName,
  IN  CONST EFI_GUID                    *VariableGuid,
  OUT       H2O_BDS_LOAD_OPTION         **BdsLoadOption
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_LOAD_OPTION_TO_VAR)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_INSERT_LOAD_OPTION)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_REMOVE_LOAD_OPTION)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_EXPAND_LOAD_OPTION)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_LAUNCH_LOAD_OPTION)(
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption,
  OUT  UINTN                       *ExitDataSize,
  OUT  CHAR16                      **ExitData OPTIONAL
  );

/**
  Enable or disable BDS detection of hot keys.
  While enabled, the Boot Manager will continue to check each key pressed against the list of registered hot keys.
  When disabled, any hot key chord detect is suspended and any in-process hot key chords wil be discarded.

  @param[in]  This               A Pointer to current instance of this protocol.
  @param[out] Enable             On entry, a pointer to a Boolean that specifies whether hot key processing should be
                                 enabled (TRUE) or disabled (FALSE). On exit, a pointer to a Boolean that indicates
                                 whether hot key processing was enabled (TRUE) or disabled (FALSE) before this call.

  @retval EFI_SUCCESS            This function completed successfully.
  @retval EFI_UNSUPPORTED        Hot keys are not supported.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_ENABLE_HOT_KEYS)(
  IN     H2O_BDS_SERVICES_PROTOCOL *This,
  IN OUT BOOLEAN                   *Enable
  );

/**
  Register a function to be called when a specified hot key chord is detected.

  @param[in]  This               A pointer to the current instance of this protocol.
  @param[in]  HotKey             A pointer to a null-terminated ASCII string that specifies one or more hot key chords
                                 for this hot key.
  @param[in]  HotKeyFn           Pointer to the function that will be called when the hot key chord specified by
                                 HotKey is detected.
  @param[in]  Id                 Unsigned integer associated with this specific hot key.
  @param[in]  Options            Optional pointer to a null-terminated ASCII string that contains additional operating
                                 parameters.
  @param[out] HotKeyHandle       On return, a pointer to a handle that uniquely specifies this hot key.

  @retval EFI_SUCCESS            Hot key registered.
  @retval EFI_INVALID_PARAMETER  The hot key string was NULL, or had an invalid format.
                                 The hot key attributes had bits set that are unsupported.
  @retval EFI_ALREADY_STARTED    A hot key with one of the specified hot key chords has already been registered.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_REGISTER_CALLBACK_HOT_KEY)(
  IN  H2O_BDS_SERVICES_PROTOCOL    *This,
  IN  CONST CHAR8                  *HotKey,
  IN  H2O_BDS_HOT_KEY_LAUNCH       HotKeyFn,
  IN  UINT32                       Id,
  IN  CONST CHAR8                  *Options OPTIONAL,
  OUT EFI_HANDLE                   *HotKeyHandle
  );

/**
  Register a load option to invoke when a specified hot key chord is detected.

  @param[in]  This               A pointer to the current instance of this protocol.
  @param[in]  HotKey             A pointer to a null-terminated ASCII string that specifies one or more hot key chords
                                 for this hot key.
  @param[in]  HotKeyLoadOption   A pointer to a load option that specifies the application to load when the hot key is
                                 pressed, along with the load options to pass to that application.
  @param[out] HotKeyHandle       On return, a pointer to a handle that uniquely specifies this hot key.

  @retval EFI_SUCCESS            Hot key registered.
  @retval EFI_INVALID_PARAMETER  The hot key string was NULL, or had an invalid format.
                                 The hot key attributes had bits set that are unsupported.
  @retval EFI_ALREADY_STARTED    A hot key with one of the specified hot key chords has already been registered.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_REGISTER_LOAD_OPTION_HOT_KEY)(
  IN  H2O_BDS_SERVICES_PROTOCOL    *This,
  IN  CONST CHAR8                  *HotKey,
  IN  CONST H2O_BDS_LOAD_OPTION    *HotKeyLoadOption,
  OUT EFI_HANDLE                   *HotKeyHandle
  );

/**
  Unregister a previously registered hot key chord.

  @param[in] This                A pointer to the current instance of this protocol.
  @param[in] HotKeyHandle        Handle that specifies a previously registered hot key.

  @retval EFI_SUCCESS            Function completed successfully.
  @retval EFI_INVALID_PARAMETER  Hot key handle is invalid.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_UNREGISTER_HOT_KEY)(
  IN H2O_BDS_SERVICES_PROTOCOL     *This,
  IN EFI_HANDLE                    HotKeyHandle
  );

/**
  Get the current boot display mode, either normal or quiet.

  @param[in]  This               A pointer to the current instance of this protocol.
  @param[out] BootDisplayMode    On exit, returns whether the boot display is in text mode or graphics mode.

  @retval EFI_SUCCESS            Function completed successfully.
  @retval EFI_INVALID_PARAMETER  This or BootDisplayMode is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_BOOT_DISPLAY_MODE)(
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  OUT  UINT8                       *BootDisplayMode
  );

/**
  Change the hot key description for a specific hot key.

  @param[in] This                A pointer to the current instance of this protocol.
  @param[in] HotKeyHandle        Handle of a previously registered hot key.
  @param[in] BootDisplayMode     Enumerated value that specifies the boot display mode for which these descriptions are
                                 valid.
  @param[in] BeforeDescText      Pointer to an optional null-terminated string that specifies the text that will be displayed
                                 before the hot key has been detected. A NULL specifies that nothing will be displayed.
  @param[in] AfterDescText       Pointer to an optional null-terminated string that specifies the text that will be displayed
                                 after the hot key has been detected. A NULL specifies that nothing will be displayed.

  @retval EFI_SUCCESS            This function has completed successfully.
  @retval EFI_INVALID_PARAMETER  HotKeyHandle specifies an invalid handle or BootDisplayMode specifies an invalid boot
                                 display mode for this platform.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_SET_HOT_KEY_DESC_TEXT)(
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   EFI_HANDLE                  HotKeyHandle,
  IN   UINT8                       BootDisplayMode,
  IN   CONST CHAR16                *BeforeDescText OPTIONAL,
  IN   CONST CHAR16                *AfterDescText OPTIONAL
  );

/**
  Change the hot key position for a specific hot key.

  @param[in] This                A pointer to the current instance of this protocol.
  @param[in] HotKeyHandle        Handle of a previously registered hot key.
  @param[in] BootDisplayMode     Enumerated value that specifies the boot display mode for which these descriptions are
                                 valid.
  @param[in] X                   Signed integer value that specifies the X offset, relative to the X origin, in unites
                                 specified by XUnit.
  @param[in] XUnit               Enumerated value that specifies the unit of measurement associated with X.
  @param[in] XOrigin             Enumerated value that specifies the horizontal origin location.
  @param[in] Y                   Signed integer value that specifies the Y offset, relative to the Y origin, in unites
                                 specified by YUnit.
  @param[in] YUnit               Enumerated value that specifies the unit of measurement associated with Y.
  @param[in] YOrigin             Enumerated value that specifies the vertical origin location.

  @retval EFI_SUCCESS            This function has completed successfully.
  @retval EFI_INVALID_PARAMETER  HotKeyHandle specifies an invalid handle or BootDisplayMode specifies an invalid boot
                                 display mode for this platform.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_SET_HOT_KEY_DESC_POSN)(
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   EFI_HANDLE                  HotKeyHandle,
  IN   UINT8                       BootDisplayMode,
  IN   INT32                       X,
  IN   UINT32                      XUnit,
  IN   UINT32                      *XOrigin OPTIONAL,
  IN   INT32                       Y,
  IN   UINT32                      YUnit,
  IN   UINT32                      *YOrigin OPTIONAL
  );

/**
  Change the hot key color for a specific hot key description.

  @param[in] This                A pointer to the current instance of this protocol.
  @param[in] HotKeyHandle        Handle of a previously registered hot key.
  @param[in] BootDisplayMode     Enumerated value that specifies the boot display mode for which these descriptions are
                                 valid.
  @param[in] ForegroundColor     The color of the "on" pixels in the font characters.
  @param[in] BackgroundColor     The color of the "off" pixels in the font characters.

  @retval EFI_SUCCESS            This function has completed successfully.
  @retval EFI_INVALID_PARAMETER  HotKeyHandle specifies an invalid handle.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_SET_HOT_KEY_DESC_COLOR)(
  IN   H2O_BDS_SERVICES_PROTOCOL     *This,
  IN   EFI_HANDLE                    HotKeyHandle,
  IN   UINT8                         BootDisplayMode,
  IN   EFI_GRAPHICS_OUTPUT_BLT_PIXEL ForegroundColor,
  IN   EFI_GRAPHICS_OUTPUT_BLT_PIXEL BackgroundColor
  );

/**
  Change the hot key font for a specific hot key description.

  @param[in] This                A pointer to the current instance of this protocol.
  @param[in] HotKeyHandle        Handle of a previously registered hot key.
  @param[in] BootDisplayMode     Enumerated value that specifies the boot display mode for which these descriptions are
                                 valid.
  @param[in] FontName            Pointer to optional null-terminated ASCII string that specifies the name of the font to
                                 use for the display of these strings. A NULL specifies that the default font name,
                                 font size and style will be used.
  @param[in] FontSize            Unsigned integer that specifies the height of the font to use for the display of these
                                 strings. If FontName is NULL, then this is ignored.
  @param[in] FontStyle           Bitmask that specifies the style of the font to use for the display of these strings.
                                 If FontName is NULL, then this is ignored

  @retval EFI_SUCCESS            This function has completed successfully.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_SET_HOT_KEY_DESC_FONT)(
  IN   H2O_BDS_SERVICES_PROTOCOL     *This,
  IN   EFI_HANDLE                    HotKeyHandle,
  IN   UINT8                         BootDisplayMode,
  IN   CONST CHAR8                   *FontName OPTIONAL,
  IN   UINT16                        FontSize,
  IN   EFI_HII_FONT_STYLE            FontStyle
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_SYSPREP_LIST)(
  IN  H2O_BDS_SERVICES_PROTOCOL        *This,
  OUT LIST_ENTRY                       **SysPrepList
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_OS_RECOVERY_LIST)(
  IN  H2O_BDS_SERVICES_PROTOCOL        *This,
  OUT LIST_ENTRY                       **OsRecoveryList
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_GET_PLATFORM_RECOVERY_LIST)(
  IN  H2O_BDS_SERVICES_PROTOCOL        *This,
  OUT LIST_ENTRY                       **PlatformRecoveryList
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_CREATE_LOAD_OPTION2)(
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_INSERT_LOAD_OPTION2)(
  IN   H2O_BDS_SERVICES_PROTOCOL   *This,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOption,
  IN   H2O_BDS_LOAD_OPTION         *BdsLoadOptionAfter OPTIONAL
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
typedef
EFI_STATUS
(EFIAPI *H2O_BDS_SERVICES_REMOVE_LOAD_OPTION2)(
  IN   H2O_BDS_SERVICES_PROTOCOL         *This,
  IN   UINT8                             OptionType,
  IN   UINT16                            LoadOrder,
  IN   CONST EFI_GUID                    *OptionVendorGuid OPTIONAL
  );

struct _H2O_BDS_SERVICES_PROTOCOL {
  UINT32                                         Size;
  H2O_BDS_SERVICES_GET_OS_INDICATIONS            GetOsIndications;
  H2O_BDS_SERVICES_GET_TIMEOUT                   GetTimeout;
  H2O_BDS_SERVICES_GET_BOOT_MODE                 GetBootMode;
  H2O_BDS_SERVICES_GET_BOOT_TYPE                 GetBootType;
  H2O_BDS_LOAD_OPTION                            *BootCurrentLoadOption;
  CONST H2O_BDS_LOAD_OPTION                      *BootNextLoadOption;
  H2O_BDS_LOAD_OPTION                            *BootPrevLoadOption;
  H2O_BDS_SERVICES_GET_BOOT_LIST                 GetBootList;
  H2O_BDS_SERVICES_GET_DRIVER_LIST               GetDriverList;
  H2O_BDS_SERVICES_CREATE_LOAD_OPTION            CreateLoadOption;
  H2O_BDS_SERVICES_FREE_LOAD_OPTION              FreeLoadOption;
  H2O_BDS_SERVICES_VAR_TO_LOAD_OPTION            ConvertVarToLoadOption;
  H2O_BDS_SERVICES_LOAD_OPTION_TO_VAR            ConvertLoadOptionToVar;
  H2O_BDS_SERVICES_INSERT_LOAD_OPTION            InsertLoadOption;
  H2O_BDS_SERVICES_REMOVE_LOAD_OPTION            RemoveLoadOption;
  H2O_BDS_SERVICES_EXPAND_LOAD_OPTION            ExpandLoadOption;
  H2O_BDS_SERVICES_LAUNCH_LOAD_OPTION            LaunchLoadOption;
  H2O_BDS_SERVICES_ENABLE_HOT_KEYS               EnableHotKeys;
  H2O_BDS_SERVICES_REGISTER_CALLBACK_HOT_KEY     RegisterCallbackHotKey;
  H2O_BDS_SERVICES_REGISTER_LOAD_OPTION_HOT_KEY  RegisterLoadOptionHotKey;
  H2O_BDS_SERVICES_UNREGISTER_HOT_KEY            UnregisterHotKey;
  H2O_BDS_SERVICES_GET_BOOT_DISPLAY_MODE         GetBootDisplayMode;
  H2O_BDS_SERVICES_SET_HOT_KEY_DESC_TEXT         SetHotKeyDescText;
  H2O_BDS_SERVICES_SET_HOT_KEY_DESC_POSN         SetHotKeyDescPosn;
  H2O_BDS_SERVICES_SET_HOT_KEY_DESC_COLOR        SetHotKeyDescColor;
  H2O_BDS_SERVICES_SET_HOT_KEY_DESC_FONT         SetHotKeyDescFont;
  H2O_BDS_SERVICES_GET_SYSPREP_LIST              GetSysPrepList;
  H2O_BDS_SERVICES_GET_OS_RECOVERY_LIST          GetOsRecoveryList;
  H2O_BDS_SERVICES_GET_PLATFORM_RECOVERY_LIST    GetPlatformRecoveryList;
  H2O_BDS_SERVICES_CREATE_LOAD_OPTION2           CreateLoadOption2;
  H2O_BDS_SERVICES_INSERT_LOAD_OPTION2           InsertLoadOption2;
  H2O_BDS_SERVICES_REMOVE_LOAD_OPTION2           RemoveLoadOption2;
};

extern EFI_GUID gH2OBdsServicesProtocolGuid;

#endif
