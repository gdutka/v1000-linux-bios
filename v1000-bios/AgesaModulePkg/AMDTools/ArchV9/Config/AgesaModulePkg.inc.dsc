#;*****************************************************************************
#;
#; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
#;
#;******************************************************************************

[LibraryClasses.Common.PEIM]
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf                                         <ALL>
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapPeiLib/AmdHeapPeiLib.inf                                                    <ALL>
  FabricResourceSizeForEachRbLib|AgesaModulePkg/Library/PeiFabricResourceSizeForEachRbLib/PeiFabricResourceSizeForEachRbLib.inf   <ALL>
  FabricResourceReportToGcdLib|AgesaModulePkg/Library/FabricResourceReportToGcdNullLib/FabricResourceReportToGcdNullLib.inf    <ALL>
  CcxSetMmioCfgBaseLib|AgesaModulePkg/Library/CcxSetMmioCfgBaseLib/CcxSetMmioCfgBaseLib.inf                            <ALL>
  IdsLib|AgesaModulePkg/Library/IdsPeiLib/IdsPeiLib.inf                                                                <ALL>
  FchPeiLibV9|AgesaModulePkg/Library/FchPeiLib/FchPeiLib.inf                                                           <ALL>
  FchTaishanPeiLib|AgesaModulePkg/Fch/Taishan/FchTaishanCore/FchTaishanLibPei.inf                                      <ZP>
  FchHuashanPeiLib|AgesaModulePkg/Fch/Huashan/FchHuashanCore/FchHuashanLibPei.inf                                      <SSP|MTS>
  FchSandstonePeiLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibPei.inf                              <RV>
  FchKernPeiLib|AgesaModulePkg/Fch/Kern/FchKernCore/FchKernLibPei.inf                                                  <FAMILY15H>
  FchInitHookLibPei|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibPei.inf                                        <ALL>
  GnbHeapLib|AgesaModulePkg/Library/GnbHeapPeiLib/GnbHeapPeiLib.inf                                                    <ALL>
  AgesaGnbPeiLib|AgesaModulePkg/Library/GnbCZLib/GnbPeiLibCZ.inf                                                       <FAMILY15H>
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibPei/AmdPspCommonLibPei.inf                                     <ALL>
  ApobCommonServiceLib|AgesaModulePkg/Library/ApobCommonServiceLibPei/ApobCommonServiceLibPei.inf                      <ALL>
  PeiSocLogicalIdServicesLib|AgesaModulePkg/Library/PeiSocLogicalIdServicesLib/PeiSocLogicalIdServicesLib.inf          <ALL>
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Pei/AmdTableSmnPeiLib.inf                                          <ALL>
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibPei/AmdCapsuleLibPei.inf                                           <ALL>
  PeiSocBistLoggingLib|AgesaModulePkg/Library/PeiSocBistLoggingLib/PeiSocBistLoggingLib.inf                            <FAMILY17H>

  ## MEM Libs
  AmdMemBaseLib|AgesaModulePkg/Library/Mem/BaseLib/AmdMemBaseLib.inf                                                   <ALL>
  AmdMemArdkLib|AgesaModulePkg/Library/Mem/ArdkLib/AmdMemArdkLib.inf                                                   <FAMILY15H>
  AmdMemMainCzLib|AgesaModulePkg/Library/Mem/MainCzLib/AmdMemMainCzLib.inf                                             <FAMILY15H>
  AmdMemTechLib|AgesaModulePkg/Library/Mem/TechLib/AmdMemTechLib.inf                                                   <FAMILY15H>
  AmdMemTechDdr3Lib|AgesaModulePkg/Library/Mem/TechDdr3Lib/AmdMemTechDdr3Lib.inf                                       <FAMILY15H>
  AmdMemTechDdr4Lib|AgesaModulePkg/Library/Mem/TechDdr4Lib/AmdMemTechDdr4Lib.inf                                       <FAMILY15H>
  AmdMemPsCzLib|AgesaModulePkg/Library/Mem/PsCzLib/AmdMemPsCzLib.inf                                                   <FAMILY15H>
  AmdMemPsLib|AgesaModulePkg/Library/Mem/PsLib/AmdMemPsLib.inf                                                         <FAMILY15H>
  AmdMemPsCzAm4Lib|AgesaModulePkg/Library/Mem/PsCzAm4Lib/AmdMemPsCzAm4Lib.inf                                          <FAMILY15H>
  AmdMemPsCzFp4Lib|AgesaModulePkg/Library/Mem/PsCzFp4Lib/AmdMemPsCzFp4Lib.inf                                          <FAMILY15H>
  AmdMemFeatIdentifyDimmLib|AgesaModulePkg/Library/Mem/FeatIdentifyDimmLib/AmdMemFeatIdentifyDimmLib.inf               <FAMILY15H>
  AmdMemFeatDmiCollectLib|AgesaModulePkg/Library/Mem/FeatDmiCollectLib/AmdMemFeatDmiCollectLib.inf                     <FAMILY15H>
  AmdMemFeatMemClrLib|AgesaModulePkg/Library/Mem/FeatMemClrLib/AmdMemFeatMemClrLib.inf                                 <FAMILY15H>
  AmdMemFeatCratLib|AgesaModulePkg/Library/Mem/FeatCratLib/AmdMemFeatCratLib.inf                                       <FAMILY15H>
  AmdMemFeatTableLib|AgesaModulePkg/Library/Mem/FeatTableLib/AmdMemFeatTableLib.inf                                    <FAMILY15H>
  AmdMemFeatEccLib|AgesaModulePkg/Library/Mem/FeatEccLib/AmdMemFeatEccLib.inf                                          <FAMILY15H>
  AmdMemServicesCzLib|AgesaModulePkg/Library/Mem/ServicesCzLib/AmdMemServicesCzLib.inf                                 <FAMILY15H>
  AmdMemFeatUmaLib|AgesaModulePkg/Library/Mem/FeatUmaLib/AmdMemFeatUmaLib.inf                                          <FAMILY15H>
  AmdMemFeatOLSpareLib|AgesaModulePkg/Library/Mem/Null/FeatOLSpareLibNull/AmdMemFeatOLSpareLibNull.inf                 <FAMILY15H>
  AmdMemFeatCsIntLvLib|AgesaModulePkg/Library/Mem/FeatCsIntLvLib/AmdMemFeatCsIntLvLib.inf                              <FAMILY15H>
  AmdMemFeatNodeIntLvLib|AgesaModulePkg/Library/Mem/Null/FeatNodeIntLvLibNull/AmdMemFeatNodeIntLvLibNull.inf           <FAMILY15H>
  AmdMemFeatChIntLvLib|AgesaModulePkg/Library/Mem/FeatChIntLvLib/AmdMemFeatChIntLvLib.inf                              <FAMILY15H>
  AmdMemFeatIntLvRgnLib|AgesaModulePkg/Library/Mem/Null/FeatIntLvRgnLibNull/AmdMemFeatIntLvRgnLibNull.inf              <FAMILY15H>
  AmdMemFeatLvDdr3Lib|AgesaModulePkg/Library/Mem/FeatLvDdr3Lib/AmdMemFeatLvDdr3Lib.inf                                 <FAMILY15H>
  AmdMemFeatOnDimmThermalLib|AgesaModulePkg/Library/Mem/FeatOnDimmThermalLib/AmdMemFeatOnDimmThermalLib.inf            <FAMILY15H>
  AmdMemFeatExcludeDimmLib|AgesaModulePkg/Library/Mem/FeatExcludeDimmLib/AmdMemFeatExcludeDimmLib.inf                  <FAMILY15H>
  AmdMemFeatAmpLib|AgesaModulePkg/Library/Mem/Null/FeatAmpLibNull/AmdMemFeatAmpLibNull.inf                             <FAMILY15H>

  FchPromontoryPeiLib|AgesaModulePkg/Fch/Promontory/PromontoryLibrary/FchPromontoryPeiLib/FchPromontoryPeiLib.inf      <AM4|SP3TR|SP3CP>
  AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2RvLib/MemSmbiosV2Lib.inf                                         <RV>
  AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2ZpLib/MemSmbiosV2Lib.inf                                         <AM4SM|SP4SM>
  AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2ZpMcmLib/MemSmbiosV2Lib.inf                                      <SP3ZP|SP4ZP|SP3TR>
  AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2SspLib/MemSmbiosV2Lib.inf                                        <SSP|MTS>
  AmdCalloutLib|AgesaModulePkg/Library/AmdCalloutLib/AmdCalloutLib.inf                                                 <RV>


  ## IDS LIB
  AmdIdsHookLib|AgesaModulePkg/Library/AmdIdsHookLibPei/AmdIdsHookLib.inf                                              <ALL>
  CcxXvIdsHookLibPei|AgesaModulePkg/Library/CcxXvIdsHookLib/Pei/CcxXvIdsHookLibPei.inf                                 <FAMILY15H>
  FabricIdsHookUnbLibPei|AgesaModulePkg/Library/FabricIdsHookUnbLib/Pei/FabricIdsHookUnbLibPei.inf                     <FAMILY15H>
  MemIdsHookLib|AgesaModulePkg/Library/Mem/MemIdsHookCzLib/MemIdsHookCzLib.inf                                         <FAMILY15H>
  CcxZenZpIdsHookLibPei|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Pei/CcxZenZpIdsHookLibPei.inf                        <ZP>
  CcxZenRvIdsHookLibPei|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Pei/CcxZenRvIdsHookLibPei.inf                        <RV>
  CcxVhIdsHookLibPei|AgesaModulePkg/Library/CcxVhIdsHookLib/Pei/CcxVhIdsHookLibPei.inf                                 <SSP|MTS>
  FabricIdsHookZpLibPei|AgesaModulePkg/Library/FabricIdsHookZpLib/Pei/FabricIdsHookZpLibPei.inf                        <ZP>
  FabricIdsHookRvLibPei|AgesaModulePkg/Library/FabricIdsHookRvLib/Pei/FabricIdsHookRvLibPei.inf                        <RV>
  FabricIdsHookSspLibPei|AgesaModulePkg/Library/FabricIdsHookSspLib/Pei/FabricIdsHookSspLibPei.inf                     <SSP|MTS>
  NbioIdsHookSspLibPei|AgesaModulePkg/Library/NbioIdsHookSspLib/Pei/NbioIdsHookSspLibPei.inf                           <SSP|MTS|RN>

  ## FCH I2C Lib
  Mp2I2cLibPei|AgesaModulePkg/Fch/Sandstone/FchSandstoneI2cPei/Mp2I2cLibPei.inf                                        <FP5RV|FT5RV>

