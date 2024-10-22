/* $NoKeywords:$ */
/**
 * @file
 *
 * Install of build option: GNB
 *
 * Contains AMD AGESA install macros and test conditions. Output is the
 * defaults tables reflecting the User's build options selection.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Options
 * @e \$Revision: 329865 $   @e \$Date: 2015-10-29 09:59:04 +0800 (Thu, 29 Oct 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include "Gnb.h"
#include  "OptionGnb.h"
#include  "SocLogicalId.h"
  #include "GnbPcie.h"
  #include "GnbGfx.h"
  #include  "GnbPcieFamServices.h"
  #include  "GnbFamServices.h"
  #include  "GnbGfxFamServices.h"

//---------------------------------------------------------------------------------------------------
// Service installation
// Export CZ services
//---------------------------------------------------------------------------------------------------
  extern F_PCIEFMGETSBCONFIGINFO                  PcieGetSbConfigInfoCZ;
  extern F_PCIEFMGETCOMPLEXDATALENGTH             PcieGetComplexDataLengthCZ;
  extern F_PCIEFMBUILDCOMPLEXCONFIGURATION        PcieBuildComplexConfigurationCZ;
  extern F_PCIEFMCONFIGUREENGINESLANEALLOCATION   PcieConfigureEnginesLaneAllocationCZ;
  extern F_PCIEFMCHECKPORTPCIDEVICEMAPPING        PcieCheckPortPciDeviceMappingCZ;
  extern F_PCIEFMMAPPORTPCIADDRESS                PcieMapPortPciAddressCZ;
  extern F_PCIEFMCHECKPORTPCIELANECANBEMUXED      PcieCheckPortPcieLaneCanBeMuxedCZ;
  PCIe_FAM_CONFIG_SERVICES GnbPcieConfigProtocolCZ = {
    PcieGetComplexDataLengthCZ,
    PcieBuildComplexConfigurationCZ,
    PcieConfigureEnginesLaneAllocationCZ,
    PcieCheckPortPciDeviceMappingCZ,
    PcieMapPortPciAddressCZ,
    PcieCheckPortPcieLaneCanBeMuxedCZ,
    PcieGetSbConfigInfoCZ
  };
  GNB_SERVICE GnbPcieConfigServicesCZ = {
    GnbPcieFamConfigService,
    AMD_FAMILY_CZ,
    &GnbPcieConfigProtocolCZ,
    NULL
  };

  //-------------------------------------------------------
  extern F_PCIEFMGETCORECONFIGURATIONVALUE        PcieGetCoreConfigurationValueCZ;
  extern F_PCIEFMGETLINKSPEEDCAP                  PcieGetLinkSpeedCapCZ;
  extern F_PCIEFMGETNATIVEPHYLANEBITMAP           PcieGetNativePhyLaneBitmapCZ;
  extern F_PCIEFMSETLINKSPEEDCAP                  PcieSetLinkSpeedCapV4;
  PCIe_FAM_INIT_SERVICES GnbPcieInitProtocolCZ = {
    PcieGetCoreConfigurationValueCZ,
    PcieGetLinkSpeedCapCZ,
    PcieGetNativePhyLaneBitmapCZ,
    PcieSetLinkSpeedCapV4
  };
  GNB_SERVICE GnbPcieInitServicesCZ = {
    GnbPcieFamInitService,
    AMD_FAMILY_CZ,
    &GnbPcieInitProtocolCZ,
    &GnbPcieConfigServicesCZ
  };

  //-------------------------------------------------------
  //  #if IDSOPT_TRACING_ENABLED == TRUE
  extern F_PCIEFMDEBUGGETHOSTREGADDRESSSPACESTRING  PcieDebugGetHostRegAddressSpaceStringCZ;
  extern F_PCIEFMDEBUGGETWRAPPERNAMESTRING          PcieDebugGetWrapperNameStringCZ;
  extern F_PCIEFMDEBUGGETCORECONFIGURATIONSTRING    PcieDebugGetCoreConfigurationStringCZ;

  PCIe_FAM_DEBUG_SERVICES GnbPcieDebugProtocolCZ = {
    PcieDebugGetHostRegAddressSpaceStringCZ,
    PcieDebugGetWrapperNameStringCZ,
    PcieDebugGetCoreConfigurationStringCZ
  };

  GNB_SERVICE GnbPcieDebugServicesCZ = {
    GnbPcieFamDebugService,
    AMD_FAMILY_CZ,
    &GnbPcieDebugProtocolCZ,
    &GnbPcieInitServicesCZ
  };
  //  #endif

  //-------------------------------------------------------
  extern F_GNB_REGISTER_ACCESS  GnbRegisterReadCZ;
  extern F_GNB_REGISTER_ACCESS  GnbRegisterWriteCZ;
  GNB_REGISTER_SERVICE GnbRegisterAccessProtocolCZ = {
    GnbRegisterReadCZ,
    GnbRegisterWriteCZ
  };
  GNB_SERVICE GnbRegisterAccessServicesCZ = {
    GnbRegisterAccessService,
    AMD_FAMILY_CZ,
    &GnbRegisterAccessProtocolCZ,
    &GnbPcieDebugServicesCZ
  };

  //-------------------------------------------------------
  extern F_GNBFMCREATEIVRSENTRY    GnbCreateIvrsEntryCZ;
  extern F_GNBFMCHECKIOMMUPRESENT  GnbCheckIommuPresentCZ;

  GNB_FAM_IOMMU_SERVICES GnbIommuConfigProtocolCZ = {
    GnbCheckIommuPresentCZ,
    GnbCreateIvrsEntryCZ
  };

  GNB_SERVICE GnbIommuConfigServicesCZ = {
    GnbIommuService,
    AMD_FAMILY_CZ,
    &GnbIommuConfigProtocolCZ,
    &GnbRegisterAccessServicesCZ
  };
  //-------------------------------------------------------
  extern F_GFXMAPENGINETODISPLAYPATH GfxMapEngineToDisplayPathCZ;
  extern F_GFXDISABLECONTROLLER      GfxDisableControllerCZ;
  extern F_GFXCALCULATECLOCK         GfxCalculateClockCZ;
  extern F_GFXISVBIOSPOSTED          GfxIsVbiosPostedCZ;
  GFX_FAM_SERVICES GfxFamilyServiceProtocolCZ = {
    GfxMapEngineToDisplayPathCZ,
    GfxDisableControllerCZ,
    GfxCalculateClockCZ,
    GfxIsVbiosPostedCZ
  };
  GNB_SERVICE GfxFamilyServicesCZ = {
    GfxFamService,
    AMD_FAMILY_CZ,
    &GfxFamilyServiceProtocolCZ,
    &GnbIommuConfigServicesCZ
  };

  //-------------------------------------------------------
  extern F_GNBTIMESTAMP GnbTimeStampCZ;
  GNB_FAM_TS_SERVICES GnbFamTsProtocolCZ = {
    GnbTimeStampCZ,
  };
  GNB_SERVICE GnbFamTsServicesCZ = {
    GnbFamTsService,
    AMD_FAMILY_CZ,
    &GnbFamTsProtocolCZ,
    &GfxFamilyServicesCZ
  };

  //-------------------------------------------------------
  extern F_GNBFMALIBGETBASETABLE PcieAlibGetBaseTableCZM;
  GNB_FAM_ALIB_SERVICES AlibProtocolCZ = {
    PcieAlibGetBaseTableCZM
  };
  GNB_SERVICE AlibServicesCZ = {
    GnbFamAlibService,
    AMD_FAMILY_CZ,
    &AlibProtocolCZ,
    &GnbFamTsServicesCZ
  };

  //-------------------------------------------------------
  extern F_GNBFMALIBGETBUILDOPTIONS GnbGetBuildOptionsCZ;
  GNB_FAM_BUILDOPTIONS_POINTERS GnbOptionsProtocolCZ = {
    &GnbGetBuildOptionsCZ
  };
  GNB_SERVICE GnbOptionsServicesCZ = {
    GnbFamBuildOptionPointer,
    AMD_FAMILY_CZ,
    &GnbOptionsProtocolCZ,
    &AlibServicesCZ
  };
//  GNB_SERVICE   *ServiceTableCZ = &GnbOptionsServicesCZ;
  GNB_SERVICE   *ServiceTable = &GnbOptionsServicesCZ;

