/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX PSP Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _CCX_PSP_LIB_H_
#define _CCX_PSP_LIB_H_

/*---------------------------------------------------------------------------------------*/
 /**
  *
  * Programs the Psp Address register to the desired value.
  *
  * @param[in]      PspAddr                Value written to Psp Bar address
  *
  */
VOID
UpdatePspAddr (
  IN       UINT64 PspAddr
  );

/*---------------------------------------------------------------------------------------*/
 /**
  *
  * Programs the S3 Resume register with the desired value.
  *
  * @param[in]      ResumeVector            Value written to S3 Resume register
  *
  */

VOID
SetupReleaseVector (
  IN OUT   VOID *ResumeVector
  );

#endif // _CCX_PSP_LIB_H_