[LibraryClasses.Common.DXE_DRIVER]
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/S3Save/AmdS3SaveLib.inf                                             <ALL>
  IdsLib|AgesaModulePkg/Library/IdsDxeLib/IdsDxeLib.inf                                                                <ALL>
  FabricResourceSizeForEachRbLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachRbLib/DxeFabricResourceSizeForEachRbLib.inf   <ALL>
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapDxeLib/AmdHeapDxeLib.inf                                                    <ALL>
  GnbHeapLib|AgesaModulePkg/Library/GnbHeapDxeLib/GnbHeapDxeLib.inf                                                    <ALL>
  AgesaGnbDxeLib|AgesaModulePkg/Library/GnbCZLib/GnbDxeLibCZ.inf                                                       <FAMILY15H>
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibDxe/AmdPspCommonLibDxe.inf                                     <ALL>
  ApobCommonServiceLib|AgesaModulePkg/Library/ApobCommonServiceLibDxe/ApobCommonServiceLibDxe.inf                      <ALL>
  AmdPspFlashAccLib|AgesaModulePkg/Library/AmdPspFlashAccLibDxe/AmdPspFlashAccLibDxe.inf                               <ALL>
  AmdPspFlashUpdateLib|AgesaModulePkg/Library/AmdPspFlashUpdateLib/AmdPspFlashUpdateLib.inf                            <ALL>
  AmdPspRomArmorLib|AgesaModulePkg/Library/AmdPspRomArmorLibNull/AmdPspRomArmorLibNull.inf                             <ZP|SSP|GN|BA>
  FchPromontoryDxeLib|AgesaModulePkg/Fch/Promontory/PromontoryLibrary/FchPromontoryDxeLib/FchPromontoryDxeLib.inf      <AM4|SP3TR|SP3CP>
  ApcbZpLib|AgesaModulePkg/Library/ApcbZpLib/ApcbLib.inf                                                               <ZP>
  ApcbRvLib|AgesaModulePkg/Library/ApcbRvLib/ApcbLib.inf                                                               <RV>
#  ApcbSspLib|AgesaModulePkg/Library/ApcbSspLib/ApcbLib.inf                                                             <SSP|MTS>
  DxeSocLogicalIdServicesLib|AgesaModulePkg/Library/DxeSocLogicalIdServicesLib/DxeSocLogicalIdServicesLib.inf          <ALL>
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Dxe/AmdTableSmnDxeLib.inf                                          <ALL>
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibDxe/AmdCapsuleLibDxe.inf                                           <ALL>
  CcxMpServicesLib|AgesaModulePkg/Library/CcxMpServicesDxeLib/CcxMpServicesDxeLib.inf                                  <ALL>
  CcxSmmAccess2Lib|AgesaModulePkg/Library/DxeCcxSmmAccess2Lib/DxeCcxSmmAccess2Lib.inf                                  <ALL>

  ## MEM Lib
  AmdMemS3InitLib|AgesaModulePkg/Library/Mem/S3InitLib/S3InitLib.inf                                                   <FAMILY15H>
  AmdMemArdkLib|AgesaModulePkg/Library/Mem/Null/ArdkLibNull/AmdMemArdkLibNull.inf                                      <FAMILY15H>
  AmdMemMainCzLib|AgesaModulePkg/Library/Mem/Null/MainCzLibNull/AmdMemMainCzLibNull.inf                                <FAMILY15H>
  AmdMemPsLib|AgesaModulePkg/Library/Mem/Null/PsLibNull/AmdMemPsLibNull.inf                                            <FAMILY15H>
  AmdMemPsCzLib|AgesaModulePkg/Library/Mem/Null/PsCzLibNull/AmdMemPsCzLibNull.inf                                      <FAMILY15H>
  AmdMemPsCzAm4Lib|AgesaModulePkg/Library/Mem/Null/PsCzAm4LibNull/AmdMemPsCzAm4LibNull.inf                             <FAMILY15H>
  AmdMemPsCzFp4Lib|AgesaModulePkg/Library/Mem/Null/PsCzFp4LibNull/AmdMemPsCzFp4LibNull.inf                             <FAMILY15H>
  AmdMemFeatDmiCollectLib|AgesaModulePkg/Library/Mem/Null/FeatDmiCollectLibNull/AmdMemFeatDmiCollectLibNull.inf        <FAMILY15H>
  AmdMemFeatCratLib|AgesaModulePkg/Library/Mem/Null/FeatCratLibNull/AmdMemFeatCratLibNull.inf                          <FAMILY15H>
  AmdMemTechLib|AgesaModulePkg/Library/Mem/Null/TechLibNull/AmdMemTechLibNull.inf                                      <FAMILY15H>
  AmdMemTechDdr3Lib|AgesaModulePkg/Library/Mem/Null/TechDdr3LibNull/AmdMemTechDdr3LibNull.inf                          <FAMILY15H>
  AmdMemTechDdr4Lib|AgesaModulePkg/Library/Mem/Null/TechDdr4LibNull/AmdMemTechDdr4LibNull.inf                          <FAMILY15H>
  AmdMemFeatTableLib|AgesaModulePkg/Library/Mem/Null/FeatTableLibNull/AmdMemFeatTableLibNull.inf                       <FAMILY15H>
  AmdMemFeatIdentifyDimmLib|AgesaModulePkg/Library/Mem/Null/FeatIdentifyDimmLibNull/AmdMemFeatIdentifyDimmLibNull.inf  <FAMILY15H>
  AmdMemFeatMemClrLib|AgesaModulePkg/Library/Mem/Null/FeatMemClrLibNull/AmdMemFeatMemClrLibNull.inf                    <FAMILY15H>
  AmdMemFeatDmiConstructLib|AgesaModulePkg/Library/Mem/FeatDmiConstructLib/MemFeatDmiConstructLib.inf                  <FAMILY15H>
  ApcbLibV3|AgesaModulePkg/Library/ApcbLibV3/ApcbLibV3.inf                                                             <SSP|MTS>

  ## IDS LIB
  AmdIdsHookLib|AgesaModulePkg/Library/AmdIdsHookLibDxe/AmdIdsHookLib.inf                                              <ALL>
  CcxXvIdsHookLibDxe|AgesaModulePkg/Library/CcxXvIdsHookLib/Dxe/CcxXvIdsHookLibDxe.inf                                 <FAMILY15H>
  FabricIdsHookUnbLibDxe|AgesaModulePkg/Library/FabricIdsHookUnbLib/Dxe/FabricIdsHookUnbLibDxe.inf                     <FAMILY15H>
  CcxZenZpIdsHookLibDxe|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Dxe/CcxZenZpIdsHookLibDxe.inf                        <ZP>
  FabricIdsHookZpLibDxe|AgesaModulePkg/Library/FabricIdsHookZpLib/Dxe/FabricIdsHookZpLibDxe.inf                        <ZP>
  CcxZenRvIdsHookLibDxe|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Dxe/CcxZenRvIdsHookLibDxe.inf                        <RV>
  FabricIdsHookRvLibDxe|AgesaModulePkg/Library/FabricIdsHookRvLib/Dxe/FabricIdsHookRvLibDxe.inf                        <RV>
  CcxVhIdsHookLibDxe|AgesaModulePkg/Library/CcxVhIdsHookLib/Dxe/CcxVhIdsHookLibDxe.inf                                 <SSP|MTS>
  FabricIdsHookSspLibDxe|AgesaModulePkg/Library/FabricIdsHookSspLib/Dxe/FabricIdsHookSspLibDxe.inf                     <SSP|MTS>
  NbioIdsHookSspLibDxe|AgesaModulePkg/Library/NbioIdsHookSspLib/Dxe/NbioIdsHookSspLibDxe.inf                           <SSP|MTS>
  ## FCH SPI ACCESS lib
#  FchSpiAccessLib|AgesaModulePkg/Library/FchSpiAccessLib/FchSpiAccessRom3Lib.inf                                      <ALL>

[LibraryClasses.Common.DXE_SMM_DRIVER]
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapDxeLib/AmdHeapDxeLib.inf                                                    <ALL>
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/S3Save/AmdS3SaveLib.inf                                             <ALL>
  FabricResourceSizeForEachRbLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachRbLib/DxeFabricResourceSizeForEachRbLib.inf   <ALL>
