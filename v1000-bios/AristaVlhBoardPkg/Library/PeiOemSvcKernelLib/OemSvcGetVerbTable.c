/** @file
  This OemService is part of setting Verb Table. The function is created for setting verb table 
  to support Multi-Sku and return the table to common code to program.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/PeiOemSvcKernelLib.h>
#include <Ppi/VerbTable.h>
#include <MultiSkuInfoCommonDef.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sample :                                                                                                      //
//                                                                                                               //
//   #define OEM_VERB_TABLE_ID_(n)         n                                                                     //
//                                                                                                               //
//   #define OEM_VERB_TABLE_(n)_HEADER(a)  0x10EC0880, // Vendor ID / Device ID                                  //
//                                         0x00000000, // SubSystem ID                                           //
//                                         0x02,       // RevisionId                                             //
//                                         0x01,       // Front panel support ( 1 = Yes, 2 = No )                //
//                                         0x000A,     // Number of Rear Jacks = 10                              //
//                                         0x0002      // Number of Front Jacks = 2                              //
//                                                                                                               //
//   #define OEM_VERB_TABLE_(n)_DATA(a)    0x01171CF0,0x01171D11,0x01171E11,0x01171F41, // NID(0x11): 0x411111F0 //
//                                         0x01271C40,0x01271D09,0x01271EA3,0x01271F99, // NID(0x12): 0x99A30940 //
//                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// To define Verb Table ID.
//
#define OEM_VERB_TABLE_ID_2         2

//
// To define verb table here.
//
#define OEM_VERB_TABLE_2_HEADER1    0x10EC0286, \
                                    0x1022C751, \
                                    0xFF,       \
                                    0x02,       \
                                    0x0011,     \
                                    0x0000

#define OEM_VERB_TABLE_2_DATA1      0x00172051, 0x001721C7, 0x00172222, 0x00172310, \
                                    0x0017FF00, 0x0017FF00, 0x0017FF00, 0x0017FF00, \
                                    0x01271C50, 0x01271D01, 0x01271EA6, 0x01271FB7, \
                                    0x01371C00, 0x01371D00, 0x01371E00, 0x01371F40, \
                                    0x01471C10, 0x01471D01, 0x01471E17, 0x01471F90, \
                                    0x01771CF0, 0x01771D11, 0x01771E11, 0x01771F41, \
                                    0x01871C40, 0x01871D10, 0x01871EA1, 0x01871F04, \
                                    0x01971CF0, 0x01971D11, 0x01971E11, 0x01971F41, \
                                    0x01A71CF0, 0x01A71D11, 0x01A71E11, 0x01A71F41, \
                                    0x01D71C2D, 0x01D71DA5, 0x01D71E67, 0x01D71F40, \
                                    0x01E71C30, 0x01E71D11, 0x01E71E45, 0x01E71F04, \
                                    0x02171C20, 0x02171D10, 0x02171E21, 0x02171F04, \
                                    0x02050071, 0x02040014, 0x02050010, 0x02040C22, \
                                    0x0205004F, 0x0204B029, 0x0205002B, 0x02040C50, \
                                    0x0205002D, 0x02041020, 0x02050020, 0x02040000, \
                                    0x02050019, 0x02040817, 0x02050035, 0x02041AA5, \
                                    0x02050063, 0x02042906, 0x02050063, 0x02042906

//For ALC 288
#define OEM_VERB_TABLE_2_HEADER2    0x10EC0288, \
                                    0x1022C751, \
                                    0xFF,       \
                                    0x02,       \
                                    0x0011,     \
                                    0x0000

#define OEM_VERB_TABLE_2_DATA2      0x00172051, 0x001721C7, 0x00172222, 0x00172310, \
                                    0x0017FF00, 0x0017FF00, 0x0017FF00, 0x0017FF00, \
                                    0x01271C50, 0x01271D01, 0x01271EA6, 0x01271FB7, \
                                    0x01371C00, 0x01371D00, 0x01371E00, 0x01371F40, \
                                    0x01471C10, 0x01471D01, 0x01471E17, 0x01471F90, \
                                    0x01771CF0, 0x01771D11, 0x01771E11, 0x01771F41, \
                                    0x01871C40, 0x01871D10, 0x01871EA1, 0x01871F04, \
                                    0x01971CF0, 0x01971D11, 0x01971E11, 0x01971F41, \
                                    0x01A71CF0, 0x01A71D11, 0x01A71E11, 0x01A71F41, \
                                    0x01D71C2D, 0x01D71DA5, 0x01D71E67, 0x01D71F40, \
                                    0x01E71C30, 0x01E71D11, 0x01E71E45, 0x01E71F04, \
                                    0x02171C20, 0x02171D10, 0x02171E21, 0x02171F04, \
                                    0x02050071, 0x02040014, 0x02050010, 0x02040C22, \
                                    0x0205004F, 0x0204B029, 0x0205002B, 0x02040C50, \
                                    0x0205002D, 0x02041020, 0x02050020, 0x02040000, \
                                    0x02050019, 0x02040817, 0x02050035, 0x02041AA5, \
                                    0x02050063, 0x02042906, 0x02050063, 0x02042906

//
// To include verb table 1 with one codec header/data
// To congregate the verb table header/data.
//
#ifdef OEM_VERB_TABLE_ID_2
DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_2(OEM_VERB_TABLE_ID_2);
COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_2(OEM_VERB_TABLE_ID_2);
#endif

/**
  This OemService is part of setting Verb Table. The function is created for setting verb table 
  to support Multi-Sku and return the table to common code to program.

  @param[out]  *VerbTableHeaderDataAddress    A pointer to VerbTable data/header

  @retval      EFI_UNSUPPORTED                Returns unsupported by default.
  @retval      EFI_SUCCESS                    The service is customized in the project.
  @retval      EFI_MEDIA_CHANGED              The value of IN OUT parameter is changed. 
  @retval      Others                         Depends on customization.
**/
EFI_STATUS
OemSvcGetVerbTable (
  OUT COMMON_CHIPSET_AZALIA_VERB_TABLE      **VerbTableHeaderDataAddress
  )
{
  *VerbTableHeaderDataAddress = VERB_TABLE_HEADER_DATA_BUFFER_ADDRESS(OEM_VERB_TABLE_ID_2);

  return EFI_MEDIA_CHANGED;
}
