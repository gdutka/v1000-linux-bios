/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_MEM_PS_UDIMM_DDR4_ODT_PAT_H_
#define _APCB_MEM_PS_UDIMM_DDR4_ODT_PAT_H_
//
// @APCB_START
//

 //-----
  // DDR
 //-----

/// DDR4 Data Bus configuration for ZP+
typedef struct {
  UINT32 DimmPerCh;   ///< Bitmap of Dimm slot per chanel
  UINT32 DDRrate;    ///< Bitmap of DDR rate
  UINT32 VDDIO;       ///< Bitmap of VDDIO
  UINT32 Dimm0;       ///< Bitmap of rank type of Dimm0
  UINT32 Dimm1;       ///< Bitmap of rank type of Dimm1

  UINT32 RttNom;      ///< Rtt_Nom
  UINT32 RttWr;       ///< Rtt_Wr
  UINT32 DqStrength;  ///< Data drive strength
  UINT32 DqsStrength; ///< DQS drive strength
  UINT32 OdtStrength; ///< ODT drive strength
} PSCFG_DATABUS_ENTRY;


///===============================================================================
/// _PSP_PS_SODIMM_DDR_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
#define OdtPatSize 5
#define CadBusSODIMMSize 30
#define DataBusSODIMMSize 30
#define MaxFreqSODIMMFM2r2Size 7
typedef struct _PSP_PS_SODIMM_DDR_STRUCT {
  PSCFG_2D_ODTPAT_ENTRY OdtPat[OdtPatSize];
  PSC_TBL_ENTRY OdtPatTblEnt;
  PSCFG_CADBUS_ENTRY ROMDATA CadBusSODIMM[CadBusSODIMMSize];
  PSC_TBL_ENTRY CadBusTblEnSO;
  PSCFG_DATABUS_ENTRY ROMDATA DataBusSODIMM[DataBusSODIMMSize];
  PSC_TBL_ENTRY DataBusTblEnSO;
  PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqSODIMMFM2r2[MaxFreqSODIMMFM2r2Size];
  PSC_TBL_ENTRY MaxFreqTblEntSOFM2r2;
} PSP_PS_SODIMM_DDR_STRUCT;
///===============================================================================
/// _PSP_PS_UDIMM_DDR_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
#define CadBusUDIMMSize 30
#define DataBusUDIMMSize 30
#define MaxFreqUDIMMFM2r2Size 7
typedef struct _PSP_PS_UDIMM_DDR_STRUCT {
  //-----------------------------------------
  // Platform Specific Data Block
  //-----------------------------------------
  PSCFG_2D_ODTPAT_ENTRY OdtPat[OdtPatSize];
  PSC_TBL_ENTRY OdtPatTblEnt;
  PSCFG_CADBUS_ENTRY ROMDATA CadBusUDIMM[CadBusUDIMMSize];
  PSC_TBL_ENTRY CadBusTblEnU;
  PSCFG_DATABUS_ENTRY ROMDATA DataBusUDIMM[DataBusUDIMMSize];
  PSC_TBL_ENTRY DataBusTblEnU;
  PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqUDIMMFM2r2[MaxFreqUDIMMFM2r2Size];
  PSC_TBL_ENTRY MaxFreqTblEntUFM2r2;
} PSP_PS_UDIMM_DDR_STRUCT;
///===============================================================================
/// _PSP_PS_DRAMDOWN_DDR_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///
#define CadBusDRAMDIMMSize 1
#define DataBusDRAMDIMMSize 1
#define MaxFreqDRAMFM2r2Size 7
typedef struct _PSP_PS_DRAMDOWN_DDR_STRUCT {
  //-----------------------------------------
  // Platform Specific Data Block
  //-----------------------------------------
  PSCFG_2D_ODTPAT_ENTRY OdtPat[OdtPatSize];
  PSC_TBL_ENTRY OdtPatTblEnt;
  PSCFG_CADBUS_ENTRY ROMDATA CadBusDRAMDIMM[CadBusDRAMDIMMSize];
  PSC_TBL_ENTRY CadBusTblEn;
  PSCFG_DATABUS_ENTRY ROMDATA DataBusDRAMDIMM[DataBusDRAMDIMMSize];
  PSC_TBL_ENTRY DataBusTblEn;
  PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqDRAMFM2r2[MaxFreqDRAMFM2r2Size];
  PSC_TBL_ENTRY MaxFreqTblEntDFM2r2;
} PSP_PS_DRAMDOWN_DDR_STRUCT;




#endif // _APCB_MEM_PS_UDIMM_DDR4_ODT_PAT_H_
