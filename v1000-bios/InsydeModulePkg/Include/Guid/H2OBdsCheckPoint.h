/** @file
  Definitions for BDS check point

;******************************************************************************
;* Copyright (c) 2014 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __H2O_BDS_CHECK_POINT_H__
#define __H2O_BDS_CHECK_POINT_H__

#include <Guid/H2OCp.h>

//
// Below H2O_BDS_XXX_YYY_ZZZ_PROTOCOL definitions are deprecated. Suggest using H2O_BDS_XXX_YYY_ZZZ_DATA not Below
// H2O_BDS_XXX_YYY_ZZZ_PROTOCOL
//
typedef H2O_BDS_CP_INIT_DATA                              H2O_BDS_CP_INIT_PROTOCOL;
typedef H2O_BDS_CP_END_OF_DXE_BEFORE_DATA                 H2O_BDS_CP_END_OF_DXE_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_END_OF_DXE_AFTER_DATA                  H2O_BDS_CP_END_OF_DXE_AFTER_PROTOCOL;
typedef H2O_BDS_CP_DRIVER_CONNECT_BEFORE_DATA             H2O_BDS_CP_DRIVER_CONNECT_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_DRIVER_CONNECT_AFTER_DATA              H2O_BDS_CP_DRIVER_CONNECT_AFTER_PROTOCOL;
typedef H2O_BDS_CP_CON_IN_CONNECT_BEFORE_DATA             H2O_BDS_CP_CON_IN_CONNECT_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_CON_IN_CONNECT_AFTER_DATA              H2O_BDS_CP_CON_IN_CONNECT_AFTER_PROTOCOL;
typedef H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_DATA            H2O_BDS_CP_CON_OUT_CONNECT_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_CON_OUT_CONNECT_AFTER_DATA             H2O_BDS_CP_CON_OUT_CONNECT_AFTER_PROTOCOL;
typedef H2O_BDS_CP_DISPLAY_BEFORE_DATA                    H2O_BDS_CP_DISPLAY_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_DISPLAY_STRING_BEFORE_DATA             H2O_BDS_CP_DISPLAY_STRING_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_CONNECT_ALL_AFTER_DATA                 H2O_BDS_CP_CONNECT_ALL_AFTER_PROTOCOL;
typedef H2O_BDS_CP_CONNECT_ALL_BEFORE_DATA                H2O_BDS_CP_CONNECT_ALL_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_OS_RECOVERY_DATA                       H2O_BDS_CP_OS_RECOVERY_PROTOCOL;
typedef H2O_BDS_CP_PLATFORM_RECOVERY_DATA                 H2O_BDS_CP_PLATFORM_RECOVERY_PROTOCOL;
typedef H2O_BDS_CP_SYSPREP_DATA                           H2O_BDS_CP_SYSPREP_PROTOCOL;
typedef H2O_BDS_CP_BOOT_DEVICE_SELECT_DATA                H2O_BDS_CP_BOOT_DEVICE_SELECT_PROTOCOL;
typedef H2O_BDS_CP_NO_BOOT_DEVICE_DATA                    H2O_BDS_CP_NO_BOOT_DEVICE_PROTOCOL;
typedef H2O_BDS_CP_READY_TO_BOOT_BEFORE_DATA              H2O_BDS_CP_READY_TO_BOOT_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_READY_TO_BOOT_AFTER_DATA               H2O_BDS_CP_READY_TO_BOOT_AFTER_PROTOCOL;
typedef H2O_BDS_CP_BOOT_BEFORE_DATA                       H2O_BDS_CP_BOOT_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_BOOT_AFTER_DATA                        H2O_BDS_CP_BOOT_AFTER_PROTOCOL;
typedef H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_DATA         H2O_BDS_CP_EXIT_BOOT_SERVICES_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_LEGACY_BOOT_BEFORE_DATA                H2O_BDS_CP_LEGACY_BOOT_BEFORE_PROTOCOL;
typedef H2O_BDS_CP_LEGACY_BOOT_AFTER_DATA                 H2O_BDS_CP_LEGACY_BOOT_AFTER_PROTOCOL;
typedef H2O_BDS_CP_BOOT_SUCCESS_DATA                      H2O_BDS_CP_BOOT_SUCCESS_PROTOCOL;
typedef H2O_BDS_CP_BOOT_FAILED_DATA                       H2O_BDS_CP_BOOT_FAILED_PROTOCOL;

//
// Below PCD are deprecated. Suggest using gH2OBdsCpxxxyyyzzzGuid not Below
// gH2OBdsCpxxxyyyzzzProtocolGuid
//
extern EFI_GUID gH2OBdsCpInitProtocolGuid;
extern EFI_GUID gH2OBdsCpEndOfDxeBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpEndOfDxeAfterProtocolGuid;
extern EFI_GUID gH2OBdsCpDriverConnectBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpDriverConnectAfterProtocolGuid;
extern EFI_GUID gH2OBdsCpConInConnectBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpConInConnectAfterProtocolGuid;
extern EFI_GUID gH2OBdsCpConOutConnectBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpConOutConnectAfterProtocolGuid;
extern EFI_GUID gH2OBdsCpDisplayBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpDisplayStringBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpConnectAllBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpConnectAllAfterProtocolGuid;
extern EFI_GUID gH2OBdsCpOsRecoveryProtocolGuid;
extern EFI_GUID gH2OBdsCpPlatformRecoveryProtocolGuid;
extern EFI_GUID gH2OBdsCpSysPrepProtocolGuid;
extern EFI_GUID gH2OBdsCpBootDeviceSelectProtocolGuid;
extern EFI_GUID gH2OBdsCpNoBootDeviceProtocolGuid;
extern EFI_GUID gH2OBdsCpReadyToBootBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpReadyToBootAfterProtocolGuid;
extern EFI_GUID gH2OBdsCpBootBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpBootAfterProtocolGuid;
extern EFI_GUID gH2OBdsCpExitBootServicesBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpLegacyBootBeforeProtocolGuid;
extern EFI_GUID gH2OBdsCpLegacyBootAfterProtocolGuid;
extern EFI_GUID gH2OBdsCpBootSuccessProtocolGuid;
extern EFI_GUID gH2OBdsCpBootFailedProtocolGuid;

#endif
