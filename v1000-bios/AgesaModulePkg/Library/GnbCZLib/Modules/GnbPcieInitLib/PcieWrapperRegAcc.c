/* $NoKeywords:$ */
/**
 * @file
 *
 * Supporting services to access PCIe wrapper/core/PIF/PHY indirect register spaces
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#include  "Gnb.h"
#include  "GnbPcie.h"
#include  "GnbPcieFamServices.h"
#include  "GnbCommonLib.h"
#include  "GnbPcieConfig.h"
#include  "GnbPcieInitLib.h"

#define FILECODE LIBRARY_GNBCZLIB_MODULES_GNBPCIEINITLIB_PCIEWRAPPERREGACC_FILECODE
/*----------------------------------------------------------------------------------------*/
/**
 * Read PCIe register value.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to Wrapper descriptor
 * @param[in]  Address     Register address
 * @param[in]  Pcie        Pointer to global PCIe configuration
 * @retval                 Register Value
 */
UINT32
PcieRegisterRead (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if ((Wrapper->Features.AccessEncoding == 1) && ((Address & 0xff0000) == 0x010000)) {
    Address = (Address & 0xffff) | 0x1400000 | ((Address >> 8) & 0xF0000);
  }
  return PcieSiliconRegisterRead (PcieConfigGetParentSilicon (Wrapper), Address, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read PCIe register value.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Silicon     Pointer to silicon descriptor
 * @param[in]  Address     Register address
 * @param[in]  Pcie        Pointer to global PCIe configuration
 * @retval                 Register Value
 */

UINT32
PcieSiliconRegisterRead (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32              Value;
  GnbLibPciWrite (Silicon->Address.AddressValue | 0xE0, AccessWidth32, &Address, GnbLibGetHeader (Pcie));
  GnbLibPciRead (Silicon->Address.AddressValue | 0xE4, AccessWidth32, &Value, GnbLibGetHeader (Pcie));
  return Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe register value.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]   Wrapper     Pointer to wrapper descriptor
 * @param[in]   Address     Register address
 * @param[in]   Value       New register value
 * @param[in]   S3Save      Save register for S3 (True/False)
 * @param[in]   Pcie        Pointer to global PCIe configuration
 */
VOID
PcieRegisterWrite (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if ((Wrapper->Features.AccessEncoding == 1) && ((Address & 0xff0000) == 0x010000)) {
    Address = (Address & 0xffff) | 0x1400000 | ((Address >> 8) & 0xF0000);
  }
  PcieSiliconRegisterWrite (
     PcieConfigGetParentSilicon (Wrapper),
     Address,
     Value,
     S3Save,
     Pcie
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe register value.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]   Silicon     Pointer to silicon descriptor
 * @param[in]   Address     Register address
 * @param[in]   Value       New register value
 * @param[in]   S3Save      Save register for S3 (True/False)
 * @param[in]   Pcie        Pointer to global PCIe configuration
 */
VOID
PcieSiliconRegisterWrite (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  IDS_HDT_CONSOLE (PCIE_HOSTREG_TRACE, "    *WR %s (%d:%d:%d):0x%08x = 0x%08x\n",
    PcieFmDebugGetHostRegAddressSpaceString (Silicon, (UINT16) (Address >> 16)),
    Silicon->Address.Address.Bus,
    Silicon->Address.Address.Device,
    Silicon->Address.Address.Function,
    Address,
    Value
    );
  GnbLibPciWrite (Silicon->Address.AddressValue | 0xE0, S3Save ? AccessS3SaveWidth32 : AccessWidth32, &Address, GnbLibGetHeader (Pcie));
  GnbLibPciWrite (Silicon->Address.AddressValue | 0xE4, S3Save ? AccessS3SaveWidth32 : AccessWidth32, &Value, GnbLibGetHeader (Pcie));
}
/*----------------------------------------------------------------------------------------*/
/**
 * Read PCIe register field.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper descriptor
 * @param[in]  Address     Register address
 * @param[in]  FieldOffset Field offset
 * @param[in]  FieldWidth  Field width
 * @param[in]  Pcie        Pointer to global PCIe configuration
 * @retval                 Register field value
 */

UINT32
PcieRegisterReadField (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Value;
  Value = PcieRegisterRead (Wrapper, Address, Pcie);
  Value = (Value >> FieldOffset) & (~(0xFFFFFFFF << FieldWidth));
  return  Value;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Write PCIe register field.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper descriptor
 * @param[in]  Address     Register address
 * @param[in]  FieldOffset Field offset
 * @param[in]  FieldWidth  Field width
 * @param[in]  Value       Value to write
 * @param[in]  S3Save      Save register for S3 (True/False)
 * @param[in]  Pcie        Pointer to global PCIe configuration
 */


VOID
PcieRegisterWriteField (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT8                 FieldOffset,
  IN      UINT8                 FieldWidth,
  IN      UINT32                Value,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  TempValue;
  UINT32  Mask;
  TempValue = PcieRegisterRead (Wrapper, Address, Pcie);
  Mask = (~(0xFFFFFFFF << FieldWidth));
  Value &= Mask;
  TempValue &= (~(Mask << FieldOffset));
  PcieRegisterWrite (Wrapper, Address, TempValue | (Value << FieldOffset), S3Save, Pcie);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write PCIe register.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper descriptor
 * @param[in]  Address     Register address
 * @param[in]  AndMask     Value & (~AndMask)
 * @param[in]  OrMask      Value | OrMask
 * @param[in]  S3Save      Save register for S3 (True/False)
 * @param[in]  Pcie        Pointer to global PCIe configuration
 */

VOID
PcieRegisterRMW (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  if ((Wrapper->Features.AccessEncoding == 1) && ((Address & 0xff0000) == 0x010000)) {
    Address = (Address & 0xffff) | 0x1400000 | ((Address >> 8) & 0xF0000);
  }
  PcieSiliconRegisterRMW (
    PcieConfigGetParentSilicon (Wrapper),
    Address,
    AndMask,
    OrMask,
    S3Save,
    Pcie
    );
}

/*----------------------------------------------------------------------------------------*/
/**
 * Read/Modify/Write PCIe register.
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Silicon     Pointer to silicon descriptor
 * @param[in]  Address     Register address
 * @param[in]  AndMask     Value & (~AndMask)
 * @param[in]  OrMask      Value | OrMask
 * @param[in]  S3Save      Save register for S3 (True/False)
 * @param[in]  Pcie        Pointer to global PCIe configuration
 */

VOID
PcieSiliconRegisterRMW (
  IN      PCIe_SILICON_CONFIG   *Silicon,
  IN      UINT32                Address,
  IN      UINT32                AndMask,
  IN      UINT32                OrMask,
  IN      BOOLEAN               S3Save,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  UINT32  Value;
  Value = PcieSiliconRegisterRead (Silicon, Address, Pcie);
  Value = (Value & (~AndMask)) | OrMask;
  PcieSiliconRegisterWrite (Silicon, Address, Value, S3Save, Pcie);
}