#@todo add specifiic SMM Lib instance, DXE Lib is depend on gBS service exisitance
  IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf                                                        <ALL>
  AmdIdsHookLib|AgesaModulePkg/Library/AmdIdsHookLibDxe/AmdIdsHookLib.inf                                              <ALL>
  AmdPspCommonLib|AgesaModulePkg/Library/AmdPspCommonLibDxe/AmdPspCommonLibDxe.inf                                     <ALL>
  ApobCommonServiceLib|AgesaModulePkg/Library/ApobCommonServiceLibDxe/ApobCommonServiceLibDxe.inf                      <ALL>
  AmdPspFlashUpdateLib|AgesaModulePkg/Library/AmdPspFlashUpdateLib/AmdPspFlashUpdateLib.inf                            <ALL>
  AmdPspRomArmorLib|AgesaModulePkg/Library/AmdPspRomArmorLib/AmdPspRomArmorLib.inf                                     <ZP|SSP|GN|BA>
  CcxZenZpIdsHookLibSmm|AgesaModulePkg/Library/CcxZenZpIdsHookLib/Smm/CcxZenZpIdsHookLibSmm.inf                        <ZP>
  CcxZenRvIdsHookLibSmm|AgesaModulePkg/Library/CcxZenRvIdsHookLib/Smm/CcxZenRvIdsHookLibSmm.inf                        <RV>
  CcxVhIdsHookLibSmm|AgesaModulePkg/Library/CcxVhIdsHookLib/Smm/CcxVhIdsHookLibSmm.inf                                 <SSP|MTS>
  ApcbZpLib|AgesaModulePkg/Library/ApcbZpLib/ApcbLib.inf                                                               <ZP>
  ApcbRvLib|AgesaModulePkg/Library/ApcbRvLib/ApcbLib.inf                                                               <RV>
#  ApcbSspLib|AgesaModulePkg/Library/ApcbSspLib/ApcbLib.inf                                                             <SSP|MTS>
  AmdCapsuleLib|AgesaModulePkg/Library/AmdCapsuleLibDxe/AmdCapsuleLibDxe.inf                                           <ALL>
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Dxe/AmdTableSmnDxeLib.inf                                          <ALL>
  CcxMpServicesLib|AgesaModulePkg/Library/CcxMpServicesSmmLib/CcxMpServicesSmmLib.inf                                  <ALL>
  RasZpSmmLib|AgesaModulePkg/Library/RasZpSmmLib/RasZpSmmLib.inf                                                       <ZP>
  RasRvSmmLib|AgesaModulePkg/Library/RasRvSmmLib/RasRvSmmLib.inf                                                       <RV>
  RasSspSmmLib|AgesaModulePkg/Library/RasSspSmmLib/RasSspSmmLib.inf                                                    <SSP|MTS>
  ApcbLibV3|AgesaModulePkg/Library/ApcbLibV3/ApcbLibV3.inf                                                             <SSP|MTS>

  ## FCH SPI ACCESS lib
#  FchSpiAccessLib|AgesaModulePkg/Library/FchSpiAccessLib/FchSpiAccessRom3Lib.inf                                      <ALL>
[LibraryClasses.common.PEI_CORE]
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapPeiLib/AmdHeapPeiLib.inf                                                    <ALL>
  AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Pei/AmdTableSmnPeiLib.inf                                          <ALL>
  ## FCH SPI ACCESS lib
#  FchSpiAccessLib|AgesaModulePkg/Library/FchSpiAccessLib/FchSpiAccessSmnLib.inf                                       <ALL>

