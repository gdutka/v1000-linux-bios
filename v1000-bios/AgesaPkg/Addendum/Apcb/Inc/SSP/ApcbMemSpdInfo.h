/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _APCB_MEM_SPD_INFO_H_
#define _APCB_MEM_SPD_INFO_H_
//
// @APCB_START
//

/// SPD_MUX_INFO
typedef struct _SSP_SPD_MUX_INFO {
  UINT8  SpdMuxPresent;      ///< SpdMux Present or not. if 1, then yes otherwise no
  UINT8  MuxI2CAddress;      ///< MuxI2cAddress
  UINT8  MuxChannel;         ///< MuxChannel no.
  UINT8  Reserved;           ///< Reserved
} SSP_SPD_MUX_INFO;

/// Dram technology type
typedef enum {
  SSP_SODIMM_PACKAGE,        ///< SODIMM package
  SSP_UDIMM_PACKAGE,         ///< UDIMM Package
  SSP_RDIMM_PACKAGE,         ///< RDIMM Package
  SSP_DRAMDOWN_PACKAGE,      ///< DRAM Down Package
  SSP_UNSUPPORTED_PACKAGE,   ///< Unsupported package
} SSP_DRAM_PACKAGE_TYPE;

/// ECC Symbol Size
typedef enum {
   SYMBOL_SIZE_X4,           ///< Symbol size x4
   SYMBOL_SIZE_X8,           ///< Symbol size x8
   SYMBOL_SIZE_X16,          ///< Symbol size x16

   SYMBOL_SIZE_UNSUPPORTED   ///< Boundary check
} ECC_SYMBOL_SIZE;

//-----------------------------------------------------------------------------
///
/// SPD Data for each DIMM.
///
typedef struct _SPD_SSP_DEF_STRUCT {
  BOOLEAN SpdValid; ///< Indicates that the SPD is valid
  BOOLEAN DimmPresent; ///< Indicates that the DIMM is present and Data is valid
  UINT8   PageAddress;  ///< Indicates the 256 Byte EE Page the data belongs to
                        //      0 = Lower Page
                        //      1 = Upper Page
  BOOLEAN NvDimmPresent; ///< Indicates this DIMM is NVDIMM
  UINT32  DramManufacturersIDCode;  /// DRAM Manufacture ID
  UINT32 Address;         ///< SMBus address of the DRAM
  SSP_SPD_MUX_INFO  SpdMuxInfo;   ///< Mux and Channel Number associated with this DIMM
  TECHNOLOGY_TYPE  Technology; ///< Indicates the type of Technology used in SPD
                             ///<   DDR3_TECHNOLOGY = Use DDR3 DIMMs
                           ///<   DDR4_TECHNOLOGY = Use DDR4 DIMMs
                           ///<   LPDDR3_TECHNOLOGY = Use LPDDR3
  SSP_DRAM_PACKAGE_TYPE  Package; ///< Indicates the package type
                             // SODIMM_PACKAGE = SODIMM package
                            // UDIMM_PACKAGE = UDIMM Package
                            // RDIMM_PACKAGE = RDIMM Package
                            // DRAMDOWN_PACKAGE = DRAM Down Package
  UINT8  SocketNumber;          ///< Indicates the socket number
  UINT8  ChannelNumber;          ///< Indicates the channel number
  UINT8  DimmNumber;          ///< Indicates the channel number
  UINT8  Reserved2;             ///< Reserved
  UINT8 Data[256];      ///< Buffer for 256 Bytes of SPD data from DIMM
} SPD_SSP_DEF_STRUCT;

///===============================================================================
/// _PSP_SPD_LPDDR3_STRUCT
/// This data structure is used to pass wrapper parameters to the memory configuration code
///

/** PSP_SPD_STRUCT
*    @GROUPID: APCB_GROUP_MEMORY
*    @TYPEID: APCB_MEM_TYPE_SPD_INFO
*    This data structure is used to pass wrapper parameters to the memory configuration code
*/
typedef struct _PSP_SPD_STRUCT {
   UINT32 TotalDimms;
  SPD_SSP_DEF_STRUCT  SpdData[APCB_TOTAL_SYSTEM_DIMMS];
} PSP_SPD_STRUCT;


#endif // _APCB_MEM_SPD_INFO_H_

