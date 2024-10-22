/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _NBIO_HOTPLUG_DESC_H_
#define _NBIO_HOTPLUG_DESC_H_

// Current PPI revision
#define AMD_NBIO_HOTPLUG_DESC_VERSION   0x01

///
/// Forward declaration for the NBIO_GET_HOTPLUG_DESC_STRUCT
///
typedef struct _NBIO_HOTPLUG_DESC_PROTOCOL NBIO_HOTPLUG_DESC_PROTOCOL;

///
/// Protocol definition NBIO_HOTPLUG_DESC_PROTOCOL
///
typedef struct _NBIO_HOTPLUG_DESC_PROTOCOL {
  UINT32                Revision;           ///< Protocol revision
  HOTPLUG_DESCRIPTOR    *NbioHotplugDesc;   ///<
} NBIO_HOTPLUG_DESC_PROTOCOL;

extern EFI_GUID gAmdHotplugDescProtocolGuid;

#endif //