[LibraryClasses]
  #
  # Agesa specific common libraries
  #
  AmdBaseLib|AgesaModulePkg/Library/AmdBaseLib/AmdBaseLib.inf                                                          <ALL>
  FabricResourceSizeForEachRbLib|AgesaModulePkg/Library/DxeFabricResourceSizeForEachRbLib/DxeFabricResourceSizeForEachRbLib.inf   <ALL>
  FabricResourceReportToGcdLib|AgesaModulePkg/Library/FabricResourceReportToGcdLib/FabricResourceReportToGcdLib.inf    <ALL>
  AmdIdsDebugPrintLib|AgesaModulePkg/Library/AmdIdsDebugPrintLib/AmdIdsDebugPrintLib.inf                               <ALL>
  AmdIdsHookLib|AgesaModulePkg/Library/AmdIdsHookLibNull/AmdIdsHookLibNull.inf                                         <ALL>
  AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf                                    <ALL>
  IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf                                                        <ALL>
  IdsMiscLib|AgesaModulePkg/Library/IdsMiscLib/IdsMiscLib.inf                                                          <ALL>
  AmdHeapLib|AgesaModulePkg/Library/AmdHeapLibNull/AmdHeapLibNull.inf                                                  <ALL>

  AmdSocBaseLib|AgesaModulePkg/Library/AmdSocBaseLib/AmdSocBaseLib.inf                                                 <ALL>
  AmdErrorLogLib|AgesaModulePkg/Library/AmdErrorLogLib/AmdErrorLogLib.inf                                              <ALL>
  AmdTableLib|AgesaModulePkg/Library/AmdTableLib/AmdTableLib.inf                                                       <ALL>
  SocCmnIdsHookZpLibPei|AgesaModulePkg/Library/SocCmnIdsHookZpLib/Pei/SocCmnIdsHookZpLibPei.inf                        <ZP|FAMILY15H>
  SocCmnIdsHookZpLibDxe|AgesaModulePkg/Library/SocCmnIdsHookZpLib/Dxe/SocCmnIdsHookZpLibDxe.inf                        <ZP|FAMILY15H>
  SocCmnIdsHookRvLibPei|AgesaModulePkg/Library/SocCmnIdsHookRvLib/Pei/SocCmnIdsHookRvLibPei.inf                        <RV>
  SocCmnIdsHookRvLibDxe|AgesaModulePkg/Library/SocCmnIdsHookRvLib/Dxe/SocCmnIdsHookRvLibDxe.inf                        <RV>
  SocCmnIdsHookSspLibPei|AgesaModulePkg/Library/SocCmnIdsHookSspLib/Pei/SocCmnIdsHookSspLibPei.inf                     <SSP|MTS>
  SocCmnIdsHookSspLibDxe|AgesaModulePkg/Library/SocCmnIdsHookSspLib/Dxe/SocCmnIdsHookSspLibDxe.inf                     <SSP|MTS>
  BaseCoreLogicalIdLib|AgesaModulePkg/Library/BaseCoreLogicalIdX86Lib/BaseCoreLogicalIdX86Lib.inf                      <ALL>
  AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf                                         <ALL>

  ## PSP Libs
  AmdPspMboxLibV1|AgesaModulePkg/Library/AmdPspMboxLibV1/AmdPspMboxLibV1.inf                                           <ALL>
  AmdPspBaseLibV1|AgesaModulePkg/Library/AmdPspBaseLibV1/AmdPspBaseLibV1.inf                                           <ALL>
  AmdPspBaseLibV2|AgesaModulePkg/Library/AmdPspBaseLibV2/AmdPspBaseLibV2.inf                                           <ALL>
  AmdPspMboxLibV2|AgesaModulePkg/Library/AmdPspMboxLibV2/AmdPspMboxLibV2.inf                                           <ALL>
  AmdPspApobLib|AgesaModulePkg/Library/AmdPspApobLib/AmdPspApobLib.inf                                                 <ALL>
  AmdPspFtpmLib|AgesaModulePkg/Library/AmdPspFtpmLib/AmdPspFtpmLib.inf                                                 <AM4|RV|SP3TR|SP3CP|SP4SM>
  AmdPspFlashAccLib|AgesaModulePkg/Library/AmdPspFlashAccLibNull/AmdPspFlashAccLibNull.inf                             <ALL>
  PspIdsHookLibDxe|AgesaModulePkg/Library/PspIdsHookLib/Dxe/PspIdsHookLibDxe.inf                                       <ALL>
  ApobZpRvLib|AgesaModulePkg/Library/ApobZpRvLib/ApobZpRvLib.inf                                                       <ALL>
  ApobZpMcmLib|AgesaModulePkg/Library/ApobZpMcmLib/ApobZpMcmLib.inf                                                    <ALL>
  ApobSspLib|AgesaModulePkg/Library/ApobSspLib/ApobSspLib.inf                                                          <ALL>
  ApobDummyLib|AgesaModulePkg/Library/ApobDummyLib/ApobDummyLib.inf                                                    <ALL>

  ## CCX Lib
  CcxBaseX86Lib|AgesaModulePkg/Library/CcxBaseX86Lib/CcxBaseX86Lib.inf                                                 <ALL>
  CcxBistLib|AgesaModulePkg/Library/CcxBistLib/CcxBistLib.inf                                                          <ALL>
  CcxPspLib|AgesaModulePkg/Library/CcxPspLib/CcxPspLib.inf                                                             <ALL>
  CcxHaltLib|AgesaModulePkg/Library/CcxHaltLib/CcxHaltLib.inf                                                          <ALL>
  CcxMicrocodePatchLib|AgesaModulePkg/Library/CcxMicrocodePatchLib/CcxMicrocodePatchLib.inf                            <ALL>
  CcxRolesLib|AgesaModulePkg/Library/CcxRolesX86Lib/CcxRolesX86Lib.inf                                                 <ALL>
  CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesZenZpLib/CcxPstatesZenZpLib.inf                                       <ZP|RV>
  CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesVhLib/CcxPstatesVhLib.inf                                             <SSP|MTS>
  CcxStallLib|AgesaModulePkg/Library/CcxStallZenLib/CcxStallZenLib.inf                                                 <FAMILY17H>
  DxeCcxBaseX86ServicesLib|AgesaModulePkg/Library/DxeCcxBaseX86ServicesLib/DxeCcxBaseX86ServicesLib.inf                <ALL>

  ## DF Lib
  FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesZpLib/FabricPstatesZpLib.inf                                    <FAMILY17H>
  BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf                     <FAMILY15H|ZP|RV>
  BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf                   <SSP|MTS>
  PeiFabricTopologyServices2Lib|AgesaModulePkg/Library/PeiFabricTopologyServices2Lib/PeiFabricTopologyServices2Lib.inf <ALL>
  DxeFabricTopologyServices2Lib|AgesaModulePkg/Library/DxeFabricTopologyServices2Lib/DxeFabricTopologyServices2Lib.inf <ALL>
  SmmFabricTopologyServices2Lib|AgesaModulePkg/Library/SmmFabricTopologyServices2Lib/SmmFabricTopologyServices2Lib.inf <ALL>
  FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf                  <ZP|RV|FAMILY15H>
  FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf                      <SSP|MTS>

  FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf            <FAMILY15H|ZP|RV>
  FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf            <SSP|MTS>
  PeiFabricResourceManagerServicesLib|AgesaModulePkg/Library/PeiFabricResourceManagerServicesLib/PeiFabricResourceManagerServicesLib.inf    <ALL>
  DxeFabricResourceManagerServicesLib|AgesaModulePkg/Library/DxeFabricResourceManagerServicesLib/DxeFabricResourceManagerServicesLib.inf    <ALL>

  ## MEM Lib
  AmdMemBaseLib|AgesaModulePkg/Library/Mem/BaseLib/AmdMemBaseLib.inf                                                   <ALL>
  AmdMemMainLib|AgesaModulePkg/Library/Mem/MainLib/AmdMemMainLib.inf                                                   <FAMILY15H>
  AmdMemNbLib|AgesaModulePkg/Library/Mem/NbLib/AmdMemNbLib.inf                                                         <FAMILY15H>
  AmdMemNbCzLib|AgesaModulePkg/Library/Mem/NbCzLib/AmdMemNbCzLib.inf                                                   <FAMILY15H>
  AmdMemFeatS3Lib|AgesaModulePkg/Library/Mem/FeatS3Lib/AmdMemFeatS3Lib.inf                                             <FAMILY15H>
  AmdMemErrorLogLib|AgesaModulePkg/Library/Mem/ErrorLogLib/AmdMemErrorLogLib.inf                                       <FAMILY15H>
  HardCodedMemCommonServiceLib|AgesaModulePkg/Library/HardCodedMemCommonServiceLib/HardCodedMemCommonServiceLib.inf    <FAMILY15H>
  OptionMemoryInstallCzLib|AgesaModulePkg/Library/OptionMemoryInstallCzLib/OptionMemoryInstallCzLib.inf                <FAMILY15H>
  MemRestoreLib|AgesaModulePkg/Library/MemRestoreLib/MemRestoreLib.inf                                                 <FAMILY17H>

  ## Gnb Lib
  GnbCommonLib|AgesaModulePkg/Library/GnbCommonLib/GnbCommonLib.inf                                                    <ALL>
  GnbMemAccLib|AgesaModulePkg/Library/GnbMemAccLib/GnbMemAccLib.inf                                                    <ALL>
  GnbIoAccLib|AgesaModulePkg/Library/GnbIoAccLib/GnbIoAccLib.inf                                                       <ALL>
  GnbPciAccLib|AgesaModulePkg/Library/GnbPciAccLib/GnbPciAccLib.inf                                                    <ALL>
  GnbCpuAccLib|AgesaModulePkg/Library/GnbCpuAccLib/GnbCpuAccLib.inf                                                    <ALL>
  GnbPciLib|AgesaModulePkg/Library/GnbPciLib/GnbPciLib.inf                                                             <ALL>
  GnbLib|AgesaModulePkg/Library/GnbLib/GnbLib.inf                                                                      <ALL>
  NbioHandleLib|AgesaModulePkg/Library/NbioHandleLib/NbioHandleLib.inf                                                 <ALL>
  NbioIommuIvrsLib|AgesaModulePkg/Library/NbioIommuIvrsLib/NbioIommuIvrsLib.inf                                        <ALL>
  PcieConfigLib|AgesaModulePkg/Library/PcieConfigLib/PcieConfigLib.inf                                                 <ALL>
  GnbSSocketLib|AgesaModulePkg/Library/GnbSSocketLib/GnbSSocketLib.inf                                                 <ALL>
  GnbRegisterAccZpLib|AgesaModulePkg/Library/GnbRegisterAccZpLib/GnbRegisterAccZpLib.inf                               <ALL>
  NbioRegisterAccLib|AgesaModulePkg/Library/NbioRegisterAccLib/NbioRegisterAcc.inf                                     <ALL>
  NbioSmuV9Lib|AgesaModulePkg/Library/NbioSmuV9Lib/NbioSmuV9Lib.inf                                                    <ZP|RV|FAMILY15H>
  NbioSmuV10Lib|AgesaModulePkg/Library/NbioSmuV10Lib/NbioSmuV10Lib.inf                                                 <RV|RN>
  NbioSmuV11Lib|AgesaModulePkg/Library/NbioSmuV11Lib/NbioSmuV11Lib.inf                                                 <SSP|MTS>
  NbioSmuV12Lib|AgesaModulePkg/Library/NbioSmuV12Lib/NbioSmuV12Lib.inf                                                 <RN>
  NbioTableBlastLib|AgesaModulePkg/Library/NbioTableBlastLib/NbioTableBlastLib.inf                                     <SSP|MTS|RN>
  NbioIdsHookCZLib|AgesaModulePkg/Library/NbioIdsHookCZLib/NbioIdsHookCZLib.inf                                        <FAMILY15H>
  NbioIdsHookZPLib|AgesaModulePkg/Library/NbioIdsHookZPLib/NbioIdsHookZPLib.inf                                        <ZP>
  NbioIdsHookRVLib|AgesaModulePkg/Library/NbioIdsHookRVLib/NbioIdsHookRVLib.inf                                        <RV>
  DxioLibV1|AgesaModulePkg/Library/DxioLibV1/DxioLibV1.inf                                                             <ZP|RV>
  DxioLibV2|AgesaModulePkg/Library/DxioLibV2/DxioLibV2.inf                                                             <SSP|MTS|RN>
  PcieMiscCommLib|AgesaModulePkg/Library/PcieMiscCommLib/PcieMiscCommLib.inf                                           <FAMILY17H>
  NbioAzaliaLib|AgesaModulePkg/Library/NbioAzaliaLib/NbioAzaliaLib.inf                                                 <FAMILY17H>
  SmnAccessLib|AgesaModulePkg/Library/SmnAccessLib/SmnAccessLib.inf                                                    <FAMILY17H>
  NbioUtilLib|AgesaModulePkg/Library/NbioUtilLib/NbioUtilLib.inf                                                       <SSP|MTS>

  ## Fch Lib
  FchBaseLib|AgesaModulePkg/Library/FchBaseLib/FchBaseLib.inf                                                          <ALL>
  BaseResetSystemLib|AgesaModulePkg/Library/FchBaseResetSystemLib/FchBaseResetSystemLib.inf                            <ALL>
  FchDxeLibV9|AgesaModulePkg/Library/FchDxeLib/FchDxeLib.inf                                                           <ALL>
  FchSmmLibV9|AgesaModulePkg/Library/FchSmmLib/FchSmmLib.inf                                                           <ALL>
  FchTaishanDxeLib|AgesaModulePkg/Fch/Taishan/FchTaishanCore/FchTaishanLibDxe.inf                                      <ZP|FAMILY15H>
  FchHuashanDxeLib|AgesaModulePkg/Fch/Huashan/FchHuashanCore/FchHuashanLibDxe.inf                                      <SSP|MTS>
  FchSandstoneDxeLib|AgesaModulePkg/Fch/Sandstone/FchSandstoneCore/FchSandstoneLibDxe.inf                              <RV>
  FchKernDxeLib|AgesaModulePkg/Fch/Kern/FchKernCore/FchKernLibDxe.inf                                                  <FAMILY15H>
  FchInitHookLibDxe|AgesaModulePkg/Library/FchInitHookLib/FchInitHookLibDxe.inf                                        <ALL>
  FchIdsHookLib|AgesaModulePkg/Library/FchIdsHookLib/FchIdsHookLib.inf                                                 <ALL>
  Mp2I2cLibDxe|AgesaModulePkg/Fch/Sandstone/FchSandstoneI2cDxe/Mp2I2cLibDxe.inf                                        <FP5RV|FT5RV>
  ## FCH SPI ACCESS lib
  FchSpiAccessLib|AgesaModulePkg/Library/FchSpiAccessLib/FchSpiAccessRom2Lib.inf                                       <ALL>
  ## Ras Lib
  RasZpBaseLib|AgesaModulePkg/Library/RasZpBaseLib/RasZpBaseLib.inf                                                    <ZP>
  RasRvBaseLib|AgesaModulePkg/Library/RasRvBaseLib/RasRvBaseLib.inf                                                    <RV>
  RasSspBaseLib|AgesaModulePkg/Library/RasSspBaseLib/RasSspBaseLib.inf                                                 <SSP|MTS>
  RasBrBaseLib|AgesaModulePkg/Library/RasBrBaseLib/RasBrBaseLib.inf                                                    <FAMILY15H>

