/* $NoKeywords:$ */
/**
 * @file
 *
 * AGESA gnb file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _GNBURATOKEN_H_
#define _GNBURATOKEN_H_

/// Template structure for register/field table
typedef struct {
  URA_REGISTER_64B_ENTRY                                        RxSmuIntReq;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuIntToggle;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuServiceIndex;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuIntSts;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuIntAck;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuIntDone;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuAuthSts;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuAuthDone;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuAuthPass;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuFwAuth;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuProtectedMode;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuRcuUcEvents;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuBootSeqDone;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuFwFlags;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuInterruptsEnabled;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuResetCntl;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuRstReg;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuClkCntl;///<
  URA_FIELD_16B_ENTRY                                           BfxSmuCkDisable;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuAuthVector;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuRamStartAddr;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuRomStartAddr;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuIntArgument;///<

} REG_FIELD_TABLE_STRUCT;

#define  IDX_CALC(VarName) (offsetof (REG_FIELD_TABLE_STRUCT, VarName) / 2)
#define  SEL_CALC(VarName) VarName
#define  FIELD_OFFSET(RegName, FieldName)  ((((IDX_CALC(FieldName) - IDX_CALC(RegName) - 1) / 2) <= 15) ? ((IDX_CALC(FieldName) - IDX_CALC(RegName) - 1) / 2) : 0)

#define  TRxSmuIntReq                                           TOKEN_DEF (IDX_CALC (RxSmuIntReq), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuIntToggle                                       TOKEN_DEF (IDX_CALC (BfxSmuIntToggle), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuServiceIndex                                    TOKEN_DEF (IDX_CALC (BfxSmuServiceIndex), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuIntSts                                           TOKEN_DEF (IDX_CALC (RxSmuIntSts), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuIntAck                                          TOKEN_DEF (IDX_CALC (BfxSmuIntAck), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuIntDone                                         TOKEN_DEF (IDX_CALC (BfxSmuIntDone), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuAuthSts                                          TOKEN_DEF (IDX_CALC (RxSmuAuthSts), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuAuthDone                                        TOKEN_DEF (IDX_CALC (BfxSmuAuthDone), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuAuthPass                                        TOKEN_DEF (IDX_CALC (BfxSmuAuthPass), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuFwAuth                                           TOKEN_DEF (IDX_CALC (RxSmuFwAuth), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuProtectedMode                                   TOKEN_DEF (IDX_CALC (BfxSmuProtectedMode), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuRcuUcEvents                                      TOKEN_DEF (IDX_CALC (RxSmuRcuUcEvents), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuBootSeqDone                                     TOKEN_DEF (IDX_CALC (BfxSmuBootSeqDone), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuFwFlags                                          TOKEN_DEF (IDX_CALC (RxSmuFwFlags), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_PROTOCOL_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuInterruptsEnabled                               TOKEN_DEF (IDX_CALC (BfxSmuInterruptsEnabled), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_PROTOCOL_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuResetCntl                                        TOKEN_DEF (IDX_CALC (RxSmuResetCntl), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuRstReg                                          TOKEN_DEF (IDX_CALC (BfxSmuRstReg), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuClkCntl                                          TOKEN_DEF (IDX_CALC (RxSmuClkCntl), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TBfxSmuCkDisable                                       TOKEN_DEF (IDX_CALC (BfxSmuCkDisable), URA_TYPE_FIELD_16, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuAuthVector                                       TOKEN_DEF (IDX_CALC (RxSmuAuthVector), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuRamStartAddr                                     TOKEN_DEF (IDX_CALC (RxSmuRamStartAddr), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuRomStartAddr                                     TOKEN_DEF (IDX_CALC (RxSmuRomStartAddr), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuIntArgument                                      TOKEN_DEF (IDX_CALC (RxSmuIntArgument), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)


/// Template structure for register/field table
typedef struct {
  URA_REGISTER_64B_ENTRY                                        RxSmuMessageId;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuMessageResponse;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuMessageArgument0;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuMessageArgument1;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuMessageArgument2;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuMessageArgument3;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuMessageArgument4;///<
  URA_REGISTER_64B_ENTRY                                        RxSmuMessageArgument5;///<

} REG_FIELD_TABLE_STRUCT_V8;

#define  IDX_CALC_V8(VarName) (offsetof (REG_FIELD_TABLE_STRUCT_V8, VarName) / 2)

#define  TRxSmuMessageId                                        TOKEN_DEF (IDX_CALC_V8 (RxSmuMessageId), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuMessageResponse                                  TOKEN_DEF (IDX_CALC_V8 (RxSmuMessageResponse), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuMessageArgument0                                 TOKEN_DEF (IDX_CALC_V8 (RxSmuMessageArgument0), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuMessageArgument1                                 TOKEN_DEF (IDX_CALC_V8 (RxSmuMessageArgument1), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuMessageArgument2                                 TOKEN_DEF (IDX_CALC_V8 (RxSmuMessageArgument2), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuMessageArgument3                                 TOKEN_DEF (IDX_CALC_V8 (RxSmuMessageArgument3), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuMessageArgument4                                 TOKEN_DEF (IDX_CALC_V8 (RxSmuMessageArgument4), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)
#define  TRxSmuMessageArgument5                                 TOKEN_DEF (IDX_CALC_V8 (RxSmuMessageArgument5), URA_TYPE_REGISTER_64, SEL_CALC (TYPE_GNB_INDIRECT_ACCESS), URA_TOKEN_PARENT_TYPE_64)

#endif


