/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CDIT Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_ACPI_CDIT_SERVICES_PROTOCOL_H_
#define _AMD_ACPI_CDIT_SERVICES_PROTOCOL_H_
#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                    T Y P E D E F S     A N D     S T R U C T U R E S
 *                            COMPONENT Locality Distance Information Table
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *----------------------------------------------------------------------------------------
 */

///
/// Forward declaration for the AMD_CCX_ACPI_CDIT_SERVICES_PROTOCOL.
///
typedef struct _AMD_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL AMD_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *AMD_CDIT_SERVICES_GET_DISTANCE_INFO) (
  IN       AMD_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL    *This,
  IN OUT   UINT8                                  *DomainCount,
  IN OUT   UINT8                                  *Distance
  );

///
/// When installed, the CDIT Services Protocol produces a collection of
/// services that return various information to generate CDIT
///
struct _AMD_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL {
  UINTN                                            Revision;               ///< Revision Number
  AMD_CDIT_SERVICES_GET_DISTANCE_INFO              GetCditDistanceInfo;    ///< CDIT - distance information
};

extern EFI_GUID gAmdFabricAcpiCditServicesProtocolGuid;

#pragma pack (pop)
#endif // _AMD_ACPI_CDIT_SERVICES_PROTOCOL_H_