[Components.IA32]
  AgesaModulePkg/Debug/AmdIdsDebugPrintPei/AmdIdsDebugPrintPei.inf                                                     <ALL>

  ##PSP Drivers
  AgesaModulePkg/Psp/AmdPspPeiV1/AmdPspPeiV1.inf                                                                       <FAMILY15H>
  AgesaModulePkg/Psp/AmdPspPeiV2/AmdPspPeiV2.inf                                                                       <RV|ZP>
  AgesaModulePkg/Psp/AmdPspPeiV2Ssp/AmdPspPeiV2.inf                                                                    <SSP|MTS>
  AgesaModulePkg/Psp/AmdPspFtpmPei/AmdPspFtpmPei.inf                                                                   <AM4|RV|SP3TR|SP3CP|SP4SM>
  AgesaModulePkg/Psp/ApobDrv/ApobZpRvPei/ApobZpRvPei.inf                                                               <AM4RV|FP5RV|AM4SM|SP4SM>
  AgesaModulePkg/Psp/ApobDrv/ApobZpMcmPei/ApobZpMcmPei.inf                                                             <SP3ZP|SP4ZP|SP3TR>
  AgesaModulePkg/Psp/ApobDrv/ApobSspPei/ApobSspPei.inf                                                                 <SSP|MTS>
  AgesaModulePkg/Psp/ApobDrv/ApobDummyPei/ApobDummyPei.inf                                                             <FAMILY15H>

  AgesaModulePkg/Ccx/Zen/CcxZenZpPei/AmdCcxZenZpPei.inf {
    <LibraryClasses>
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Ccx/Zen/CcxZenRvPei/AmdCcxZenRvPei.inf {
    <LibraryClasses>
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesRvLib/CcxResetTablesRvLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesRvLib/CcxResetTablesRvLib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Ccx/Vh/Pei/AmdCcxVhPei.inf {
    <LibraryClasses>
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesVhLib/CcxResetTablesVhLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxRolesLib|AgesaModulePkg/Library/CcxRolesVhLib/CcxRolesVhLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
  }                                                                                                                    <SSP|MTS|RN>
  AgesaModulePkg/Ccx/Xv/CcxXvPei/AmdCcxXvPei.inf {
    <LibraryClasses>
    ApCommunicationLib|AgesaModulePkg/Library/ApCommunicationXvLib/ApCommunicationXvLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxDownCoreLib|AgesaModulePkg/Library/CcxDownCoreXvLib/CcxDownCoreXvLib.inf
    CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesXvLib/CcxPstatesXvLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Fabric/ZP/FabricZpPei/AmdFabricZpPei.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Fabric/SSP/FabricSspPei/AmdFabricSspPei.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
  }                                                                                                                    <SSP|MTS>
  AgesaModulePkg/Fabric/RV/FabricRvPei/AmdFabricRvPei.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Fabric/XV/UnbXvPei/AmdUnbXvPei.inf {
    <LibraryClasses>
    FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesUnbLib/FabricPstatesUnbLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV8Pei/AmdNbioSmuV8Pei.inf                                                          <FAMILY15H>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV9Pei/AmdNbioSmuV9Pei.inf                                                          <ZP>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV10Pei/AmdNbioSmuV10Pei.inf                                                        <RV>
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieZPPei/AmdNbioPcieZPPei.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieRVPei/AmdNbioPcieRVPei.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseZPPei/AmdNbioBaseZPPei.inf                                                   <ZP>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseCZPei/AmdNbioBaseCZPei.inf                                                   <FAMILY15H>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseRVPei/AmdNbioBaseRVPei.inf                                                   <RV>
  AgesaModulePkg/Nbio/GFX/AmdNbioGfxRVPei/AmdNbioGfxRVPei.inf                                                          <RV>
  AgesaModulePkg/Nbio/GFX/RN/PEI/AmdNbioGfxRNPei.inf                                                                   <RN>
  AgesaModulePkg/Nbio/IOMMU/AmdNbioIOMMUZPPei/AmdNbioIOMMUZPPei.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
  }                                                                                                                    <ZP|RV>
  AgesaModulePkg/Nbio/SMU/SSP/PEI/SmuV11PeiSsp.inf                                                                     <SSP|MTS>
  AgesaModulePkg/Nbio/PCIe/SSP/PEI/AmdNbioPcieSspPei.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
  }                                                                                                                    <SSP|MTS>
  AgesaModulePkg/Nbio/NbioBase/SSP/PEI/AmdNbioBaseSspPei.inf                                                           <SSP|MTS>
  AgesaModulePkg/Nbio/IOMMU/SSP/PEI/AmdNbioIOMMUSspPei.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
  }                                                                                                                    <SSP|MTS>
  AgesaModulePkg/Nbio/SMU/RN/PEI/SmuV12PeiRN.inf                                                                       <RN>
  AgesaModulePkg/Nbio/PCIe/RN/PEI/AmdNbioPcieRNPei.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
  }                                                                                                                    <RN>
  AgesaModulePkg/Nbio/NbioBase/RN/PEI/AmdNbioBaseRNPei.inf                                                             <RN>
  AgesaModulePkg/Nbio/IOMMU/RN/PEI/AmdNbioIOMMURNPei.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
  }                                                                                                                    <RN>

  AgesaModulePkg/Fch/Taishan/FchTaishanPei/FchPei.inf                                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmbusPei/Smbus.inf                                                              <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchPei/FchMultiFchPei.inf  {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Fch/Huashan/FchHuashanPei/FchPei.inf                                                                  <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanSmbusPei/Smbus.inf                                                              <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanMultiFchPei/FchMultiFchPei.inf  {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
  }                                                                                                                    <SSP>
  AgesaModulePkg/Fch/Sandstone/FchSandstonePei/FchPei.inf                                                              <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmbusPei/Smbus.inf                                                          <RV>
  AgesaModulePkg/Fch/Kern/FchKernPei/FchPei.inf                                                                        <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmbusPei/Smbus.inf                                                                    <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemAm4ZpPei/AmdMemAm4ZpPei.inf                                                                 <AM4BR|AM4RV|FP5RV|AM4SM|SP4SM|FT5RV>
  AgesaModulePkg/Mem/AmdMemAm4SspPei/AmdMemAm4SspPei.inf                                                               <AM4MTS>
  AgesaModulePkg/Mem/AmdMemSspSp3Pei/AmdMemSspSp3Pei.inf                                                               <SP3SSP|SP3CP>
  AgesaModulePkg/Mem/AmdMemZpSp3Pei/AmdMemZpSp3Pei.inf                                                                 <SP3ZP|SP4ZP>
  AgesaModulePkg/Mem/AmdMemZpSp3r2Pei/AmdMemZpSp3r2Pei.inf                                                             <SP3TR>
  AgesaModulePkg/Soc/AmdSocAm4MtsPei/AmdSocAm4MtsPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceInit3Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <AM4MTS>
  AgesaModulePkg/Soc/AmdSocAm4RvPei/AmdSocAm4RvPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceInit1Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistRvLib/PeiSocBistRvLib.inf
  }                                                                                                                    <AM4RV>
  AgesaModulePkg/Soc/AmdSocAm4SmPei/AmdSocAm4SmPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceInit1Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <AM4SM>
  AgesaModulePkg/Soc/AmdSocSp4SmPei/AmdSocSp4SmPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceInit1Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP4SM>
  AgesaModulePkg/Soc/AmdSocAm4BrPei/AmdSocAm4BrPei.inf {
    <LibraryClasses>
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdUnbLib/BaseSocketLogicalIdUnbLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyUnbLib/BaseFabricTopologyUnbLib.inf
  }                                                                                                                    <AM4BR>
  AgesaModulePkg/Soc/AmdSocFp5RvPei/AmdSocFp5RvPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceInit1Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistRvLib/PeiSocBistRvLib.inf
  }                                                                                                                    <FP5RV>
  AgesaModulePkg/Soc/AmdSocFp6RnPei/AmdSocFp6RnPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceInit3Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistRvLib/PeiSocBistRvLib.inf
  }                                                                                                                    <FP6RN>
  AgesaModulePkg/Soc/AmdSocAm4RnPei/AmdSocAm4RnPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceInit3Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistRvLib/PeiSocBistRvLib.inf
  }                                                                                                                    <AM4RN>
  AgesaModulePkg/Soc/AmdSocSp3RmPei/AmdSocSp3RmPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceInit3Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP3SSP>
  AgesaModulePkg/Soc/AmdSocSp3ZpPei/AmdSocSp3ZpPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceInit1Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP3ZP>
  AgesaModulePkg/Soc/AmdSocSp4ZpPei/AmdSocSp4ZpPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceInit1Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP4ZP>
  AgesaModulePkg/Soc/AmdSocSp3r2TrPei/AmdSocSp3r2TrPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceInit1Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP3TR>
  AgesaModulePkg/Soc/AmdSocSp3r3CpPei/AmdSocSp3r3CpPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceInit3Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistZpLib/PeiSocBistZpLib.inf
  }                                                                                                                    <SP3CP>
  AgesaModulePkg/Soc/AmdSocFt5RvPei/AmdSocFt5RvPei.inf {
    <LibraryClasses>
    FabricResourceInitLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceInit1Lib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    PeiSocBistLib|AgesaModulePkg/Library/PeiSocBistRvLib/PeiSocBistRvLib.inf
  }                                                                                                                    <FT5RV>

  AgesaModulePkg/Mem/AmdMemFeatPei/AmdMemFeatPei.inf                                                                   <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemCzPei/AmdMemCzPei.inf {
    <LibraryClasses>
    CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesXvLib/CcxPstatesXvLib.inf
    FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesUnbLib/FabricPstatesUnbLib.inf
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemChanXLatZpPei/MemChanXLatZpPei.inf                                                          <ZP|RV>
  AgesaModulePkg/Mem/AmdMemChanXLatSspPei/MemChanXLatSspPei.inf                                                        <SSP|MTS>
  AgesaModulePkg/Mem/AmdMemSmbiosV2RvPei/MemSmbiosV2Pei.inf {
    <LibraryClasses>
    AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2RvLib/MemSmbiosV2Lib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Mem/AmdMemSmbiosV2ZpPei/MemSmbiosV2Pei.inf {
    <LibraryClasses>
    AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2ZpLib/MemSmbiosV2Lib.inf
  }                                                                                                                    <AM4SM|SP4SM>
  AgesaModulePkg/Mem/AmdMemSmbiosV2ZpMcmPei/MemSmbiosV2Pei.inf {
    <LibraryClasses>
    AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2ZpMcmLib/MemSmbiosV2Lib.inf
  }                                                                                                                    <SP3ZP|SP4ZP|SP3TR>
  AgesaModulePkg/Mem/AmdMemSmbiosV2SspPei/MemSmbiosV2Pei.inf {
    <LibraryClasses>
    AmdMemSmbiosV2Lib|AgesaModulePkg/Library/MemSmbiosV2SspLib/MemSmbiosV2Lib.inf
  }                                                                                                                    <SSP|MTS>
  AgesaModulePkg/Mem/AmdMemRestorePei/MemRestorePei.inf                                                                <FAMILY17H>
  AgesaModulePkg/Mem/AmdMbistZpSp3Pei/AmdMbistPei.inf                                                                  <SP3ZP|SP4ZP>
  AgesaModulePkg/Mem/AmdMbistSspPei/AmdMbistPei.inf                                                                    <SP3SSP>
  AgesaModulePkg/ErrorLog/AmdErrorLogPei/AmdErrorLogPei.inf                                                            <ALL>
  AgesaModulePkg/Nbio/EarlyPhase/AmdNbioEarlyPhaseRVPei/AmdNbioEarlyPhaseRVPei.inf                                     <RV>
  AgesaModulePkg/Mem/AmdMemoryHobInfoPeimRv/AmdMemoryHobInfoPeim.inf                                                   <RV>
  AgesaModulePkg/Mem/AmdMemoryHobInfoPeimZp/AmdMemoryHobInfoPeim.inf                                                   <ZP|AM4BR>
  AgesaModulePkg/Mem/AmdMemoryHobInfoPeimV2/AmdMemoryHobInfoPeimV2.inf                                                 <SP3SSP|SP3CP|AM4MTS>
  AgesaModulePkg/Universal/Version/AmdVersionPei/AmdVersionPei.inf                                                     <ALL>

  ## PT Pei Drivers
  AgesaModulePkg/Fch/Promontory/PromontoryPei/FchPromontoryPei.inf                                                     <AM4|SP3TR|SP3CP>
  AgesaModulePkg/Fch/Promontory/PromontoryPei/FchPromontoryGpioPei.inf                                                 <AM4|SP3TR|SP3CP>
  AgesaModulePkg/Fch/Promontory/PromontoryPei/FchPromontoryCbsPei.inf                                                  <AM4|SP3TR|SP3CP>

[Components.X64]
  AgesaModulePkg/Debug/AmdIdsDebugPrintDxe/AmdIdsDebugPrintDxe.inf                                                     <ALL>
  AgesaModulePkg/Mem/AmdMemAm4Dxe/AmdMemAm4Dxe.inf                                                                     <AM4|FP5RV|FP6RN|SP4SM|FT5RV>
  AgesaModulePkg/Mem/AmdMemSspSp3Dxe/AmdMemSspSp3Dxe.inf                                                               <SP3SSP|SP3CP>
  AgesaModulePkg/Mem/AmdMemZpSp3Dxe/AmdMemZpSp3Dxe.inf                                                                 <SP3ZP|SP4ZP>
  AgesaModulePkg/Mem/AmdMemZpSp3r2Dxe/AmdMemZpSp3r2Dxe.inf                                                           <SP3TR>
  AgesaModulePkg/Mem/AmdMemSmbiosDxe/AmdMemSmbiosDxe.inf                                                               <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemSmbiosV2Dxe/AmdMemSmbiosV2Dxe.inf                                                           <FAMILY17H>
  AgesaModulePkg/Mem/AmdMemS3CzDxe/AmdMemS3CzDxe.inf {
    <LibraryClasses>
    FabricPstatesLib|AgesaModulePkg/Library/FabricPstatesUnbLib/FabricPstatesUnbLib.inf
    CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesXvLib/CcxPstatesXvLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Mem/AmdMemRestoreDxe/MemRestoreDxe.inf                                                                <FAMILY17H>
  AgesaModulePkg/Mem/AmdMemPprSmmDriverSsp/AmdMemPprSmmDriverSsp.inf                                                   <SP3SSP>
  AgesaModulePkg/Psp/ApcbDrv/ApcbZpDxe/ApcbZpDxe.inf{
    <LibraryClasses>
    ApcbZpLib|AgesaModulePkg/Library/ApcbZpLib/ApcbLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Psp/ApcbDrv/ApcbZpSmm/ApcbZpSmm.inf{
    <LibraryClasses>
    ApcbZpLib|AgesaModulePkg/Library/ApcbZpLib/ApcbLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Psp/ApcbDrv/ApcbRvDxe/ApcbRvDxe.inf{
    <LibraryClasses>
    ApcbRvLib|AgesaModulePkg/Library/ApcbRvLib/ApcbLib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Psp/ApcbDrv/ApcbRvSmm/ApcbRvSmm.inf{
    <LibraryClasses>
    ApcbRvLib|AgesaModulePkg/Library/ApcbRvLib/ApcbLib.inf
  }                                                                                                                    <RV>
#  AgesaModulePkg/Psp/ApcbDrv/ApcbSspDxe/ApcbSspDxe.inf{
#    <LibraryClasses>
#    ApcbSspLib|AgesaModulePkg/Library/ApcbSspLib/ApcbLib.inf
#  }                                                                                                                    <SSP|MTS>
#  AgesaModulePkg/Psp/ApcbDrv/ApcbSspSmm/ApcbSspSmm.inf{
#    <LibraryClasses>
#    ApcbSspLib|AgesaModulePkg/Library/ApcbSspLib/ApcbLib.inf
#  }                                                                                                                    <SSP|MTS>
  AgesaModulePkg/Psp/ApcbDrv/ApcbV3Dxe/ApcbV3Dxe.inf                                                                   <SSP|MTS>
  AgesaModulePkg/Psp/ApcbDrv/ApcbV3Smm/ApcbV3Smm.inf                                                                   <SSP|MTS>
  AgesaModulePkg/Psp/AmdPspDxeV1/AmdPspDxeV1.inf                                                                       <FAMILY15H>
  AgesaModulePkg/Psp/AmdPspSmmV1/AmdPspSmmV1.inf                                                                       <FAMILY15H>
  AgesaModulePkg/Psp/AmdPspP2CmboxV1/AmdPspP2CmboxV1.inf                                                               <FAMILY15H>

  AgesaModulePkg/Psp/AmdPspDxeV2Zp/AmdPspDxeV2.inf                                                                     <AM4SM|SP4SM>
  AgesaModulePkg/Psp/AmdPspDxeV2ZpMcm/AmdPspDxeV2.inf                                                                  <SP3ZP|SP4ZP|SP3TR>
  AgesaModulePkg/Psp/AmdPspDxeV2Rv/AmdPspDxeV2.inf                                                                     <RV>
  AgesaModulePkg/Psp/AmdPspDxeV2Ssp/AmdPspDxeV2.inf                                                                    <SSP|MTS>

  AgesaModulePkg/Psp/AmdPspP2CmboxV2/AmdPspP2CmboxV2.inf                                                               <ZP|RV>
  AgesaModulePkg/Psp/AmdPspP2CmboxV2/AmdPspP2CmboxV2SmmBuffer.inf                                                      <ZP|RV|RN|CZN>
  AgesaModulePkg/Psp/AmdPspP2CmboxV2Ssp/AmdPspP2CmboxV2.inf                                                            <SSP|MTS>
  AgesaModulePkg/Psp/AmdPspP2CmboxV2Ssp/AmdPspP2CmboxV2SmmBuffer.inf                                                   <SSP|MTS|GN|BA|VMR>

  AgesaModulePkg/Psp/AmdPspSmmV2/AmdPspSmmV2.inf                                                                       <ZP|RV>
  AgesaModulePkg/Psp/AmdPspSmmV2Ssp/AmdPspSmmV2.inf                                                                    <SSP|MTS>

  AgesaModulePkg/Psp/AmdPspFtpmDxe/AmdPspFtpmDxe.inf                                                                   <AM4|RV|SP3TR|SP3CP|SP4SM>
  AgesaModulePkg/Psp/AmdHstiV1/AmdHstiV1.inf                                                                           <FAMILY15H>
  AgesaModulePkg/Psp/AmdHstiV2/AmdHstiV2.inf                                                                           <FAMILY17H>

  AgesaModulePkg/Psp/ApobDrv/ApobZpRvDxe/ApobZpRvDxe.inf                                                               <AM4RV|FP5RV|AM4SM|SP4SM>
  AgesaModulePkg/Psp/ApobDrv/ApobZpMcmDxe/ApobZpMcmDxe.inf                                                             <SP3ZP|SP4ZP|SP3TR>
  AgesaModulePkg/Psp/ApobDrv/ApobSspDxe/ApobSspDxe.inf                                                                 <SSP|MTS>
  AgesaModulePkg/Psp/ApobDrv/ApobDummyDxe/ApobDummyDxe.inf                                                             <FAMILY15H>

  AgesaModulePkg/Ccx/Zen/CcxZenZpDxe/AmdCcxZenZpDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaZpLib/CcxSetMcaZpLib.inf
    FabricWdtLib|AgesaModulePkg/Library/FabricWdtZpLib/FabricWdtZpLib.inf
    CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateZenLib/CcxIdsCustomPstatesZenLib.inf
    CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosZenZpLib/CcxSmbiosZenZpLib.inf
  }                                                                                                                    <AM4SM|SP4SM|SP3TR>
  AgesaModulePkg/Ccx/Zen/CcxZenZpDxe/AmdCcxZenZpDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaZpLib/CcxSetMcaZpLib.inf
    FabricWdtLib|AgesaModulePkg/Library/FabricWdtZpLib/FabricWdtZpLib.inf
    CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateZenLib/CcxIdsCustomPstatesZenLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesZpLib/CcxResetTablesZpLib.inf
    CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosZenZpLib/CcxSmbiosZenZpLib.inf
  }                                                                                                                    <SP3ZP|SP4ZP>
  AgesaModulePkg/Ccx/Zen/CcxZenRvDxe/AmdCcxZenRvDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesRvLib/CcxResetTablesRvLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateZenLib/CcxIdsCustomPstatesZenLib.inf
    CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaRvLib/CcxSetMcaRvLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesRvLib/CcxResetTablesRvLib.inf
    CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosZenRvLib/CcxSmbiosZenRvLib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Ccx/Vh/Dxe/AmdCcxVhDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesVhLib/CcxResetTablesVhLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaVhLib/CcxSetMcaVhLib.inf
    FabricWdtLib|AgesaModulePkg/Library/FabricWdtDf3Lib/FabricWdtDf3Lib.inf
    CcxIdsCustomPstatesLib|AgesaModulePkg/Library/CcxIdsCustomPstateZenLib/CcxIdsCustomPstatesZenLib.inf
    CcxResetTablesLib|AgesaModulePkg/Library/CcxResetTablesVhLib/CcxResetTablesVhLib.inf
    CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosVhLib/CcxSmbiosVhLib.inf
    CcxRolesLib|AgesaModulePkg/Library/CcxRolesVhLib/CcxRolesVhLib.inf
  }                                                                                                                    <SSP|MTS|RN>
  AgesaModulePkg/Ccx/Xv/CcxXvDxe/AmdCcxXvDxe.inf {
    <LibraryClasses>
    CcxPstatesLib|AgesaModulePkg/Library/CcxPstatesXvLib/CcxPstatesXvLib.inf
    CcxSmbiosLib|AgesaModulePkg/Library/CcxSmbiosXvLib/CcxSmbiosXvLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Ccx/Xv/CcxXvSmm/AmdCcxXvSmm.inf  {
    <LibraryClasses>
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Ccx/Zen/CcxZenZpSmm/AmdCcxZenZpSmm.inf {
    <LibraryClasses>
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaZpLib/CcxSetMcaZpLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Ccx/Zen/CcxZenRvSmm/AmdCcxZenRvSmm.inf {
    <LibraryClasses>
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaRvLib/CcxSetMcaRvLib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Ccx/Vh/Smm/AmdCcxVhSmm.inf {
    <LibraryClasses>
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    CcxSetMcaLib|AgesaModulePkg/Library/CcxSetMcaVhLib/CcxSetMcaVhLib.inf
  }                                                                                                                    <SSP|MTS|RN>
  AgesaModulePkg/Fabric/ZP/FabricZpDxe/AmdFabricZpDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Fabric/ZP/FabricZpSmm/AmdFabricZpSmm.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Fabric/SSP/FabricSspDxe/AmdFabricSspDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
  }                                                                                                                    <SSP|MTS>
  AgesaModulePkg/Fabric/SSP/FabricSspSmm/AmdFabricSspSmm.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
  }                                                                                                                    <SSP|MTS>
  AgesaModulePkg/Fabric/RV/FabricRvDxe/AmdFabricRvDxe.inf {
    <LibraryClasses>
    IdsLib|AgesaModulePkg/Library/IdsNonUefiLib/IdsNonUefiLib.inf
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Fabric/RV/FabricRvSmm/AmdFabricRvSmm.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Fabric/XV/UnbXvDxe/AmdUnbXvDxe.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerUnbLib/FabricResourceManagerUnbLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    AmdTableSmnLib|AgesaModulePkg/Library/AmdTableLib/Null/AmdTableSmnNullLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Soc/AmdSocAm4MtsDxe/AmdSocAm4MtsDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  }                                                                                                                    <AM4MTS>
  AgesaModulePkg/Soc/AmdSocAm4SmDxe/AmdSocAm4SmDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  }                                                                                                                    <AM4SM>
  AgesaModulePkg/Soc/AmdSocSp4SmDxe/AmdSocSp4SmDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  }                                                                                                                    <SP4SM>
  AgesaModulePkg/Soc/AmdSocAm4BrDxe/AmdSocAm4BrDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdUnbLib/BaseSocketLogicalIdUnbLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatBrLib/BaseSocLogicalIdXlatBrLib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyUnbLib/BaseFabricTopologyUnbLib.inf
  }                                                                                                                    <AM4BR>
  AgesaModulePkg/Soc/AmdSocFp5RvDxe/AmdSocFp5RvDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  }                                                                                                                    <FP5RV>
  AgesaModulePkg/Soc/AmdSocFp6RnDxe/AmdSocFp6RnDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  }                                                                                                                    <FP6RN>
  AgesaModulePkg/Soc/AmdSocAm4RnDxe/AmdSocAm4RnDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  }                                                                                                                    <AM4RN>
  AgesaModulePkg/Soc/AmdSocSp3RmDxe/AmdSocSp3RmDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
  }                                                                                                                    <SP3SSP>
  AgesaModulePkg/Soc/AmdSocSp3ZpDxe/AmdSocSp3ZpDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  }                                                                                                                    <SP3ZP>
  AgesaModulePkg/Soc/AmdSocSp4ZpDxe/AmdSocSp4ZpDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  }                                                                                                                    <SP4ZP>
  AgesaModulePkg/Soc/AmdSocAm4RvDxe/AmdSocAm4RvDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
    AmdIdsHookExtLib|AgesaModulePkg/Library/AmdIdsExtLibNull/AmdIdsHookExtLibNull.inf
  }                                                                                                                    <AM4RV>
  AgesaModulePkg/Soc/AmdSocSp3r2TrDxe/AmdSocSp3r2TrDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdZpDieLib/BaseSocketLogicalIdZpDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatZpDieLib/BaseSocLogicalIdXlatZpDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  }                                                                                                                    <SP3TR>
  AgesaModulePkg/Soc/AmdSocSp3r3CpDxe/AmdSocSp3r3CpDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdSspDieLib/BaseSocketLogicalIdSspDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatSspDieLib/BaseSocLogicalIdXlatSspDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
  }                                                                                                                    <SP3CP>
  AgesaModulePkg/Soc/AmdSocFt5RvDxe/AmdSocFt5RvDxe.inf {
    <LibraryClasses>
    AmdS3SaveLib|AgesaModulePkg/Library/AmdS3SaveLib/WOS3Save/AmdWOS3SaveLib.inf
    BaseSocketLogicalIdLib|AgesaModulePkg/Library/BaseSocketLogicalIdRvDieLib/BaseSocketLogicalIdRvDieLib.inf
    BaseSocLogicalIdXlatLib|AgesaModulePkg/Library/BaseSocLogicalIdXlatRvDieLib/BaseSocLogicalIdXlatRvDieLib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  }                                                                                                                    <FT5RV>

  AgesaModulePkg/ErrorLog/AmdErrorLogDxe/AmdErrorLogDxe.inf                                                            <ALL>

  ## Gnb Dxe Drivers
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseCZDxe/AmdNbioBaseCZDxe.inf {
      <LibraryClasses>
    CcxBtcLib|AgesaModulePkg/Library/CcxBtcLib/CcxXvBtcLib.inf
  }                                                                                                                    <FAMILY15H>
  AgesaModulePkg/Nbio/NbioBase/RN/DXE/AmdNbioBaseRNDxe.inf                                                   <RN>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseRVDxe/AmdNbioBaseRVDxe.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Nbio/NbioBase/AmdNbioBaseZPDxe/AmdNbioBaseZPDxe.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccSoc15Lib/FabricRegisterAccSoc15Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologyZpLib/BaseFabricTopologyZpLib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV9Dxe/AmdNbioSmuV9Dxe.inf                                                          <ZP>
  AgesaModulePkg/Nbio/SMU/AmdNbioSmuV10Dxe/AmdNbioSmuV10Dxe.inf                                                        <RV>
  AgesaModulePkg/Nbio/SMU/RN/DXE/SmuV12DxeRN.inf                                                                  <RN>
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieZPDxe/AmdNbioPcieZPDxe.inf                                                       <ZP>
  AgesaModulePkg/Nbio/PCIe/AmdNbioPcieRVDxe/AmdNbioPcieRVDxe.inf                                                       <RV>
  AgesaModulePkg/Nbio/PCIe/RN/DXE/AmdNbioPcieRNDxe.inf                                                                 <RN>
  AgesaModulePkg/Nbio/IOMMU/AmdNbioIOMMUZPDxe/AmdNbioIOMMUZPDxe.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
  }                                                                                                                    <ZP>
  AgesaModulePkg/Nbio/IOMMU/RN/DXE/AmdNbioIOMMURNDxe.inf                                                               <RN>
  AgesaModulePkg/Nbio/IOMMU/AmdNbioIOMMURVDxe/AmdNbioIOMMURVDxe.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf1Lib/FabricResourceManager1Lib.inf
  }                                                                                                                    <RV>
  AgesaModulePkg/Nbio/GFX/AmdNbioGfxRVDxe/AmdNbioGfxRVDxe.inf                                                          <RV>
  AgesaModulePkg/Nbio/GFX/RN/DXE/AmdNbioGfxRNDxe.inf                                                                   <RN>
  AgesaModulePkg/Nbio/NbioALib/SSP/DXE/AmdNbioAlibDxe.inf                                                              <SSP|MTS>
  AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibZPDxe/AmdNbioAlibZPDxe.inf                                                   <ZP>
  AgesaModulePkg/Nbio/NbioALib/AmdNbioAlibRVDxe/AmdNbioAlibRVDxe.inf                                                   <RV>
