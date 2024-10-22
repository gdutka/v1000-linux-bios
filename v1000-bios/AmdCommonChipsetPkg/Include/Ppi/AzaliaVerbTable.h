/** @file


  Cache PPI definition.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#ifndef _PEI_INSTALL_AZALIA_VERB_TALBE_PPI_H_
#define _PEI_INSTALL_AZALIA_VERB_TALBE_PPI_H_

#define PEI_INSTALL_AZALIA_VERB_TALBE_PPI_GUID \
  { \
    0x0BA3B847, 0xA82D, 0x4B51, {0x9A, 0xE5, 0x30, 0xDA, 0x02, 0xC4, 0x9E, 0xC3 } \
  }

typedef
EFI_STATUS
(EFIAPI *INSTALL_AZALIA_VERB_TABLE_FUNCTION) (
  IN CONST EFI_PEI_SERVICES                **PeiServices,
  IN UINT32                                 HdaBar
  );
//typedef struct _PEI_Install_AZALIA_VERB_TALBE_PPI;

//
// PEI_CACHE_PPI
//
typedef struct _PEI_Install_AZALIA_VERB_TALBE_PPI {
  INSTALL_AZALIA_VERB_TABLE_FUNCTION              InstallAzaliaVerbTable;
} PEI_Install_AZALIA_VERB_TALBE_PPI;

extern EFI_GUID gPeiInstallAzaliaVerbTablePpiGuid;

#endif
