/** @file
  GNbReg.h.

;*******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/


#ifndef _GNB_REG_H_
#define _GNB_REG_H_

#include "CommonReg.h"
//
// Definitions beginning with "R_" are registers
// Definitions beginning with "B_" are bits within registers
// Definitions beginning with "V_" are meaningful values of bits within the registers
// Definitions beginning with "S_" are register sizes
// Definitions beginning with "N_" are the bit position
//

//
// North Bridge(D0:F0) PCI Register
//

#define NB_BUS                        0
#define NB_DEV                        0
#define NB_FUNC                       0

#define NB_CLK_BUS                    0
#define NB_CLK_DEV                    0
#define NB_CLK_FUNC                   1

#define NB_PCIE_GFX1_BUS              0
#define NB_PCIE_GFX1_DEV              2
#define NB_PCIE_GFX1_FUNC             1

#define NB_GPP_BUS                    0
#define NB_GPP_DEV                    2

#define NB_PCIE_GFX2_BUS              0
#define NB_PCIE_GFX2_DEV              2
#define NB_PCIE_GFX2_FUNC             2

#define NB_PCIE_RP1_BUS               0
#define NB_PCIE_RP1_DEV               2
#define NB_PCIE_RP1_FUNC              3

#define NB_PCIE_RP2_BUS               0
#define NB_PCIE_RP2_DEV               2
#define NB_PCIE_RP2_FUNC              4

#define NB_PCIE_RP3_BUS               0
#define NB_PCIE_RP3_DEV               2
#define NB_PCIE_RP3_FUNC              5

#define INTERNAL_VGA_BUS              0
#define INTERNAL_VGA_DEV              8
#define INTERNAL_VGA_FUNC             1

#define EXTERNAL_VGA_BUS              1
#define EXTERNAL_VGA_DEV              0
#define EXTERNAL_VGA_FUNC             0

#define NB_VID                        0x1022
#define NB_DID                        0x9600
#define NB_DID2                       0x9600
#define NB_DID3                       0x9600
#define NB_DID4                       0x9601  //add for RS880
#define NB_DID10                      0x1705  //add for Fusion_Llano
#define INTERNAL_VGA_VID              0x1002
#define INTERNAL_VGA_VID1             0x1022

#define DEFAULT_IGP_DID               INTERNAL_VGA_DID13

#define INTERNAL_VGA_DID0             0x9830  // Fusion Kabini
#define INTERNAL_VGA_DID1             0x9831  // Fusion Kabini
#define INTERNAL_VGA_DID2             0x9832  // Fusion Kabini
#define INTERNAL_VGA_DID3             0x9833  // Fusion Kabini
#define INTERNAL_VGA_DID4             0x9834  // Fusion Kabini
#define INTERNAL_VGA_DID5             0x9835  // Fusion Kabini
#define INTERNAL_VGA_DID6             0x9836  // Fusion Kabini
#define INTERNAL_VGA_DID7             0x9837  // Fusion Kabini
#define INTERNAL_VGA_DID8             0x9838  // Fusion Kabini
#define INTERNAL_VGA_DID9             0x9839  // Fusion Kabini
#define INTERNAL_VGA_DID10            0x983A  // Fusion Kabini
#define INTERNAL_VGA_DID11            0x983B  // Fusion Kabini
#define INTERNAL_VGA_DID12            0x9855  // Mullines
#define INTERNAL_VGA_DID13            0x15DD  // Raven
#define INTERNAL_VGA_DID14            0x15D8  // Picasso


//******************************************************************************************
// MISC Devices BDF
//******************************************************************************************
//#define NB_BDF                              0
#define NB_EXT_BDF                          NB_BDF << 12

//#define APC_DEV                             1
//#define APC_BDF                             APC_DEV << 3
#define APC_EXT_BDF                         APC_BDF << 12

#define IGFX_DEV                            5
#define IGFX_BUS                            1
#define IGFX_BDF                            (IGFX_BUS << 8) + (IGFX_DEV << 3)
#define IGFX_EXT_BDF                        IGFX_BDF << 12

//#define CLK_BDF                             1
#define CLK_EXT_BDF                         CLK_BDF << 12

#define K8_LDT_BDF                          (0x18 << 3) + 0
#define K8_DDR_BDF                          (0x18 << 3) + 2
#define K8_MISC_BDF                         (0x18 << 3) + 3

//#define PCIE_CAP_ID                          10 
//******************************************************************************************
// PCIE Ports BDF
//******************************************************************************************

#define DEV8                                8 << 3
#define DEV8_EXT_BDF                        DEV8 << 12
#define DEV7                                7 << 3
#define DEV6                                6 << 3
#define DEV5                                5 << 3
#define DEV4                                4 << 3
#define DEV3                                3 << 3
#define DEV2                                2 << 3
#define DEV2_EXT_BDF                        DEV2 << 12
#define CLKDEV                              (0 << 3) + 1

//******************************************************************************************
// NBCFG register definition  NB Bus0 Dev0 Fun0 PCI config register
//******************************************************************************************
#define ATI_NB_REG04                             0x0004      //NB_COMMAND
#define ATI_NB_REG08                             0x0008      //NB_REVISION_ID
#define ATI_NB_REG10                             0x0010      
#define ATI_NB_REG18                             0x0018      //NB_BAR2_PM2
#define ATI_NB_REG19                             0x0019      //NB_BAR2_PM2
#define ATI_NB_REG1C                             0x001c      //NB_BAR3_PCIEXP_MMCFG
#define ATI_NB_REG4C                             0x004c      //NB_PCI_CTRL
#define ATI_NB_REG4E                             0x004e      //NB_PCI_CTRL
#define ATI_NB_REG50                             0x0050
#define ATI_NB_REG60                             0x0060      //NB_MISC_INDEX
#define ATI_NB_REG64                             0x0064      //NB_MISC_DATA
#define ATI_NB_REG70                             0x0070      //NB_MC_CNTL
#define ATI_NB_REG71                             0x0071      //NB_MC_CNTL
#define ATI_NB_REG7C                             0x007c      //NB_BIU_CFG
#define ATI_NB_REG7F                             0x007f      //NB_BIU_CFG
#define ATI_NB_REG80                             0x0080      //NB_HT_CLK_CNTL_RECEIVER_COMP_CNTL
#define ATI_NB_REG84                             0x0084      //NB_PCI_ARB
#define ATI_NB_REG85                             0x0085      //NB_PCI_ARB
#define ATI_NB_REG8C                             0x008C      //NB_GC_STRAPS
#define ATI_NB_REG90                             0x0090      //NB_TOP_OF_DRAM_SLOT1
#define ATI_NB_REG94                             0x0094      //NB_HT_TRANS_COMP_CNTL
#define ATI_NB_REG95                             0x0095      //NB_HT_TRANS_COMP_CNTL
#define ATI_NB_REG97                             0x0097      //NB_HT_TRANS_COMP_CNTL
#define ATI_NB_REG98                             0x0098      //SCRATCH_2_NBCFG
#define ATI_NB_REG9A                             0x009A      //SCRATCH_2_NBCFG
#define ATI_NB_REG9B                             0x009B      //SCRATCH_2_NBCFG
#define ATI_NB_REG9C                             0x009C      //NB_CFG_Q_F1000_800
#define ATI_NB_REGA8                             0x00A8      //NB_HTIU_INDEX
#define ATI_NB_REGAC                             0x00AC      //NB_HTIU_DATA
#define ATI_NB_REGC4                             0x00C4      //NB_HT_LINK_COMMAND
#define ATI_NB_REGC8                             0x00C8      //NB_HT_LINK_CONF_CNTL
#define ATI_NB_REGD0                             0x00D0      //NB_HT_LINK_FREQ_CAP_A
#define ATI_NB_REGE0                             0x00E0      //NB_PCIE_INDX_ADDR
#define ATI_NB_REGE4                             0x00E4      //NB_PCIE_INDX_DATA
#define ATI_NB_REGE8                             0x00E8      //NB_MC_INDX_ADDR
#define ATI_NB_REGEC                             0x00EC      //NB_MC_INDX_DATA
#define ATI_NB_REGFC                             0x00FC      //NB_AGP_MODE_CONTROL
//******************************************************************************************
// MISCIND/NBCFG register definition
//******************************************************************************************
#define ATI_MISC_REG00                           0x0000       //NB_CNTL
#define ATI_MISC_REG07                           0x0007       //PCIE_PDNB_CNTL
#define ATI_MISC_REG08                           0x0008       //PCIE_LINK_CFG
#define ATI_MISC_REG0B                           0x000B       //IOC_PCIE_CNTL
#define ATI_MISC_REG0C                           0x000C       //IOC_P2P_CNTL
#define ATI_MISC_REG20                           0x0020
#define ATI_MISC_REG25                           0x0025       //HTIU_DEBUG
#define ATI_MISC_REG26                           0x0026       //HTIU_DOWNSTREAM_CONFIG
#define ATI_MISC_REG27                           0x0027       //HTIU_UPSTREAM_CONFIG_0
#define ATI_MISC_REG31                           0x0031       //PCIE_STRAP_REG1
#define ATI_MISC_REG32                           0x0032 
#define ATI_MISC_REG33                           0x0033 
#define ATI_MISC_REG37                           0x0037       //PCIE_NBCFG_REG7
#define ATI_MISC_REG38                           0x0038       //PCIE_NBCFG_REG8
#define ATI_MISC_REG40                           0x0040
#define ATI_MISC_REG42                           0x0042
#define ATI_MISC_REG46                           0x0046       //HTIU_UPSTREAM_CONFIG_15
#define ATI_MISC_REG4E                           0x004E
#define ATI_MISC_REG4F                           0x004F
#define ATI_MISC_REG51                           0x0051
#define ATI_MISC_REG53                           0x0053
#define ATI_MISC_REG55                           0x0055
#define ATI_MISC_REG57                           0x0057
#define ATI_MISC_REG59                           0x0059
#define ATI_MISC_REG5B                           0x005B
#define ATI_MISC_REG67                           0x0067
#define ATI_MISC_REG6A                           0x006A

//******************************************************************************************
// HTIUNBIND register definition
//******************************************************************************************
#define ATI_HTIUNBIND_REG05                      0x0005 
#define ATI_HTIUNBIND_REG06                      0x0006 
#define ATI_HTIUNBIND_REG07                      0x0007
#define ATI_HTIUNBIND_REG16                      0X0016 
#define ATI_HTIUNBIND_REG12                      0x0012 
#define ATI_HTIUNBIND_REG1A                      0x001A 
#define ATI_HTIUNBIND_REG32                      0x0032 

//******************************************************************************************
// BIF_NBP/PCIEIND_P register definition
//******************************************************************************************
#define ATI_BIFNBP_REG02                         0x00200     //PCIEP_HW_DEBUG
#define ATI_BIFNBP_REG10                         0x01000     //PCIEP_PORT_CNTL
#define ATI_BIFNBP_REG20                         0x02000     //PCIE_TX_CNTL
#define ATI_BIFNBP_REG21                         0x02100     //PCIE_TX_REQUEST_NUM_CNTL
#define ATI_BIFNBP_REG70                         0x07000     //PCIE_RX_CNTL
#define ATI_BIFNBP_REGA0                         0x0a000     //PCIE_LC_CNTL
#define ATI_BIFNBP_REGA1                         0x0a100
#define ATI_BIFNBP_REGA2                         0x0a200     //PCIE_LC_LINK_WIDTH_CNTL
#define ATI_BIFNBP_REGA5                         0x0a500     //PCIE_LC_STATE0


//******************************************************************************************
// BIF_NB/PCIEIND register definition
//******************************************************************************************

#define ATI_BIFNB_REG02                          0x00200     //PCIE_HW_DEBUG
#define ATI_BIFNB_REG10                          0x01000     //PCIE_CNTL
#define ATI_BIFNB_REG11                          0x01100     //PCIE_CONFIG_CNTL
#define ATI_BIFNB_REG20                          0x02000     
#define ATI_BIFNB_REG21                          0x02100     //PCIE_BUS_CNTL
#define ATI_BIFNB_REG40                          0x04000     //PCIE_P_CNTL
#define ATI_BIFNB_REG64                          0x06400     //PCIE_P_PAD_FORCE_EN
#define ATI_BIFNB_REG65                          0x06500     //PCIE_P_PAD_FORCE_DIS

//******************************************************************************************
// Clock Congiguration register
//******************************************************************************************
#define ATI_CLK_REG4C                            0x004C
#define ATI_CLK_REG5C                            0x005C     
#define ATI_CLK_REG60                            0x0060      //SCRATCH Register
#define ATI_CLK_REG8C                            0x008C      //CLKGATE_DISABLE2
#define ATI_CLK_REG94                            0x0094      //CLKGATE_DISABLE
#define ATI_CLK_REGB0                            0x00B0
#define ATI_CLK_REGB4                            0x00B4
#define ATI_CLK_REGCC                            0x00CC     
#define ATI_CLK_REGD4                            0x00D4 
#define ATI_CLK_REGD5                            0x00D5      //CLK_CFG_HTPLL_CNTL
#define ATI_CLK_REGD6                            0x00D6      //CLK_CFG_HTPLL_CNTL
#define ATI_CLK_REGE0                            0x00E0      //CLK_TOP_SPARE_A
#define ATI_CLK_REGE4                            0x00E4     
#define ATI_CLK_REGE8                            0x00E8     
#define ATI_CLK_REGF0                            0x00F0       
#define ATI_CLK_REGF8                            0x00F8      //CLK_TOP_SPARE_C
#define ATI_CLK_REGF9                            0x00F9      //CLK_TOP_SPARE_C
#define ATI_CLK_REGFA                            0x00FA      //CLK_TOP_SPARE_C
#define ATI_CLK_REGFB                            0x00FB      //CLK_TOP_SPARE_C

//******************************************************************************************
// MC Congiguration register
//******************************************************************************************
#define ATI_MC_REG00                             0x0000      //MC_GENERAL_PURPOSE
#define ATI_MC_REG01                             0x0001      //MC_IN_TIMING_A
#define ATI_MC_REG02                             0x0002      //MC_OUT_TIMING_A
#define ATI_MC_REG03                             0x0003      //MC_OUT_DELAY1_A
#define ATI_MC_REG04                             0x0004      //MC_OUT_DELAY2_A
#define ATI_MC_REG07                             0x0007      //MC_IN_DELAY3_A
#define ATI_MC_REG0F                             0x000f      //MC_PLL_IO_CNTL
#define ATI_MC_REG11                             0x0011 
#define ATI_MC_REG12                             0x0012      //MC_IMP_CTRL_STRENGTH_A
#define ATI_MC_REG18                             0x0018      //MC_MISC_CNTL
#define ATI_MC_REG19                             0x0019      //NB_MC_TEST_DEBUG
#define ATI_MC_REG1A                             0x001A      //NB_MEM_SIZE_A
#define ATI_MC_REG1B                             0x001B      
#define ATI_MC_REG1C                             0x001C      //NB_MEM_CNTL0
#define ATI_MC_REG1E                             0x001E      //K8_FB_LOCATION
#define ATI_MC_REG1D                             0x001D      //NB_EXT_MEM_CNTL
#define ATI_MC_REG22                             0x0022      //MC0_PERF_CNTL
#define ATI_MC_REG25                             0x0025      //MC1_PERF_CNTL
#define ATI_MC_REG26                             0x0026      //MC_PM_CNTL
#define ATI_MC_REG2B                             0x002B      //GART_FEATURE_ID
#define ATI_MC_REG2C                             0x002C      //GART_BASE
#define ATI_MC_REG2E                             0x002E      //GART_CACHE_CNTRL
#define ATI_MC_REG2F                             0x002F      //GART_CACHE_ENTRY_CNTRL
#define ATI_MC_REG38                             0x0038      //AGP_ADDRESS_SPACE_SIZE
#define ATI_MC_REG39                             0x0039      //AGP_MODE_CONTROL
#define ATI_MC_REG3A                             0x003A      //AIC_CTRL_SCRATCH
#define ATI_MC_REG3B                             0x003B      //MC_GART_ERROR_ADDRESS
#define ATI_MC_REG3C                             0x003c      //MC_GART_ERROR_ADDRESS_HI
#define ATI_MC_REG40                             0x0040      //MC_OCD_ENABLE
#define ATI_MC_REG45                             0x0045      //NB_MEM_CONFIG_PLL_AND_MISC
#define ATI_MC_REG47                             0x0047      //NB_EXT_MEM_CNTL_DDR2
#define ATI_MC_REG48                             0x0048      //NB_MEM_ODT_EN
#define ATI_MC_REG49                             0x0049      //NB_MEMA_STB
#define ATI_MC_REG4E                             0x004e      //MC_MISC_CNTL2
#define ATI_MC_REG4F                             0x004F      //MC_MISC_CNTL3
#define ATI_MC_REG5A                             0x005A      //MC_PA_IO_CNTL1_A
#define ATI_MC_REG5B                             0x005B      //MC_PA_IO_CNTL2_A
#define ATI_MC_REG5F                             0x005F      //MC_MISC_UMA_CNTL
#define ATI_MC_REG63                             0x0063      //K8_DRAM_CS0_BASE
#define ATI_MC_REG73                             0x0073      //K8_DRAM_BANK_ADDR_MAPPING
#define ATI_MC_REG74                             0x0074      //MC_MPLL_CONTROL
#define ATI_MC_REG75                             0x0075      //MC_MPLL_CONTROL2
#define ATI_MC_REG76                             0x0076      //MC_MPLL_CONTROL3
#define ATI_MC_REG77                             0x0077 
#define ATI_MC_REG78                             0x0078      //MC_MPLL_SEQ_CONTROL
#define ATI_MC_REG79                             0x0079 
#define ATI_MC_REG7A                             0x007A      //MC_MCLK_CONTROL
#define ATI_MC_REG7B                             0x007B 
#define ATI_MC_REG7C                             0x007C      //MC_UMA_GRP_CNTL
#define ATI_MC_REG7D                             0x007D      //MC_UMA_GRP_TMR
#define ATI_MC_REG7E                             0x007E      //MC_MISC_UMA_CNTL2
#define ATI_MC_REG80                             0x0080 
#define ATI_MC_REG81                             0x0081 
#define ATI_MC_REG82                             0x0082 
#define ATI_MC_REG83                             0x0083 
#define ATI_MC_REG84                             0x0084 
#define ATI_MC_REG85                             0x0084 
#define ATI_MC_REG86                             0x0086 
#define ATI_MC_REG90                             0x0090 
#define ATI_MC_REG91                             0x0091 
#define ATI_MC_REG92                             0x0092 
#define ATI_MC_REG93                             0x0093 
#define ATI_MC_REG94                             0x0094 
#define ATI_MC_REG95                             0x0095 
#define ATI_MC_REG96                             0x0096 
#define ATI_MC_REG97                             0x0097 
#define ATI_MC_REG98                             0x0098 
#define ATI_MC_REG99                             0x0099 
#define ATI_MC_REGA0                             0x00A0 
#define ATI_MC_REGA1                             0x00A1 
#define ATI_MC_REGA2                             0x00A2 
#define ATI_MC_REGA3                             0x00A3 
#define ATI_MC_REGA4                             0x00A4 
#define ATI_MC_REGA5                             0x00A5 
#define ATI_MC_REGA6                             0x00A6 
#define ATI_MC_REGA7                             0x00A7 
#define ATI_MC_REGA8                             0x00A8 
#define ATI_MC_REGA9                             0x00A9 
#define ATI_MC_REGAA                             0x00AA 
#define ATI_MC_REGAB                             0x00AB 
#define ATI_MC_REGAC                             0x00AC 
#define ATI_MC_REGAD                             0x00AD 
#define ATI_MC_REGAE                             0x00AE 
#define ATI_MC_REGAF                             0x00AF 
#define ATI_MC_REGB0                             0x00B0 
#define ATI_MC_REGB1                             0x00B1 
#define ATI_MC_REGB2                             0x00B2 
#define ATI_MC_REGB3                             0x00B3 
#define ATI_MC_REGB4                             0x00B4 
#define ATI_MC_REGB5                             0x00B5 
#define ATI_MC_REGB6                             0x00B6 
#define ATI_MC_REGB7                             0x00B7 
#define ATI_MC_REGB8                             0x00B8 
#define ATI_MC_REGB9                             0x00B9 
#define ATI_MC_REGBA                             0x00BA 
#define ATI_MC_REGBB                             0x00BB 
#define ATI_MC_REGBC                             0x00BC 
#define ATI_MC_REGBE                             0x00BE 
#define ATI_MC_REGBF                             0x00BF 
#define ATI_MC_REGC0                             0x00C0 
#define ATI_MC_REGC1                             0x00C1 
#define ATI_MC_REGC2                             0x00C2 
#define ATI_MC_REGC3                             0x00C3 
#define ATI_MC_REGC4                             0x00C4 
#define ATI_MC_REGC5                             0x00C5 
#define ATI_MC_REGC6                             0x00C6 
#define ATI_MC_REGC8                             0x00C8 
#define ATI_MC_REGC9                             0x00C9 
#define ATI_MC_REGCA                             0x00CA 
#define ATI_MC_REGCB                             0x00CB 
#define ATI_MC_REGCC                             0x00CC 
#define ATI_MC_REGCD                             0x00CD 
#define ATI_MC_REGCE                             0x00CE 
#define ATI_MC_REGCF                             0x00CF 
#define ATI_MC_REGD0                             0x00D0 
#define ATI_MC_REGD1                             0x00D1 
#define ATI_MC_REGD2                             0x00D2 
#define ATI_MC_REGD3                             0x00D3 
#define ATI_MC_REGD4                             0x00D4 
#define ATI_MC_REGD5                             0x00D5 
#define ATI_MC_REGD6                             0x00D6 
#define ATI_MC_REGD7                             0x00D7 
#define ATI_MC_REGD8                             0x00D8 
#define ATI_MC_REGD9                             0x00D9 
#define ATI_MC_REGE0                             0x00E0 
#define ATI_MC_REGE1                             0x00E1 
#define ATI_MC_REGE8                             0x00E8 
#define ATI_MC_REGE9                             0x00E9 
#define ATI_MC_REGF0                             0x00F0 
#define ATI_MC_REGF1                             0x00F1 
#define ATI_MC_REGF2                             0x00F2 
#define ATI_MC_REGF3                             0x00F3 
#define ATI_MC_REGF4                             0x00F4 
#define ATI_MC_REGF5                             0x00F5 
#define ATI_MC_REGF6                             0x00F6 
#define ATI_MC_REGF7                             0x00F7 
#define ATI_MC_REGF8                             0x00F8 
#define ATI_MC_REGF9                             0x00F9 
#define ATI_MC_REGFA                             0x00Fa 
#define ATI_MC_REGFB                             0x00Fb 
#define ATI_MC_REGFC                             0x00Fc 
#define ATI_MC_REGFD                             0x00Fd 
#define ATI_MC_REGFE                             0x00Fe 
#define ATI_MC_REGFF                             0x00Ff 
#define ATI_MC_REG100                            0x0100 
#define ATI_MC_REG104                            0x0104 
#define ATI_MC_REG105                            0x0105 
#define ATI_MC_REG106                            0x0106 
#define ATI_MC_REG107                            0x0107 
#define ATI_MC_REG108                            0x0108 
#define ATI_MC_REG109                            0x0109 
#define ATI_MC_REG10A                            0x010a 
#define ATI_MC_REG110                            0x0110 
#define ATI_MC_REG111                            0x0111 
#define ATI_MC_REG112                            0x0112 
#define ATI_MC_REG113                            0x0113 
#define ATI_MC_REG114                            0x0114 
#define ATI_MC_REG115                            0x0115 
#define ATI_MC_REG116                            0x0116 
#define ATI_MC_REG117                            0x0117 
#define ATI_MC_REG118                            0x0118 
#define ATI_MC_REG119                            0x0119 
#define ATI_MC_REG11A                            0x011A 
#define ATI_MC_REG11B                            0x011B 
#define ATI_MC_REG11D                            0x011D 
#define ATI_MC_REG11E                            0x011e 
#define ATI_MC_REG11F                            0x011F 
#define ATI_MC_REG120                            0x0120 
#define ATI_MC_REG121                            0x0121 
#define ATI_MC_REG122                            0x0122 
#define ATI_MC_REG123                            0x0123 
#define ATI_MC_REG124                            0x0124 
                               
//******************************************************************************************
// GFX PCI Configuration register
//******************************************************************************************
#define ATI_GFX_REG18                            0x018 

//******************************************************************************************
// GFX MMREG Configuration register
//******************************************************************************************
#define ATI_MMREG_REG130                         0x0130      //HOST_PATH_CNTL
#define ATI_MMREG_REG150                         0x0150      //MC_STATUS
#define ATI_MMREG_REG158                         0x0158      //MEM_SDRAM_MODE_REG
#define ATI_MMREG_REG15C                         0x015C      //GC_NB_TOM
#define ATI_MMREG_REG178                         0x0178      //MC_CNTL
//******************************************************************************************
// K8 Configuration register
//******************************************************************************************
#define K8_LDT_REG68                             0x00068 
#define K8_LDT_REG88                             0x00088 
#define K8_LDT_REG84                             0x00084 
#define K8_LDT_REG90                             0x00090 
                                              
#define K8_MC_REG40                              0x00040 
#define K8_MC_REG80                              0x00080 
#define K8_MC_REG90                              0x00090 
#define K8_MC_REG94                              0x00094 
#define K8_MC_REGA0                              0x000A0 
                                           
#define K8_MISC_REG44                            0x00044 
#define K8_MISC_REG70                            0x00070 
#define K8_MISC_REG74                            0x00074 
#define K8_MISC_REG78                            0x00078 
#define K8_MISC_REG7C                            0x0007C 
#define K8_MISC_REG80                            0x00080 
#define K8_MISC_REG84                            0x00084 
#define K8_MISC_REGDC                            0x000DC 
#define K8_MISC_REGE8                            0x000e8 
//******************************************************************************************
// K8 MISC
//******************************************************************************************
#define K8_REV_Cx                                5
#define K8_REV_Dx                                7
#define K8_REV_Ex                                8
#define K8_REV_Fx                                128


#define K8_PMM4567_DESKTOP                       0x021132113 
#define K8_PMM0123_DESKTOP                       0x023070000 
#define K8_PMM4567_MOBILE                        0x061134113 
#define K8_PMM0123_MOBILE                        0x033070031 

#define C2_ENABLED                               BIT2
#define C3_ENABLED                               BIT3
//******************************************************************************************
// GFX configuration params
//******************************************************************************************

#endif