#  AgesaModulePkg/Nbio/IOAPIC/RN/PEI/AmdNbioIoApicRNPei.inf                                                             <RN>
  AgesaModulePkg/Nbio/NbioBase/SSP/DXE/AmdNbioBaseSspDxe.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
    BaseFabricTopologyLib|AgesaModulePkg/Library/BaseFabricTopologySspLib/BaseFabricTopologySspLib.inf
  }                                                                                                                    <SSP|MTS>
  AgesaModulePkg/Nbio/SMU/SSP/DXE/SmuV11DxeSsp.inf                                                                     <SSP|MTS>
  AgesaModulePkg/Nbio/PCIe/SSP/DXE/AmdNbioPcieSspDxe.inf                                                               <SSP|MTS>
  AgesaModulePkg/Nbio/IOMMU/SSP/DXE/AmdNbioIOMMUSspDxe.inf {
    <LibraryClasses>
    FabricResourceManagerLib|AgesaModulePkg/Library/FabricResourceManagerDf3Lib/FabricResourceManager3Lib.inf
  }                                                                                                                    <SSP|MTS>

  ## Fch Dxe Drivers
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchDxe.inf                                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmm/FchSmm.inf                                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmbusDxe/SmbusLight.inf                                                         <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanCf9ResetDxe/Cf9Reset.inf                                                        <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanLegacyInterruptDxe/LegacyInterrupt.inf                                          <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmmControlDxe/SmmControl.inf                                                    <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanSmmDispatcher/FchSmmDispatcher.inf                                              <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchDxe/FchMultiFchDxe.inf                                                  <ZP>
  AgesaModulePkg/Fch/Taishan/FchTaishanMultiFchSmm/FchMultiFchSmm.inf                                                  <SP3ZP|SP4ZP|SP3TR>
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchTaishanSsdt.inf                                                          <AM4SM|SP4SM>
  AgesaModulePkg/Fch/Taishan/FchTaishanDxe/FchTaishanAml.inf                                                           <AM4SM|SP4SM>
  AgesaModulePkg/Fch/Huashan/FchHuashanDxe/FchDxe.inf                                                                  <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanSmm/FchSmm.inf                                                                  <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanSmbusDxe/SmbusLight.inf                                                         <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanCf9ResetDxe/Cf9Reset.inf                                                        <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanLegacyInterruptDxe/LegacyInterrupt.inf                                          <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanSmmControlDxe/SmmControl.inf                                                    <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanSmmDispatcher/FchSmmDispatcher.inf                                              <SSP|MTS>
  AgesaModulePkg/Fch/Huashan/FchHuashanMultiFchDxe/FchMultiFchDxe.inf {
    <LibraryClasses>
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
  }                                                                                                                    <SSP>
  AgesaModulePkg/Fch/Huashan/FchHuashanMultiFchSmm/FchMultiFchSmm.inf {
    <LibraryClasses>
    FabricRegisterAccLib|AgesaModulePkg/Library/FabricRegisterAccDf3Lib/FabricRegisterAccDf3Lib.inf
  }                                                                                                                    <SSP>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneDxe/FchDxe.inf                                                              <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmm/FchSmm.inf                                                              <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmbusDxe/SmbusLight.inf                                                     <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneCf9ResetDxe/Cf9Reset.inf                                                    <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneLegacyInterruptDxe/LegacyInterrupt.inf                                      <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmmControlDxe/SmmControl.inf                                                <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneSmmDispatcher/FchSmmDispatcher.inf                                          <RV>
  AgesaModulePkg/Fch/Sandstone/FchSandstoneComplement/FchSSSataD3Cold/FchSSSataD3ColdSmm.inf                           <RV>
  AgesaModulePkg/Fch/Kern/FchKernDxe/FchDxe.inf                                                                        <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmm/FchSmm.inf                                                                        <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmbusDxe/SmbusLight.inf                                                               <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernCf9ResetDxe/Cf9Reset.inf                                                              <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernLegacyInterruptDxe/LegacyInterrupt.inf                                                <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmmControlDxe/SmmControl.inf                                                          <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernSmmDispatcher/FchSmmDispatcher.inf                                                    <FAMILY15H>
  AgesaModulePkg/Fch/Kern/FchKernComplement/ImcControl/ImcControl.inf                                                  <FAMILY15H>

  # Universal
  AgesaModulePkg/Universal/Smbios/AmdSmbiosDxe.inf                                                                     <ALL>
  AgesaModulePkg/Universal/Acpi/AmdAcpiDxe.inf                                                                         <ALL>
  AgesaModulePkg/Universal/AmdSmmCommunication/AmdSmmCommunication.inf                                                 <ALL>
  AgesaModulePkg/Universal/Version/AmdVersionDxe/AmdVersionDxe.inf                                                     <ALL>
  AgesaModulePkg/Universal/AmdiGpuVgaControlDxe/AmdiGpuVgaControlDxe.inf                                               <FAMILY15H>

  ## PT Dxe Drivers
  AgesaModulePkg/Fch/Promontory/PromontoryDxe/FchPromontoryGpioDxe.inf                                                 <AM4|SP3TR|SP3CP>
  AgesaModulePkg/Fch/Promontory/PromontoryDxe/FchPromontorySsdt.inf                                                    <AM4|SP3TR|SP3CP>
  AgesaModulePkg/Fch/Promontory/PromontorySmm/FchPromontorySmm.inf                                                     <AM4|SP3TR|SP3CP>
  AgesaModulePkg/Fch/Promontory/PromontorySmm/FchPromontoryGpioSmm.inf                                                 <AM4|SP3TR|SP3CP>
  AgesaModulePkg/Fch/Promontory/PromontoryDxe/FchPromontoryAml.inf                                                     <AM4|SP3TR|SP3CP>

  ## Ras Dxe Drivers
  AgesaModulePkg/Ras/AmdRasZpApeiDxe/AmdRasZpApeiDxe.inf                                                               <ZP>
  AgesaModulePkg/Ras/AmdRasZpDxe/AmdRasZpDxe.inf                                                                       <ZP>
  AgesaModulePkg/Ras/AmdRasZpSmm/AmdRasZpSmm.inf                                                                       <ZP>
  AgesaModulePkg/Ras/AmdRasRvApeiDxe/AmdRasRvApeiDxe.inf                                                               <RV>
  AgesaModulePkg/Ras/AmdRasRvDxe/AmdRasRvDxe.inf                                                                       <RV>
  AgesaModulePkg/Ras/AmdRasRvSmm/AmdRasRvSmm.inf                                                                       <RV>
  AgesaModulePkg/Ras/AmdRasSspApeiDxe/AmdRasSspApeiDxe.inf                                                             <SSP|MTS>
  AgesaModulePkg/Ras/AmdRasSspDxe/AmdRasSspDxe.inf                                                                     <SSP|MTS>
  AgesaModulePkg/Ras/AmdRasSspSmm/AmdRasSspSmm.inf                                                                     <SSP|MTS>
  AgesaModulePkg/Ras/AmdRasBrApeiDxe/AmdRasBrApeiDxe.inf                                                               <FAMILY15H>

  ## NVDIMM Dxe Driver
  AgesaModulePkg/JedecNvdimm/JedecNvdimm.inf                                                                           <SP3SSP|Sp3SspZp|SP3ZP>

[PcdsFixedAtBuild]
  # AMD AGESA PI Name & Version string
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9/0BristolPI-AM4 1.1.0.0"                            <*Am4Br>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0RavenPI-FP5-AM4 1.1.0.2"                          <*Am4Rv>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SummitPI-AM4 0.0.6.0"                             <*Am4SmBr>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0RavenPI-FP5-AM4 1.1.0.2"                          <*Am4RvSmBr>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0RavenPI-FP5-AM4 1.1.0.2"                          <*Am4RvSm>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SummitPI-AM4 0.0.6.0"                             <*Am4Sm>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0PinnaclePI-SP4 0.0.5.0"                           <*Sp4Sm>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0PicassoPI-FP5 1.0.1.0"                            <*Fp5Rv>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0RenoirPI-FP6-AM4 E.0.1.0"                         <*Fp6Rn>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0NaplesPI-SP3 0.0.6.0"                             <*Sp3Zp>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0SnowOwlPI-SP4 0.0.6.0"                            <*Sp4Zp>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0ThreadRipperPI-SP3r2-0.0.5.0"                     <*Sp3TR>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0MatissePI-AM4 0.0.5.0"                            <*Am4Mts>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0CastlePeakPI-SP3r3 0.0.1.0"                       <*Sp3CP>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0RomePI-SP3 0.0.3.0"                               <*Sp3Ssp>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0RomePI-SP3 0.0.3.0"                               <*Sp3SspZp>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdAmdPackageString|"AGESA!V9\0PollockPI-FT5 1.0.0.0"                            <*Ft5Rv>

[PcdsDynamicDefault]
  # Gnb Gbs Override
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdIommuL1ClockGatingEnable|TRUE                                                 <RV>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdIommuL2ClockGatingEnable|TRUE                                                 <RV>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdIOHCClkGatingSupport|TRUE                                                     <RV>
  gEfiAmdAgesaModulePkgTokenSpaceGuid.PcdCfgSstunlClkGating|TRUE                                                       <RV>

  # CCX and Fabric Default
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdCStateMode|1                                                                     <FP5RV|FT5RV>
  gEfiAmdAgesaPkgTokenSpaceGuid.PcdAmdAcpiC3Ctrl|0x00                                                                  <FP5RV|FT5RV>

