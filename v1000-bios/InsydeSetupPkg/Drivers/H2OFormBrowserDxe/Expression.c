/** @file

 Utility functions for expression evaluation.

;******************************************************************************
;* Copyright (c) 2013 - 2020, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

Copyright (c) 2007 - 2012, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalH2OFormBrowser.h"

//
// Global stack used to evaluate boolean expresions
//
EFI_HII_VALUE *mOpCodeScopeStack = NULL;
EFI_HII_VALUE *mOpCodeScopeStackEnd = NULL;
EFI_HII_VALUE *mOpCodeScopeStackPointer = NULL;

EFI_HII_VALUE *mExpressionEvaluationStack = NULL;
EFI_HII_VALUE *mExpressionEvaluationStackEnd = NULL;
EFI_HII_VALUE *mExpressionEvaluationStackPointer = NULL;
UINTN         mExpressionEvaluationStackOffset = 0;

EFI_HII_VALUE *mCurrentExpressionStack = NULL;
EFI_HII_VALUE *mCurrentExpressionEnd = NULL;
EFI_HII_VALUE *mCurrentExpressionPointer = NULL;

EFI_HII_VALUE *mMapExpressionListStack = NULL;
EFI_HII_VALUE *mMapExpressionListEnd = NULL;
EFI_HII_VALUE *mMapExpressionListPointer = NULL;

FORM_EXPRESSION   **mFormExpressionStack = NULL;
FORM_EXPRESSION   **mFormExpressionEnd = NULL;
FORM_EXPRESSION   **mFormExpressionPointer = NULL;

FORM_EXPRESSION   **mStatementExpressionStack = NULL;
FORM_EXPRESSION   **mStatementExpressionEnd = NULL;
FORM_EXPRESSION   **mStatementExpressionPointer = NULL;

FORM_EXPRESSION   **mOptionExpressionStack = NULL;
FORM_EXPRESSION   **mOptionExpressionEnd = NULL;
FORM_EXPRESSION   **mOptionExpressionPointer = NULL;


//
// Unicode collation protocol interface
//
EFI_UNICODE_COLLATION_PROTOCOL *mUnicodeCollation = NULL;
EFI_USER_MANAGER_PROTOCOL      *mUserManager = NULL;

extern EFI_GUID                mH2OIfrExtGuid;

EFI_GET_PCD_INFO_PROTOCOL     *mIfrPiPcdInfo = NULL;


/**
  Process OpCodeH2oPcd to get Pcd value from PcdDatabase

  @param [in] *OpCodeData     Point to OpCodeData.
  @param [in] *PcdInfo        Point to H2O_IFR_GUID_PCD structure
  @param [in] *Value          Point to value that store Pcd value 

  @retval EFI_SUCCESS if Pcd value find in PcdDatabase by PcdGuid and PcdTokenNumber.
  @retval EFI_UNSUPPORTED if Pcd not exist in PcdDatabase by PcdGuid and PcdTokenNumber.
**/
STATIC
EFI_STATUS
ProcessOpCodeH2OPcd (
  IN UINT8                *OpCodeData,
  IN H2O_IFR_GUID_PCD     *PcdInfo,
  IN EFI_HII_VALUE        *Value
 )
{
  VOID                          *DataPtr;
  EFI_STATUS                    Status;
  EFI_PCD_INFO                  PcdInformation;


  DataPtr = NULL;
  Status = EFI_SUCCESS;

  ZeroMem (&PcdInformation,sizeof(EFI_PCD_INFO));
  CopyMem (&PcdInfo->PcdTokenNumber, &(( H2O_IFR_GUID_PCD *) OpCodeData)->PcdTokenNumber, sizeof (UINT32));

  if (mIfrPiPcdInfo == NULL) {
    Status = gBS->LocateProtocol (&gEfiGetPcdInfoProtocolGuid, NULL, (VOID **)&mIfrPiPcdInfo);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    
    Status = mIfrPiPcdInfo->GetInfo (&((( H2O_IFR_GUID_PCD *)OpCodeData)->PcdGuid),PcdInfo->PcdTokenNumber, &(PcdInformation));
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "The PCD is unsupported -- PCD GUID = %g, TokenNumber = %x\n",((( H2O_IFR_GUID_PCD *) OpCodeData)->PcdGuid),PcdInfo->PcdTokenNumber));                       
      return Status;            
    }
  }  

  switch(PcdInformation.PcdType) {
    case EFI_PCD_TYPE_8:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
      Value->Value.u8 = LibPcdGetEx8(&((( H2O_IFR_GUID_PCD *) OpCodeData)->PcdGuid),PcdInfo->PcdTokenNumber);                
      break;
    case EFI_PCD_TYPE_16:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_16;
      Value->Value.u16 = LibPcdGetEx16(&((( H2O_IFR_GUID_PCD *) OpCodeData)->PcdGuid),PcdInfo->PcdTokenNumber);   
      break;
    case EFI_PCD_TYPE_32:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_32;
      Value->Value.u32 = LibPcdGetEx32(&((( H2O_IFR_GUID_PCD *) OpCodeData)->PcdGuid),PcdInfo->PcdTokenNumber);                
      break;
    case EFI_PCD_TYPE_64:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
      Value->Value.u64 = LibPcdGetEx64(&((( H2O_IFR_GUID_PCD *) OpCodeData)->PcdGuid),PcdInfo->PcdTokenNumber);                
      break;
    case EFI_PCD_TYPE_BOOL:
      Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
      Value->Value.b = LibPcdGetExBool(&((( H2O_IFR_GUID_PCD *) OpCodeData)->PcdGuid),PcdInfo->PcdTokenNumber);                
      break;
    case EFI_PCD_TYPE_PTR:        
      Value->Type = EFI_IFR_TYPE_BUFFER;
      DataPtr = LibPcdGetExPtr(&((( H2O_IFR_GUID_PCD *) OpCodeData)->PcdGuid),PcdInfo->PcdTokenNumber);   
      Value->Buffer = AllocateCopyPool(PcdInformation.PcdSize, DataPtr);            
      Value->BufferLen = (UINT16) PcdInformation.PcdSize;        
      break;            
    default:
      Value->Type = EFI_IFR_TYPE_UNDEFINED;                                      
      break;        
  }      
  return Status;
}


/**
  Grow size of the stack.

  This is an internal function.

  @param  [in,out] Stack         On input: old stack; On output: new stack
  @param  [in,out] StackPtr      On input: old stack pointer; On output: new stack
                                 pointer
  @param  [in,out] StackEnd      On input: old stack end; On output: new stack end

  @retval EFI_SUCCESS            Grow stack success.
  @retval EFI_OUT_OF_RESOURCES   No enough memory for stack space.

**/
STATIC
EFI_STATUS
GrowStack (
  IN OUT EFI_HII_VALUE  **Stack,
  IN OUT EFI_HII_VALUE  **StackPtr,
  IN OUT EFI_HII_VALUE  **StackEnd
  )
{
  UINTN           Size;
  EFI_HII_VALUE  *NewStack;

  Size = EXPRESSION_STACK_SIZE_INCREMENT;
  if (*StackPtr != NULL) {
    Size = Size + (*StackEnd - *Stack);
  }

  NewStack = AllocatePool (Size * sizeof (EFI_HII_VALUE));
  if (NewStack == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (*StackPtr != NULL) {
    //
    // Copy from Old Stack to the New Stack
    //
    CopyMem (
      NewStack,
      *Stack,
      (*StackEnd - *Stack) * sizeof (EFI_HII_VALUE)
      );

    //
    // Free The Old Stack
    //
    FreePool (*Stack);
  }

  //
  // Make the Stack pointer point to the old data in the new stack
  //
  *StackPtr = NewStack + (*StackPtr - *Stack);
  *Stack    = NewStack;
  *StackEnd = NewStack + Size;

  return EFI_SUCCESS;
}


/**
  Push an element onto the Boolean Stack.

  @param  [in,out] Stack         On input: old stack; On output: new stack
  @param  [in,out] StackPtr      On input: old stack pointer; On output: new stack
                                 pointer
  @param  [in,out] StackEnd      On input: old stack end; On output: new stack end
  @param  Data                   Data to push.

  @retval EFI_SUCCESS            Push stack success.

**/
STATIC
EFI_STATUS
PushStack (
  IN OUT EFI_HII_VALUE       **Stack,
  IN OUT EFI_HII_VALUE       **StackPtr,
  IN OUT EFI_HII_VALUE       **StackEnd,
  IN EFI_HII_VALUE           *Data
  )
{
  EFI_STATUS  Status;

  //
  // Check for a stack overflow condition
  //
  if (*StackPtr >= *StackEnd) {
    //
    // Grow the stack
    //
    Status = GrowStack (Stack, StackPtr, StackEnd);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Push the item onto the stack
  //
  CopyMem (*StackPtr, Data, sizeof (EFI_HII_VALUE));
  if (Data->Type == EFI_IFR_TYPE_BUFFER) {
    (*StackPtr)->Buffer = AllocateCopyPool(Data->BufferLen, Data->Buffer);
    ASSERT ((*StackPtr)->Buffer != NULL);
  }

  *StackPtr = *StackPtr + 1;

  return EFI_SUCCESS;
}


/**
  Pop an element from the stack.

  @param  [in]     Stack         On input: old stack
  @param  [in,out] StackPtr      On input: old stack pointer; On output: new stack pointer
  @param  [out]    Data          Data to pop.

  @retval EFI_SUCCESS            The value was popped onto the stack.
  @retval EFI_ACCESS_DENIED      The pop operation underflowed the stack

**/
STATIC
EFI_STATUS
PopStack (
  IN  EFI_HII_VALUE          *Stack,
  IN OUT EFI_HII_VALUE       **StackPtr,
  OUT EFI_HII_VALUE          *Data
  )
{
  //
  // Check for a stack underflow condition
  //
  if (*StackPtr == Stack) {
    return EFI_ACCESS_DENIED;
  }

  //
  // Pop the item off the stack
  //
  *StackPtr = *StackPtr - 1;
  CopyMem (Data, *StackPtr, sizeof (EFI_HII_VALUE));
  return EFI_SUCCESS;
}


/**
  Reset stack pointer to begin of the stack.

**/
VOID
ResetCurrentExpressionStack (
  VOID
  )
{
  mCurrentExpressionPointer   = mCurrentExpressionStack;
  mFormExpressionPointer      = mFormExpressionStack;
  mStatementExpressionPointer = mStatementExpressionStack;
  mOptionExpressionPointer    = mOptionExpressionStack;
}


/**
  Push current expression onto the Stack

  @param  [in] Pointer           Pointer to current expression.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PushCurrentExpression (
  IN VOID  *Pointer
  )
{
  EFI_HII_VALUE  Data;

  Data.Type = EFI_IFR_TYPE_NUM_SIZE_64;
  Data.Value.u64 = (UINT64) (UINTN) Pointer;

  return PushStack (
    &mCurrentExpressionStack,
    &mCurrentExpressionPointer,
    &mCurrentExpressionEnd,
    &Data
    );
}


/**
  Pop current expression from the Stack

  @param  [out] Pointer          Pointer to current expression to be pop.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PopCurrentExpression (
  OUT VOID    **Pointer
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Data;

  Status = PopStack (
    mCurrentExpressionStack,
    &mCurrentExpressionPointer,
    &Data
    );

  *Pointer = (VOID *) (UINTN) Data.Value.u64;

  return Status;
}

/**
  Reset stack pointer to begin of the stack.

**/
VOID
ResetMapExpressionListStack (
  VOID
  )
{
  mMapExpressionListPointer = mMapExpressionListStack;
}


/**
  Grow size of the stack.

  This is an internal function.

  @param  [in,out] Stack         On input: old stack; On output: new stack
  @param  [in,out] StackPtr      On input: old stack pointer; On output: new stack
                                 pointer
  @param  [in,out] StackEnd      On input: old stack end; On output: new stack end
  @param  [in] MemberSize        The stack member size.

  @retval EFI_SUCCESS            Grow stack success.
  @retval EFI_OUT_OF_RESOURCES   No enough memory for stack space.

**/
STATIC
EFI_STATUS
GrowConditionalStack (
  IN OUT FORM_EXPRESSION   ***Stack,
  IN OUT FORM_EXPRESSION   ***StackPtr,
  IN OUT FORM_EXPRESSION   ***StackEnd,
  IN     UINTN             MemberSize
  )
{
  UINTN             Size;
  FORM_EXPRESSION   **NewStack;

  Size = EXPRESSION_STACK_SIZE_INCREMENT;
  if (*StackPtr != NULL) {
    Size = Size + (*StackEnd - *Stack);
  }

  NewStack = AllocatePool (Size * MemberSize);
  if (NewStack == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (*StackPtr != NULL) {
    //
    // Copy from Old Stack to the New Stack
    //
    CopyMem (
      NewStack,
      *Stack,
      (*StackEnd - *Stack) * MemberSize
      );

    //
    // Free The Old Stack
    //
    FreePool (*Stack);
  }

  //
  // Make the Stack pointer point to the old data in the new stack
  //
  *StackPtr = NewStack + (*StackPtr - *Stack);
  *Stack    = NewStack;
  *StackEnd = NewStack + Size;

  return EFI_SUCCESS;
}

/**
  Push an element onto the Stack.

  @param  [in,out] Stack         On input: old stack; On output: new stack
  @param  [in,out] StackPtr      On input: old stack pointer; On output: new stack
                                 pointer
  @param  [in,out] StackEnd      On input: old stack end; On output: new stack end
  @param  [in] Data              Data to push.

  @retval EFI_SUCCESS            Push stack success.

**/
STATIC
EFI_STATUS
PushConditionalStack (
  IN OUT FORM_EXPRESSION   ***Stack,
  IN OUT FORM_EXPRESSION   ***StackPtr,
  IN OUT FORM_EXPRESSION   ***StackEnd,
  IN     FORM_EXPRESSION   **Data
  )
{
  EFI_STATUS  Status;

  //
  // Check for a stack overflow condition
  //
  if (*StackPtr >= *StackEnd) {
    //
    // Grow the stack
    //
    Status = GrowConditionalStack (Stack, StackPtr, StackEnd, sizeof (FORM_EXPRESSION *));
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Push the item onto the stack
  //
  CopyMem (*StackPtr, Data, sizeof (FORM_EXPRESSION *));
  *StackPtr = *StackPtr + 1;

  return EFI_SUCCESS;

}

/**
  Pop an element from the stack.

  @param  [in]  Stack            On input: old stack
  @param  [in,  out]StackPtr     On input: old stack pointer; On output: new stack pointer
  @param  [out] Data             Data to pop.

  @retval EFI_SUCCESS            The value was popped onto the stack.
  @retval EFI_ACCESS_DENIED      The pop operation underflowed the stack

**/
STATIC
EFI_STATUS
PopConditionalStack (
  IN     FORM_EXPRESSION   **Stack,
  IN OUT FORM_EXPRESSION   ***StackPtr,
  OUT    FORM_EXPRESSION   **Data
  )
{
  //
  // Check for a stack underflow condition
  //
  if (*StackPtr == Stack) {
    return EFI_ACCESS_DENIED;
  }

  //
  // Pop the item off the stack
  //
  *StackPtr = *StackPtr - 1;
  CopyMem (Data, *StackPtr, sizeof (FORM_EXPRESSION  *));
  return EFI_SUCCESS;

}

/**
  Get the expression list count by ExpressForm.

  @retval                        The expression count

**/
INTN
GetConditionalExpressionCount (
  IN EXPRESS_LEVEL       Level
  )
{
  switch (Level) {
    case ExpressForm:
      return mFormExpressionPointer - mFormExpressionStack;
    case ExpressStatement:
      return mStatementExpressionPointer - mStatementExpressionStack;
    case ExpressOption:
      return mOptionExpressionPointer - mOptionExpressionStack;
    default:
      ASSERT (FALSE);
      return -1;
  }
}

/**
  Get the expression Buffer pointer by ExpressForm.

  @retval  The start pointer of the expression buffer.

**/
FORM_EXPRESSION **
GetConditionalExpressionList (
  IN EXPRESS_LEVEL       Level
  )
{
  switch (Level) {
    case ExpressForm:
      return mFormExpressionStack;
    case ExpressStatement:
      return mStatementExpressionStack;
    case ExpressOption:
      return mOptionExpressionStack;
    default:
      ASSERT (FALSE);
      return NULL;
  }
}

/**
  Push the expression options onto the Stack.

  @param  [in] Pointer           Pointer to the current expression.
  @param  [in] Level             Which type this expression belong to. Form,
                                 statement or option?

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PushConditionalExpression (
  IN FORM_EXPRESSION   *Pointer,
  IN EXPRESS_LEVEL     Level
  )
{
  switch (Level) {
    case ExpressForm:
      return PushConditionalStack (
        &mFormExpressionStack,
        &mFormExpressionPointer,
        &mFormExpressionEnd,
        &Pointer
        );
    case ExpressStatement:
      return PushConditionalStack (
        &mStatementExpressionStack,
        &mStatementExpressionPointer,
        &mStatementExpressionEnd,
        &Pointer
        );
    case ExpressOption:
      return PushConditionalStack (
        &mOptionExpressionStack,
        &mOptionExpressionPointer,
        &mOptionExpressionEnd,
        &Pointer
        );
    default:
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
  }
}

/**
  Pop the expression options from the Stack

  @param  [in] Level             Which type this expression belong to. Form,
                                 statement or option?

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PopConditionalExpression (
  IN  EXPRESS_LEVEL      Level
  )
{
  FORM_EXPRESSION   *Pointer;

  switch (Level) {
    case ExpressForm:
      return PopConditionalStack (
        mFormExpressionStack,
        &mFormExpressionPointer,
        &Pointer
      );

    case ExpressStatement:
      return PopConditionalStack (
        mStatementExpressionStack,
        &mStatementExpressionPointer,
        &Pointer
      );

    case ExpressOption:
      return PopConditionalStack (
        mOptionExpressionStack,
        &mOptionExpressionPointer,
        &Pointer
      );

    default:
      ASSERT (FALSE);
      return EFI_INVALID_PARAMETER;
  }
}

/**
  Push the list of map expression onto the Stack

  @param  [in] Pointer           Pointer to the list of map expression to be pushed.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PushMapExpressionList (
  IN VOID  *Pointer
  )
{
  EFI_HII_VALUE  Data;

  Data.Type = EFI_IFR_TYPE_NUM_SIZE_64;
  Data.Value.u64 = (UINT64) (UINTN) Pointer;

  return PushStack (
    &mMapExpressionListStack,
    &mMapExpressionListPointer,
    &mMapExpressionListEnd,
    &Data
    );
}


/**
  Pop the list of map expression from the Stack

  @param  [out] Pointer          Pointer to the list of map expression to be pop.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the stack.

**/
EFI_STATUS
PopMapExpressionList (
  OUT VOID    **Pointer
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Data;

  Status = PopStack (
    mMapExpressionListStack,
    &mMapExpressionListPointer,
    &Data
    );

  *Pointer = (VOID *) (UINTN) Data.Value.u64;

  return Status;
}

/**
  Reset stack pointer to begin of the stack.

**/
VOID
ResetScopeStack (
  VOID
  )
{
  mOpCodeScopeStackPointer = mOpCodeScopeStack;
}


/**
  Push an Operand onto the Stack

  @param  [in] Operand           Operand to push.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the
                                 stack.
**/
EFI_STATUS
PushScope (
  IN UINT8   Operand
  )
{
  EFI_HII_VALUE  Data;

  Data.Type = EFI_IFR_TYPE_NUM_SIZE_8;
  Data.Value.u8 = Operand;

  return PushStack (
           &mOpCodeScopeStack,
           &mOpCodeScopeStackPointer,
           &mOpCodeScopeStackEnd,
           &Data
           );
}


/**
  Pop an Operand from the Stack

  @param  [out] Operand          Operand to pop.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the
                                 stack.

**/
EFI_STATUS
PopScope (
  OUT UINT8     *Operand
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Data;

  Status = PopStack (
             mOpCodeScopeStack,
             &mOpCodeScopeStackPointer,
             &Data
             );

  *Operand = Data.Value.u8;

  return Status;
}


/**
  Push an Expression value onto the Stack

  @param  [out] Value            Expression value to push.

  @retval EFI_SUCCESS            The value was pushed onto the stack.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the
                                 stack.

**/
STATIC
EFI_STATUS
PushExpression (
  IN EFI_HII_VALUE  *Value
  )
{
  return PushStack (
           &mExpressionEvaluationStack,
           &mExpressionEvaluationStackPointer,
           &mExpressionEvaluationStackEnd,
           Value
           );
}


/**
  Pop an Expression value from the stack.

  @param  [out] Value                  Expression value to pop.

  @retval EFI_SUCCESS            The value was popped onto the stack.
  @retval EFI_ACCESS_DENIED      The pop operation underflowed the stack

**/
STATIC
EFI_STATUS
PopExpression (
  OUT EFI_HII_VALUE  *Value
  )
{
  return PopStack (
           mExpressionEvaluationStack + mExpressionEvaluationStackOffset,
           &mExpressionEvaluationStackPointer,
           Value
           );
}

/**
  Get current stack offset from stack start.

  @return Stack offset to stack start.
**/
STATIC
UINTN
SaveExpressionEvaluationStackOffset (
  )
{
  UINTN TempStackOffset;
  TempStackOffset = mExpressionEvaluationStackOffset;
  mExpressionEvaluationStackOffset = mExpressionEvaluationStackPointer - mExpressionEvaluationStack;
  return TempStackOffset;
}

/**
  Restore stack offset based on input stack offset

  @param  StackOffset  Offset to stack start.

**/
STATIC
VOID
RestoreExpressionEvaluationStackOffset (
  UINTN StackOffset
  )
{
  mExpressionEvaluationStackOffset = StackOffset;
}

/**
  Get Form given its FormId.

  @param  [in] FormSet                The FormSet which contains this Form.
  @param  [in] FormId                 Id of this Form.

  @retval Pointer                     The Form.
  @retval NULL                        Specified Form is not found in the FormSet.

**/
FORM_BROWSER_FORM *
IdToForm (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN EFI_FORM_ID                FormId
)
{
  LIST_ENTRY         *Link;
  FORM_BROWSER_FORM  *Form;

  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);

    if (Form->FormId == FormId) {
      return Form;
    }

    Link = GetNextNode (&FormSet->FormListHead, Link);
  }

  return NULL;
}


/**
  Search a Question in Form scope using its Question ID.

  @param  [in] Form              The Form which contains this Question.
  @param  [in] QuestionId        Id of this Question.

  @retval Pointer                The Question.
  @retval NULL                   Specified Question not found in the Form.

**/
FORM_BROWSER_STATEMENT *
IdToQuestion2 (
  IN FORM_BROWSER_FORM  *Form,
  IN UINT16             QuestionId
  )
{
  LIST_ENTRY              *Link;
  FORM_BROWSER_STATEMENT  *Question;

  if (QuestionId == 0 || Form == NULL) {
    //
    // The value of zero is reserved
    //
    return NULL;
  }

  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);

    if (Question->QuestionId == QuestionId) {
      return Question;
    }

    Link = GetNextNode (&Form->StatementListHead, Link);
  }

  return NULL;
}


/**
  Search a Question in Formset scope using its QuestionId.

  @param  [in] FormSet           The FormSet which contains this Form.
  @param  [in] Form              The Form which contains this Question.
  @param  [in] QuestionId        Id of this Question.

  @retval Pointer                The Question.
  @retval NULL                   Specified Question not found in the Form.

**/
FORM_BROWSER_STATEMENT *
IdToQuestion (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form,
  IN UINT16                QuestionId
  )
{
  LIST_ENTRY              *Link;
  FORM_BROWSER_STATEMENT  *Question;

  //
  // Search in the FORM scope first
  //
  Question = IdToQuestion2 (Form, QuestionId);
  if (Question != NULL) {
    return Question;
  }

  //
  // Search in the FormSet scope
  //
  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);

    Question = IdToQuestion2 (Form, QuestionId);
    if (Question != NULL) {
      //
      // EFI variable storage may be updated by Callback() asynchronous,
      // to keep synchronous, always reload the Question Value.
      //
      if (Question->Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
        GetQuestionValue (FormSet, Form, Question, GetSetValueWithHiiDriver);
      }

      return Question;
    }

    Link = GetNextNode (&FormSet->FormListHead, Link);
  }

  return NULL;
}


/**
  Get Expression given its RuleId.

  @param  [in] Form              The Form which contains this Expression.
  @param  [in] RuleId            Id of this Expression.

  @retval Pointer                The Expression.
  @retval NULL                   Specified Expression not found in the Form.

**/
STATIC
FORM_EXPRESSION *
RuleIdToExpression (
  IN FORM_BROWSER_FORM  *Form,
  IN UINT8              RuleId
  )
{
  LIST_ENTRY       *Link;
  FORM_EXPRESSION  *Expression;

  Link = GetFirstNode (&Form->ExpressionListHead);
  while (!IsNull (&Form->ExpressionListHead, Link)) {
    Expression = FORM_EXPRESSION_FROM_LINK (Link);

    if (Expression->Type == EFI_HII_EXPRESSION_RULE && Expression->RuleId == RuleId) {
      return Expression;
    }

    Link = GetNextNode (&Form->ExpressionListHead, Link);
  }

  return NULL;
}


/**
  Locate the Unicode Collation Protocol interface for later use.

  @retval EFI_SUCCESS            Protocol interface initialize success.
  @retval Other                  Protocol interface initialize failed.

**/
STATIC
EFI_STATUS
InitializeUnicodeCollationProtocol (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mUnicodeCollation != NULL) {
    return EFI_SUCCESS;
  }

  //
  // BUGBUG: Proper impelmentation is to locate all Unicode Collation Protocol
  // instances first and then select one which support English language.
  // Current implementation just pick the first instance.
  //
  Status = gBS->LocateProtocol (
                  &gEfiUnicodeCollation2ProtocolGuid,
                  NULL,
                  (VOID **) &mUnicodeCollation
                  );
  return Status;
}

/**
  Convert the input Unicode character to upper.

  @param String  Th Unicode character to be converted.

**/
STATIC
VOID
IfrStrToUpper (
  IN CHAR16                   *String
  )
{
  mUnicodeCollation->StrUpr(mUnicodeCollation,String);
}


/**
  Evaluate opcode EFI_IFR_TO_STRING.

  @param  [in]  FormSet          Formset which contains this opcode.
  @param  [in]  Format           String format in EFI_IFR_TO_STRING.
  @param  [out] Result           Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
STATIC
EFI_STATUS
IfrToString (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN UINT8                 Format,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String;
  CHAR16         *PrintFormat;
  CHAR16         Buffer[MAXIMUM_VALUE_CHARACTERS];
  UINT8          *TmpBuf;
  UINTN          BufferSize;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  switch (Value.Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
  case EFI_IFR_TYPE_NUM_SIZE_16:
  case EFI_IFR_TYPE_NUM_SIZE_32:
  case EFI_IFR_TYPE_NUM_SIZE_64:
    BufferSize = MAXIMUM_VALUE_CHARACTERS * sizeof (CHAR16);
    switch (Format) {
    case EFI_IFR_STRING_UNSIGNED_DEC:
    case EFI_IFR_STRING_SIGNED_DEC:
      PrintFormat = L"%ld";
      break;

    case EFI_IFR_STRING_LOWERCASE_HEX:
      PrintFormat = L"%lx";
      break;

    case EFI_IFR_STRING_UPPERCASE_HEX:
      PrintFormat = L"%lX";
      break;

    default:
      Result->Type = EFI_IFR_TYPE_UNDEFINED;
      return EFI_SUCCESS;
    }
    UnicodeSPrint (Buffer, BufferSize, PrintFormat, Value.Value.u64);
    String = Buffer;
    break;

  case EFI_IFR_TYPE_STRING:
    CopyMem (Result, &Value, sizeof (EFI_HII_VALUE));
    return EFI_SUCCESS;

  case EFI_IFR_TYPE_BOOLEAN:
    String = (Value.Value.b) ? L"True" : L"False";
    break;

  case EFI_IFR_TYPE_BUFFER:
    //
    // + 3 is base on the unicode format, the length may be odd number,
    // so need 1 byte to align, also need 2 bytes for L'\0'.
    //
    TmpBuf = AllocateZeroPool (Value.BufferLen + 3);
    ASSERT (TmpBuf != NULL);
    if (TmpBuf == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    if (Format == EFI_IFR_STRING_ASCII) {
      CopyMem (TmpBuf, Value.Buffer, Value.BufferLen);
      PrintFormat = L"%a";
    } else {
      // Format == EFI_IFR_STRING_UNICODE
      CopyMem (TmpBuf, Value.Buffer, Value.BufferLen * sizeof (CHAR16));
      PrintFormat = L"%s";
    }
    UnicodeSPrint (Buffer, sizeof (Buffer), PrintFormat, Value.Buffer);
    String = Buffer;
    FreePool (TmpBuf);
    FreePool (Value.Buffer);
    break;

  default:
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    return EFI_SUCCESS;
  }

  Result->Type = EFI_IFR_TYPE_STRING;
  Result->Value.string = NewString (String, FormSet->HiiHandle);
  return EFI_SUCCESS;
}


/**
  Evaluate opcode EFI_IFR_TO_UINT.

  @param  [in] FormSet           Formset which contains this opcode.
  @param  [out] Result           Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
STATIC
EFI_STATUS
IfrToUint (
  IN FORM_BROWSER_FORMSET  *FormSet,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value;
  CHAR16         *String;
  CHAR16         *StringPtr;

  Status = PopExpression (&Value);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Value.Type >= EFI_IFR_TYPE_OTHER && Value.Type != EFI_IFR_TYPE_BUFFER) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    return EFI_SUCCESS;
  }

  Status = EFI_SUCCESS;
  if (Value.Type == EFI_IFR_TYPE_STRING) {
    String = GetString (Value.Value.string, FormSet->HiiHandle);
    if (String == NULL) {
      return EFI_NOT_FOUND;
    }

    IfrStrToUpper (String);
    StringPtr = StrStr (String, L"0X");
    if (StringPtr != NULL) {
      //
      // Hex string
      //
      Result->Value.u64 = StrHexToUint64 (String);
    } else {
      //
      // decimal string
      //
      Result->Value.u64 = StrDecimalToUint64 (String);
    }
    FreePool (String);
  } else if (Value.Type == EFI_IFR_TYPE_BUFFER) {
    if (Value.BufferLen > 8) {
      FreePool (Value.Buffer);
      Result->Type = EFI_IFR_TYPE_UNDEFINED;
      return EFI_SUCCESS;
    }
    Result->Value.u64 = *(UINT64*) Value.Buffer;
    FreePool (Value.Buffer);
  } else {
    CopyMem (Result, &Value, sizeof (EFI_HII_VALUE));
  }

  Result->Type = EFI_IFR_TYPE_NUM_SIZE_64;
  return Status;
}


/**
  Evaluate opcode EFI_IFR_CATENATE.

  @param  [in] FormSet           Formset which contains this opcode.
  @param  [out] Result           Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
STATIC
EFI_STATUS
IfrCatenate (
  IN FORM_BROWSER_FORMSET  *FormSet,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value[2];
  CHAR16         *String[2];
  UINTN          Index;
  CHAR16         *StringPtr;
  UINTN          Size;
  UINTN          MaxLen;

  //
  // String[0] - The second string
  // String[1] - The first string
  //
  String[0] = NULL;
  String[1] = NULL;
  StringPtr = NULL;
  Status = EFI_SUCCESS;
  ZeroMem (Value, sizeof (Value));

  Status = PopExpression (&Value[0]);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = PopExpression (&Value[1]);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  for (Index = 0; Index < 2; Index++) {
    if (Value[Index].Type != EFI_IFR_TYPE_STRING && Value[Index].Type != EFI_IFR_TYPE_BUFFER) {
      Result->Type = EFI_IFR_TYPE_UNDEFINED;
      Status = EFI_SUCCESS;
      goto Done;
    }

    if (Value[Index].Type == EFI_IFR_TYPE_STRING) {
      String[Index] = GetString (Value[Index].Value.string, FormSet->HiiHandle);
      if (String[Index] == NULL) {
        Status = EFI_NOT_FOUND;
        goto Done;
      }
    }
  }

  if (Value[0].Type == EFI_IFR_TYPE_STRING) {
    Size = StrSize (String[0]);
    MaxLen = (StrSize (String[1]) + Size) / sizeof (CHAR16);
    StringPtr= AllocatePool (MaxLen * sizeof (CHAR16));
    ASSERT (StringPtr != NULL);
    if (StringPtr == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }
    StrCpyS (StringPtr, MaxLen, String[1]);
    StrCatS (StringPtr, MaxLen, String[0]);

    Result->Type = EFI_IFR_TYPE_STRING;
    Result->Value.string = NewString (StringPtr, FormSet->HiiHandle);
  } else {
    Result->Type = EFI_IFR_TYPE_BUFFER;
    Result->BufferLen = (UINT16) (Value[0].BufferLen + Value[1].BufferLen);

    Result->Buffer = AllocateZeroPool (Result->BufferLen);
    ASSERT (Result->Buffer != NULL);
    if (Result->Buffer == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (Result->Buffer, Value[0].Buffer, Value[0].BufferLen);
    CopyMem (&Result->Buffer[Value[0].BufferLen], Value[1].Buffer, Value[1].BufferLen);
  }
Done:
  if (Value[0].Buffer != NULL) {
    FreePool (Value[0].Buffer);
  }
  if (Value[1].Buffer != NULL) {
    FreePool (Value[1].Buffer);
  }
  if (String[0] != NULL) {
    FreePool (String[0]);
  }
  if (String[1] != NULL) {
    FreePool (String[1]);
  }
  if (StringPtr != NULL) {
    FreePool (StringPtr);
  }

  return Status;
}


/**
  Evaluate opcode EFI_IFR_MATCH.

  @param  [in]  FormSet          FormSet which contains this opcode.
  @param  [out] Result           Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
STATIC
EFI_STATUS
IfrMatch (
  IN FORM_BROWSER_FORMSET  *FormSet,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value[2];
  CHAR16         *String[2];
  UINTN          Index;

  //
  // String[0] - The string to search
  // String[1] - pattern
  //
  String[0] = NULL;
  String[1] = NULL;
  Status = EFI_SUCCESS;
  ZeroMem (Value, sizeof (Value));

  Status = PopExpression (&Value[0]);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = PopExpression (&Value[1]);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  for (Index = 0; Index < 2; Index++) {
    if (Value[Index].Type != EFI_IFR_TYPE_STRING) {
      Result->Type = EFI_IFR_TYPE_UNDEFINED;
      Status = EFI_SUCCESS;
      goto Done;
    }

    String[Index] = GetString (Value[Index].Value.string, FormSet->HiiHandle);
    if (String [Index] == NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
  }

  Result->Type = EFI_IFR_TYPE_BOOLEAN;
  Result->Value.b = mUnicodeCollation->MetaiMatch (mUnicodeCollation, String[0], String[1]);

Done:
  if (String[0] != NULL) {
    FreePool (String[0]);
  }
  if (String[1] != NULL) {
    FreePool (String[1]);
  }

  return Status;
}


/**
  Evaluate opcode EFI_IFR_FIND.

  @param  [in]  FormSet          FormSet which contains this opcode.
  @param  [in]  Format           Case sensitive or insensitive.
  @param  [out] Result           Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
STATIC
EFI_STATUS
IfrFind (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN UINT8                 Format,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value[3];
  CHAR16         *String[2];
  UINTN          Base;
  CHAR16         *StringPtr;
  UINTN          Index;

  ZeroMem (Value, sizeof (Value));

  if (Format > EFI_IFR_FF_CASE_INSENSITIVE) {
    return EFI_INVALID_PARAMETER;
  }

  Status = PopExpression (&Value[0]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PopExpression (&Value[1]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PopExpression (&Value[2]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Value[0].Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    return EFI_SUCCESS;
  }
  Base = (UINTN) Value[0].Value.u64;

  //
  // String[0] - sub-string
  // String[1] - The string to search
  //
  String[0] = NULL;
  String[1] = NULL;
  for (Index = 0; Index < 2; Index++) {
    if (Value[Index + 1].Type != EFI_IFR_TYPE_STRING) {
      Result->Type = EFI_IFR_TYPE_UNDEFINED;
      Status = EFI_SUCCESS;
      goto Done;
    }

    String[Index] = GetString (Value[Index + 1].Value.string, FormSet->HiiHandle);
    if (String[Index] == NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }

    if (Format == EFI_IFR_FF_CASE_INSENSITIVE) {
      //
      // Case insensitive, convert both string to upper case
      //
      IfrStrToUpper (String[Index]);
    }
  }

  Result->Type = EFI_IFR_TYPE_NUM_SIZE_64;
  if (Base >= StrLen (String[1])) {
    Result->Value.u64 = UINT64_MAX;
  } else {
    StringPtr = StrStr (String[1] + Base, String[0]);
    Result->Value.u64 = (StringPtr == NULL) ? UINT64_MAX : (StringPtr - String[1]);
  }

Done:
  if (String[0] != NULL) {
    FreePool (String[0]);
  }
  if (String[1] != NULL) {
    FreePool (String[1]);
  }

  return Status;
}


/**
  Evaluate opcode EFI_IFR_MID.

  @param  [in]  FormSet          FormSet which contains this opcode.
  @param  [out] Result           Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
STATIC
EFI_STATUS
IfrMid (
  IN FORM_BROWSER_FORMSET  *FormSet,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value[3];
  CHAR16         *String;
  UINTN          Base;
  UINTN          Length;
  CHAR16         *SubString;
  UINT16         BufferLen;

  ZeroMem (Value, sizeof (Value));

  Status = PopExpression (&Value[0]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PopExpression (&Value[1]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PopExpression (&Value[2]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Value[0].Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    return EFI_SUCCESS;
  }
  Length = (UINTN) Value[0].Value.u64;

  if (Value[1].Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    return EFI_SUCCESS;
  }
  Base = (UINTN) Value[1].Value.u64;

  if (Value[2].Type != EFI_IFR_TYPE_STRING && Value[2].Type != EFI_IFR_TYPE_BUFFER) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    return EFI_SUCCESS;
  }
  if (Value[2].Type == EFI_IFR_TYPE_STRING) {
    String = GetString (Value[2].Value.string, FormSet->HiiHandle);
    if (String == NULL) {
      return EFI_NOT_FOUND;
    }

    if (Length == 0 || Base >= StrLen (String)) {
      SubString = gEmptyString;
    } else {
      SubString = String + Base;
      if ((Base + Length) < StrLen (String)) {
        SubString[Length] = L'\0';
      }
    }

    Result->Type = EFI_IFR_TYPE_STRING;
    Result->Value.string = NewString (SubString, FormSet->HiiHandle);

    FreePool (String);
  } else {
    BufferLen = Value[2].BufferLen;

    Result->Type = EFI_IFR_TYPE_BUFFER;
    if (Length == 0 || Base >= BufferLen) {
      Result->BufferLen = 0;
      Result->Buffer = NULL;
    } else {
      Result->BufferLen = (UINT16)((BufferLen - Base) < Length ? (BufferLen - Base) : Length);
      Result->Buffer = AllocateZeroPool (Result->BufferLen);
      ASSERT (Result->Buffer != NULL);
      if (Result->Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (Result->Buffer, &Value[2].Buffer[Base], Result->BufferLen);
    }

    FreePool (Value[2].Buffer);
  }

  return Status;
}


/**
  Evaluate opcode EFI_IFR_TOKEN.

  @param  [in]  FormSet          FormSet which contains this opcode.
  @param  [out] Result           Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
STATIC
EFI_STATUS
IfrToken (
  IN FORM_BROWSER_FORMSET  *FormSet,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value[3];
  CHAR16         *String[2];
  UINTN          Count;
  CHAR16         *Delimiter;
  CHAR16         *SubString;
  CHAR16         *StringPtr;
  UINTN          Index;

  ZeroMem (Value, sizeof (Value));

  Status = PopExpression (&Value[0]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PopExpression (&Value[1]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PopExpression (&Value[2]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Value[0].Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    return EFI_SUCCESS;
  }
  Count = (UINTN) Value[0].Value.u64;

  //
  // String[0] - Delimiter
  // String[1] - The string to search
  //
  String[0] = NULL;
  String[1] = NULL;
  for (Index = 0; Index < 2; Index++) {
    if (Value[Index + 1].Type != EFI_IFR_TYPE_STRING) {
      Result->Type = EFI_IFR_TYPE_UNDEFINED;
      Status = EFI_SUCCESS;
      goto Done;
    }

    String[Index] = GetString (Value[Index + 1].Value.string, FormSet->HiiHandle);
    if (String[Index] == NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
  }

  Delimiter = String[0];
  SubString = String[1];
  while (Count > 0) {
    SubString = StrStr (SubString, Delimiter);
    if (SubString != NULL) {
      //
      // Skip over the delimiter
      //
      SubString = SubString + StrLen (Delimiter);
    } else {
      break;
    }
    Count--;
  }

  if (SubString == NULL) {
    //
    // nth delimited sub-string not found, push an empty string
    //
    SubString = gEmptyString;
  } else {
    //
    // Put a NULL terminator for nth delimited sub-string
    //
    StringPtr = StrStr (SubString, Delimiter);
    if (StringPtr != NULL) {
      *StringPtr = L'\0';
    }
  }

  Result->Type = EFI_IFR_TYPE_STRING;
  Result->Value.string = NewString (SubString, FormSet->HiiHandle);

Done:
  if (String[0] != NULL) {
    FreePool (String[0]);
  }
  if (String[1] != NULL) {
    FreePool (String[1]);
  }

  return Status;
}


/**
  Evaluate opcode EFI_IFR_SPAN.

  @param  [in]  FormSet          FormSet which contains this opcode.
  @param  [in]  Flags            FIRST_MATCHING or FIRST_NON_MATCHING.
  @param  [out] Result           Evaluation result for this opcode.

  @retval EFI_SUCCESS            Opcode evaluation success.
  @retval Other                  Opcode evaluation failed.

**/
STATIC
EFI_STATUS
IfrSpan (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN UINT8                 Flags,
  OUT  EFI_HII_VALUE       *Result
  )
{
  EFI_STATUS     Status;
  EFI_HII_VALUE  Value[3];
  CHAR16         *String[2];
  CHAR16         *Charset;
  UINTN          Base;
  UINTN          Index;
  CHAR16         *StringPtr;
  BOOLEAN        Found;

  ZeroMem (Value, sizeof (Value));

  Status = PopExpression (&Value[0]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PopExpression (&Value[1]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PopExpression (&Value[2]);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (Value[0].Type > EFI_IFR_TYPE_NUM_SIZE_64) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    return EFI_SUCCESS;
  }
  Base = (UINTN) Value[0].Value.u64;

  //
  // String[0] - Charset
  // String[1] - The string to search
  //
  String[0] = NULL;
  String[1] = NULL;
  for (Index = 0; Index < 2; Index++) {
    if (Value[Index + 1].Type != EFI_IFR_TYPE_STRING) {
      Result->Type = EFI_IFR_TYPE_UNDEFINED;
      Status = EFI_SUCCESS;
      goto Done;
    }

    String[Index] = GetString (Value[Index + 1].Value.string, FormSet->HiiHandle);
    if (String [Index] == NULL) {
      Status = EFI_NOT_FOUND;
      goto Done;
    }
  }

  if (Base >= StrLen (String[1])) {
    Result->Type = EFI_IFR_TYPE_UNDEFINED;
    Status = EFI_SUCCESS;
    goto Done;
  }

  Found = FALSE;
  StringPtr = String[1] + Base;
  Charset = String[0];
  while (*StringPtr != 0 && !Found) {
    Index = 0;
    while (Charset[Index] != 0) {
      if (*StringPtr >= Charset[Index] && *StringPtr <= Charset[Index + 1]) {
        if (Flags == EFI_IFR_FLAGS_FIRST_MATCHING) {
          Found = TRUE;
          break;
        }
      } else {
        if (Flags == EFI_IFR_FLAGS_FIRST_NON_MATCHING) {
          Found = TRUE;
          break;
        }
      }
      //
      // Skip characters pair representing low-end of a range and high-end of a range
      //
      Index += 2;
    }

    if (!Found) {
      StringPtr++;
    }
  }

  Result->Type = EFI_IFR_TYPE_NUM_SIZE_64;
  Result->Value.u64 = StringPtr - String[1];

Done:
  if (String[0] != NULL) {
    FreePool (String[0]);
  }
  if (String[1] != NULL) {
    FreePool (String[1]);
  }

  return Status;
}


/**
  Zero extend integer/boolean/date/time to UINT64 for comparing.

  @param  [in] Value                  HII Value to be converted.

**/
VOID
ExtendValueToU64 (
  IN  EFI_HII_VALUE   *Value
  )
{
  UINT64  Temp;

  Temp = 0;
  switch (Value->Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    Temp = Value->Value.u8;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    Temp = Value->Value.u16;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    Temp = Value->Value.u32;
    break;

  case EFI_IFR_TYPE_BOOLEAN:
    Temp = Value->Value.b;
    break;

  case EFI_IFR_TYPE_TIME:
    Temp = Value->Value.u32 & 0xffffff;
    break;

  case EFI_IFR_TYPE_DATE:
    Temp = Value->Value.u32;
    break;

  default:
    return;
  }

  Value->Value.u64 = Temp;
}

/**
  Get UINT64 type value.

  @param  Value                  Input Hii value.

  @retval UINT64                 Return the UINT64 type value.

**/
UINT64
HiiValueToUINT64 (
  IN EFI_HII_VALUE      *Value
  )
{
  UINT64  RetVal;

  RetVal = 0;

  switch (Value->Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    RetVal = Value->Value.u8;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    RetVal = Value->Value.u16;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    RetVal = Value->Value.u32;
    break;

  case EFI_IFR_TYPE_BOOLEAN:
    RetVal = Value->Value.b;
    break;

  case EFI_IFR_TYPE_DATE:
    RetVal = *(UINT64*) &Value->Value.date;
    break;

  case EFI_IFR_TYPE_TIME:
    RetVal = (*(UINT64*) &Value->Value.time) & 0xffffff;
    break;

  default:
    RetVal = Value->Value.u64;
    break;
  }

  return RetVal;
}

/**
  Compare two HII values.

  @param  [in] Value1            Expression value to compare on left-hand.
  @param  [in] Value2            Expression value to compare on right-hand.
  @param  [out] Result           Return value after compare.
                                 @retval 0                      Two operators equal.
                                 return Positive value if Value1 is greater than Value2.
                                 retval Negative value if Value1 is less than Value2.
  @param  [in,out]HiiHandle      Only required for string compare.

  @retval other                  Could not perform compare on two values.
  @retval EFI_SUCCESS            Compare the value success.

**/
EFI_STATUS
CompareValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  OUT INTN            *Result,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  )
{
  INT64   Temp64;
  CHAR16  *Str1;
  CHAR16  *Str2;
  UINTN   Len;

  if (Value1->Type >= EFI_IFR_TYPE_OTHER || Value2->Type >= EFI_IFR_TYPE_OTHER ) {
    if (Value1->Type != EFI_IFR_TYPE_BUFFER && Value2->Type != EFI_IFR_TYPE_BUFFER) {
      return EFI_UNSUPPORTED;
    }
  }

  if (Value1->Type == EFI_IFR_TYPE_STRING || Value2->Type == EFI_IFR_TYPE_STRING ) {
    if (Value1->Type != Value2->Type) {
      //
      // Both Operator should be type of String
      //
      return EFI_UNSUPPORTED;
    }

    if (Value1->Value.string == 0 || Value2->Value.string == 0) {
      //
      // StringId 0 is reserved
      //
      return EFI_INVALID_PARAMETER;
    }

    if (Value1->Value.string == Value2->Value.string) {
      *Result = 0;
      return EFI_SUCCESS;
    }

    Str1 = GetString (Value1->Value.string, HiiHandle);
    if (Str1 == NULL) {
      //
      // String not found
      //
      return EFI_NOT_FOUND;
    }

    Str2 = GetString (Value2->Value.string, HiiHandle);
    if (Str2 == NULL) {
      FreePool (Str1);
      return EFI_NOT_FOUND;
    }

    *Result = StrCmp (Str1, Str2);

    FreePool (Str1);
    FreePool (Str2);

    return EFI_SUCCESS;
  }

  if (Value1->Type == EFI_IFR_TYPE_BUFFER || Value2->Type == EFI_IFR_TYPE_BUFFER ) {
    if (Value1->Type != Value2->Type) {
      //
      // Both Operator should be type of Buffer.
      //
      return EFI_UNSUPPORTED;
    }
    Len = Value1->BufferLen > Value2->BufferLen ? Value2->BufferLen : Value1->BufferLen;
    *Result = CompareMem (Value1->Buffer, Value2->Buffer, Len);
    if ((*Result == 0) && (Value1->BufferLen != Value2->BufferLen))
    {
      //
      // In this case, means base on samll number buffer, the data is same
      // So which value has more data, which value is bigger.
      //
      *Result = Value1->BufferLen > Value2->BufferLen ? 1 : -1;
    }
    return EFI_SUCCESS;
  }

  //
  // Take remain types(integer, boolean, date/time) as integer
  //
  Temp64 = HiiValueToUINT64(Value1) - HiiValueToUINT64(Value2);
  if (Temp64 > 0) {
    *Result = 1;
  } else if (Temp64 < 0) {
    *Result = -1;
  } else {
    *Result = 0;
  }

  return EFI_SUCCESS;
}

/**
  Check if current user has the privilege specified by the permissions GUID.

  @param  [in] Guid  A GUID specifying setup access permissions.

  @retval TRUE       Current user has the privilege.
  @retval FALSE      Current user does not have the privilege.
**/
STATIC
BOOLEAN
CheckUserPrivilege (
  IN EFI_GUID *Guid
  )
{
  EFI_STATUS                   Status;
  EFI_USER_PROFILE_HANDLE      UserProfileHandle;
  EFI_USER_INFO_HANDLE         UserInfoHandle;
  EFI_USER_INFO                *UserInfo;
  EFI_GUID                     *UserPermissionsGuid;
  UINTN                        UserInfoSize;
  UINTN                        AccessControlDataSize;
  EFI_USER_INFO_ACCESS_CONTROL *AccessControl;
  UINTN                        RemainSize;

  if (mUserManager == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiUserManagerProtocolGuid,
                    NULL,
                    (VOID **) &mUserManager
                    );
    if (EFI_ERROR (Status)) {
      ///
      /// If the system does not support user management, then it is assumed that
      /// all users have admin privilege and evaluation of each EFI_IFR_SECURITY
      /// op-code is always TRUE.
      ///
      return TRUE;
    }
  }

  Status = mUserManager->Current (mUserManager, &UserProfileHandle);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Enumerate all user information of the current user profile
  /// to look for any EFI_USER_INFO_ACCESS_SETUP record.
  ///

  for (UserInfoHandle = NULL;;) {
    Status = mUserManager->GetNextInfo (mUserManager, UserProfileHandle, &UserInfoHandle);
    if (EFI_ERROR (Status)) {
      break;
    }

    UserInfoSize = 0;
    Status = mUserManager->GetInfo (mUserManager, UserProfileHandle, UserInfoHandle, NULL, &UserInfoSize);
    if (Status != EFI_BUFFER_TOO_SMALL) {
      continue;
    }

    UserInfo = (EFI_USER_INFO *) AllocatePool (UserInfoSize);
    if (UserInfo == NULL) {
      break;
    }

    Status = mUserManager->GetInfo (mUserManager, UserProfileHandle, UserInfoHandle, UserInfo, &UserInfoSize);
    if (EFI_ERROR (Status) ||
        UserInfo->InfoType != EFI_USER_INFO_ACCESS_POLICY_RECORD ||
        UserInfo->InfoSize <= sizeof (EFI_USER_INFO)) {
      FreePool (UserInfo);
      continue;
    }

    RemainSize = UserInfo->InfoSize - sizeof (EFI_USER_INFO);
    AccessControl = (EFI_USER_INFO_ACCESS_CONTROL *)(UserInfo + 1);
    while (RemainSize >= sizeof (EFI_USER_INFO_ACCESS_CONTROL)) {
      if (RemainSize < AccessControl->Size || AccessControl->Size < sizeof (EFI_USER_INFO_ACCESS_CONTROL)) {
        break;
      }
      if (AccessControl->Type == EFI_USER_INFO_ACCESS_SETUP) {
        ///
        /// Check if current user has the privilege specified by the permissions GUID.
        ///

        UserPermissionsGuid = (EFI_GUID *)(AccessControl + 1);
        AccessControlDataSize = AccessControl->Size - sizeof (EFI_USER_INFO_ACCESS_CONTROL);
        while (AccessControlDataSize >= sizeof (EFI_GUID)) {
          if (CompareGuid (Guid, UserPermissionsGuid)) {
            FreePool (UserInfo);
            return TRUE;
          }
          UserPermissionsGuid++;
          AccessControlDataSize -= sizeof (EFI_GUID);
        }
      }
      RemainSize -= AccessControl->Size;
      AccessControl = (EFI_USER_INFO_ACCESS_CONTROL *)((UINT8 *)AccessControl + AccessControl->Size);
    }

    FreePool (UserInfo);
  }
  return FALSE;
}

/**
  Get question value from the predefined FormSet.

  @param  [in]  DevicePath       The driver's device path which produece the FormSet data.
  @param  [in]  InputHiiHandle   The HII handle associate with the FormSet data.
  @param  [in]  FormSetGuid      The FormSet guid which include the question.
  @param  [in]  QuestionId       The question id which need to get value from.
  @param  [out] Value            The return data about question's value.

  @retval TRUE                   Get the question value success.
  @retval FALSE                  Get the question value failed.
**/
STATIC
BOOLEAN
GetQuestionValueFromForm (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN EFI_HII_HANDLE            InputHiiHandle,
  IN EFI_GUID                  *FormSetGuid,
  IN EFI_QUESTION_ID           QuestionId,
  OUT EFI_HII_VALUE            *Value
  )
{
  EFI_STATUS                   Status;
  EFI_HII_HANDLE               HiiHandle;
  FORM_BROWSER_STATEMENT       *Question;
  FORM_BROWSER_FORMSET         *FormSet;
  FORM_BROWSER_FORM            *Form;
  BOOLEAN                      ValueRetrieved;
  LIST_ENTRY                   *Link;

  //
  // The input parameter DevicePath or InputHiiHandle must have one valid input.
  //
  ASSERT ((DevicePath != NULL && InputHiiHandle == NULL) ||
          (DevicePath == NULL && InputHiiHandle != NULL) );

  ValueRetrieved    = TRUE;
  HiiHandle    = NULL;
  Question     = NULL;
  Form         = NULL;

  //
  // Get HII handles that were installed by the driver specified by DevicePath.
  //
  if (DevicePath != NULL) {
    HiiHandle = DevicePathToHiiHandle (DevicePath, FormSetGuid);
    if (HiiHandle == NULL) {
      return FALSE;
    }
  } else {
    HiiHandle = InputHiiHandle;
  }
  ASSERT (HiiHandle != NULL);

  //
  // Get the FormSet data include this question.
  //
  FormSet = AllocateZeroPool (sizeof (FORM_BROWSER_FORMSET));
  ASSERT (FormSet != NULL);
  if (FormSet == NULL) {
    return FALSE;
  }
  Status = InitializeFormSet(HiiHandle, FormSetGuid, FormSet);
  if (EFI_ERROR (Status)) {
    ValueRetrieved = FALSE;
    goto Done;
  }

  //
  // Use the Question ID to get the question information.
  //
  Question = IdToQuestion(FormSet, NULL, QuestionId);
  if (Question == NULL) {
    ValueRetrieved = FALSE;
    goto Done;
  }

  //
  // Search for the Form in the Form Set that contains the question
  //
  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);

    Question = IdToQuestion2 (Form, QuestionId);
    if (Question != NULL) {
      break;
    }

    Link = GetNextNode (&FormSet->FormListHead, Link);
    Form = NULL;
  }

  ASSERT (Form != NULL);
  if (Form == NULL) {
    DEBUG ((EFI_D_ERROR, "GetQuestionValueFromForm() can't find a form which contain the question"));
    return FALSE;
  }

  if (Question == NULL) {
    ValueRetrieved = FALSE;
    goto Done;
  }

  //
  // Get the question value.
  //
  Status = GetQuestionValue(FormSet, Form, Question, GetSetValueWithHiiDriver);
  if (EFI_ERROR (Status)) {
    ValueRetrieved = FALSE;
    goto Done;
  }

  CopyHiiValue (Value, &Question->HiiValue);

Done:
  //
  // Clean the FormSet structure and restore the global parameter.
  //
  if (FormSet != NULL) {
    DestroyFormSet (FormSet);
  }

  return ValueRetrieved;
}

/**
  Evaluate the result of a HII expression.

  If Expression is NULL, then ASSERT.

  @param  [in]     FormSet       FormSet associated with this expression.
  @param  [in]     Form          Form associated with this expression.
  @param  [in,out] Expression    Expression to be evaluated.

  @retval EFI_SUCCESS            The expression evaluated successfuly
  @retval EFI_NOT_FOUND          The Question which referenced by a QuestionId
                                 could not be found.
  @retval EFI_OUT_OF_RESOURCES   There is not enough system memory to grow the
                                 stack.
  @retval EFI_ACCESS_DENIED      The pop operation underflowed the stack
  @retval EFI_INVALID_PARAMETER  Syntax error with the Expression

**/
EFI_STATUS
EvaluateExpression (
  IN FORM_BROWSER_FORMSET  *FormSet,
  IN FORM_BROWSER_FORM     *Form,
  IN OUT FORM_EXPRESSION   *Expression
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  EXPRESSION_OPCODE       *OpCode;
  FORM_BROWSER_STATEMENT  *Question;
  FORM_BROWSER_STATEMENT  *Question2;
  UINT16                  Index;
  EFI_HII_VALUE           Data1;
  EFI_HII_VALUE           Data2;
  EFI_HII_VALUE           Data3;
  FORM_EXPRESSION         *RuleExpression;
  EFI_HII_VALUE           *Value;
  INTN                    Result;
  CHAR16                  *StrPtr;
  CHAR16                  *NameValue;
  UINT32                  TempValue;
  LIST_ENTRY              *SubExpressionLink;
  FORM_EXPRESSION         *SubExpression;
  UINTN                   StackOffset;
  UINTN                   TempLength;
  CHAR16                  TempStr[5];
  UINT8                   DigitUint8;
  UINT8                   *TempBuffer;
  EFI_TIME                EfiTime;
  EFI_HII_VALUE           QuestionVal;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;
  UINTN                   Offset;
  H2O_IFR_GUID_PCD        PcdInfo;
  
 
  ZeroMem (&PcdInfo,sizeof(H2O_IFR_GUID_PCD));
  Offset = 0;
  //
  // Save current stack offset.
  //
  StackOffset = SaveExpressionEvaluationStackOffset ();

  ASSERT (Expression != NULL);
  Expression->Result.Type = EFI_IFR_TYPE_OTHER;

  Link = GetFirstNode (&Expression->OpCodeListHead);
  while (!IsNull (&Expression->OpCodeListHead, Link)) {
    OpCode = EXPRESSION_OPCODE_FROM_LINK (Link);

    Link = GetNextNode (&Expression->OpCodeListHead, Link);

    ZeroMem (&Data1, sizeof (EFI_HII_VALUE));
    ZeroMem (&Data2, sizeof (EFI_HII_VALUE));
    ZeroMem (&Data3, sizeof (EFI_HII_VALUE));

    Value = &Data3;
    Value->Type = EFI_IFR_TYPE_BOOLEAN;
    Status = EFI_SUCCESS;

    switch (OpCode->Operand) {
    //
    // Built-in functions
    //
    case EFI_IFR_EQ_ID_VAL_OP:
      Question = IdToQuestion (FormSet, Form, OpCode->QuestionId);
      if (Question == NULL) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Status = CompareValue (&Question->HiiValue, &OpCode->Value, &Result, NULL);
      if (Status == EFI_UNSUPPORTED) {
        Status = EFI_SUCCESS;
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      if (EFI_ERROR (Status)) {
        goto Done;
      }
      Value->Value.b = (BOOLEAN) ((Result == 0) ? TRUE : FALSE);
      break;

    case EFI_IFR_EQ_ID_ID_OP:
      Question = IdToQuestion (FormSet, Form, OpCode->QuestionId);
      if (Question == NULL) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Question2 = IdToQuestion (FormSet, Form, OpCode->QuestionId2);
      if (Question2 == NULL) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Status = CompareValue (&Question->HiiValue, &Question2->HiiValue, &Result, FormSet->HiiHandle);
      if (Status == EFI_UNSUPPORTED) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        Status = EFI_SUCCESS;
        break;
      }
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      Value->Value.b = (BOOLEAN) ((Result == 0) ? TRUE : FALSE);
      break;

    case EFI_IFR_EQ_ID_VAL_LIST_OP:
      Question = IdToQuestion (FormSet, Form, OpCode->QuestionId);
      if (Question == NULL) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Value->Value.b = FALSE;
      for (Index =0; Index < OpCode->ListLength; Index++) {
        if (Question->HiiValue.Value.u16 == OpCode->ValueList[Index]) {
          Value->Value.b = TRUE;
          break;
        }
      }
      break;

    case EFI_IFR_DUP_OP:
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      Status = PushExpression (Value);
      break;

    case EFI_IFR_QUESTION_REF1_OP:
    case EFI_IFR_THIS_OP:
      Question = IdToQuestion (FormSet, Form, OpCode->QuestionId);
      if (Question == NULL) {
        Status = EFI_NOT_FOUND;
        goto Done;
      }

      Value = &Question->HiiValue;
      break;

    case EFI_IFR_SECURITY_OP:
      Value->Value.b = CheckUserPrivilege (&OpCode->Guid);
      break;

    case EFI_IFR_GET_OP:
      //
      // Get Value from VarStore buffer, EFI VarStore, Name/Value VarStore.
      //
      Value->Type = EFI_IFR_TYPE_UNDEFINED;
      Value->Value.u8 = 0;
      if (OpCode->VarStorage != NULL) {
        switch (OpCode->VarStorage->Type) {
        case EFI_HII_VARSTORE_BUFFER:
        case EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER:
          //
          // Get value from Edit Buffer
          //
          Value->Type = OpCode->ValueType;
          CopyMem (&Value->Value, OpCode->VarStorage->EditBuffer + OpCode->VarStoreInfo.VarOffset, OpCode->ValueWidth);
          break;
        case EFI_HII_VARSTORE_NAME_VALUE:
          if (OpCode->ValueType != EFI_IFR_TYPE_STRING) {
            //
            // Get value from string except for STRING value.
            //
            Status = GetValueFromNameValueStorage (OpCode->VarStorage, OpCode->ValueName, &StrPtr, GetSetValueWithEditBuffer);
            ASSERT (StrPtr != NULL);
            if (!EFI_ERROR (Status) && StrPtr != NULL) {
              TempLength = StrLen (StrPtr);
              if (OpCode->ValueWidth >= ((TempLength + 1) / 2)) {
                Value->Type = OpCode->ValueType;
                TempBuffer = (UINT8 *) &Value->Value;
                ZeroMem (TempStr, sizeof (TempStr));
                for (Index = 0; Index < TempLength; Index ++) {
                  TempStr[0] = StrPtr[TempLength - Index - 1];
                  DigitUint8 = (UINT8) StrHexToUint64 (TempStr);
                  if ((Index & 1) == 0) {
                    TempBuffer [Index/2] = DigitUint8;
                  } else {
                    TempBuffer [Index/2] = (UINT8) ((DigitUint8 << 4) + TempBuffer [Index/2]);
                  }
                }
              }
            }
          }
          break;
        case EFI_HII_VARSTORE_EFI_VARIABLE:
          //
          // Get value from variable.
          //
          TempLength = OpCode->ValueWidth;
          Value->Type = OpCode->ValueType;
          Status = gRT->GetVariable (
                          OpCode->ValueName,
                          &OpCode->VarStorage->Guid,
                          NULL,
                          &TempLength,
                          &Value->Value
                          );
          if (EFI_ERROR (Status)) {
            Value->Type = EFI_IFR_TYPE_UNDEFINED;
            Value->Value.u8 = 0;
          }
          break;
        default:
          //
          // Not recognize storage.
          //
          Status = EFI_UNSUPPORTED;
          goto Done;
        }
      } else {
        //
        // For Time/Date Data
        //
        if (OpCode->ValueType != EFI_IFR_TYPE_DATE && OpCode->ValueType != EFI_IFR_TYPE_TIME) {
          //
          // Only support Data/Time data when storage doesn't exist.
          //
          Status = EFI_UNSUPPORTED;
          goto Done;
        }
        Status = gRT->GetTime (&EfiTime, NULL);
        if (!EFI_ERROR (Status)) {
          if (OpCode->ValueType == EFI_IFR_TYPE_DATE) {
            switch (OpCode->VarStoreInfo.VarOffset) {
            case 0x00:
              Value->Type = EFI_IFR_TYPE_NUM_SIZE_16;
              Value->Value.u16 = EfiTime.Year;
              break;
            case 0x02:
              Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
              Value->Value.u8 = EfiTime.Month;
              break;
            case 0x03:
              Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
              Value->Value.u8 = EfiTime.Day;
              break;
            default:
              //
              // Invalid Date field.
              //
              Status = EFI_INVALID_PARAMETER;
              goto Done;
            }
          } else {
            switch (OpCode->VarStoreInfo.VarOffset) {
            case 0x00:
              Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
              Value->Value.u8 = EfiTime.Hour;
              break;
            case 0x01:
              Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
              Value->Value.u8 = EfiTime.Minute;
              break;
            case 0x02:
              Value->Type = EFI_IFR_TYPE_NUM_SIZE_8;
              Value->Value.u8 = EfiTime.Second;
              break;
            default:
              //
              // Invalid Time field.
              //
              Status = EFI_INVALID_PARAMETER;
              goto Done;
            }
          }
        }
      }

      break;

    case EFI_IFR_QUESTION_REF3_OP:
      //
      // EFI_IFR_QUESTION_REF3
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Validate the expression value
      //
      if ((Value->Type > EFI_IFR_TYPE_NUM_SIZE_64) || (Value->Value.u64 > 0xffff)) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      if (OpCode->DevicePath != 0) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;

        StrPtr = GetString (OpCode->DevicePath, FormSet->HiiHandle);
        if (StrPtr != NULL && mPathFromText != NULL) {
          DevicePath = mPathFromText->ConvertTextToDevicePath(StrPtr);
          if (DevicePath != NULL && GetQuestionValueFromForm(DevicePath, NULL, &OpCode->Guid, Value->Value.u16, &QuestionVal)) {
            Value = &QuestionVal;
          }
          if (DevicePath != NULL) {
            FreePool (DevicePath);
          }
        }

        if (StrPtr != NULL) {
          FreePool (StrPtr);
        }
      } else if (CompareGuid (&OpCode->Guid, &gZeroGuid) != 0) {
        if (!GetQuestionValueFromForm(NULL, FormSet->HiiHandle, &OpCode->Guid, Value->Value.u16, &QuestionVal)){
          Value->Type = EFI_IFR_TYPE_UNDEFINED;
          break;
        }
        Value = &QuestionVal;
      } else {
        Question = IdToQuestion (FormSet, Form, Value->Value.u16);
        if (Question == NULL) {
          Value->Type = EFI_IFR_TYPE_UNDEFINED;
          break;
        }

        //
        // push the questions' value on to the expression stack
        //
        Value = &Question->HiiValue;
      }
      break;

    case EFI_IFR_GUID_OP:
        if (CompareGuid (&mH2OIfrExtGuid, (EFI_GUID *)(((UINT8 *)(OpCode->GuidOpCodeData)) + sizeof (EFI_IFR_OP_HEADER)))) {
          if ((((H2O_IFR_GUID_PCD *)((UINT8 *)(OpCode->GuidOpCodeData)))->Function) == H2O_IFR_EXT_PCD) {
             Status = ProcessOpCodeH2OPcd(((UINT8 *)(OpCode->GuidOpCodeData)),&PcdInfo,Value);
             if (EFI_ERROR(Status)) {
               return  Status;
             }
          }
        }        
      break;

    case EFI_IFR_RULE_REF_OP:
      //
      // Find expression for this rule
      //
      RuleExpression = RuleIdToExpression (Form, OpCode->RuleId);
      if (RuleExpression == NULL) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      //
      // Evaluate this rule expression
      //
      Status = EvaluateExpression (FormSet, Form, RuleExpression);
      if (EFI_ERROR (Status) || RuleExpression->Result.Type == EFI_IFR_TYPE_UNDEFINED) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Value = &RuleExpression->Result;
      break;

    case EFI_IFR_STRING_REF1_OP:
      Value->Type = EFI_IFR_TYPE_STRING;
      Value->Value.string = OpCode->Value.Value.string;
      break;

    //
    // Constant
    //
    case EFI_IFR_TRUE_OP:
    case EFI_IFR_FALSE_OP:
    case EFI_IFR_ONE_OP:
    case EFI_IFR_ONES_OP:
    case EFI_IFR_UINT8_OP:
    case EFI_IFR_UINT16_OP:
    case EFI_IFR_UINT32_OP:
    case EFI_IFR_UINT64_OP:
    case EFI_IFR_UNDEFINED_OP:
    case EFI_IFR_VERSION_OP:
    case EFI_IFR_ZERO_OP:
      Value = &OpCode->Value;
      break;

    //
    // unary-op
    //
    case EFI_IFR_LENGTH_OP:
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      if (Value->Type != EFI_IFR_TYPE_STRING && Value->Type != EFI_IFR_TYPE_BUFFER) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      if (Value->Type == EFI_IFR_TYPE_STRING) {
        StrPtr = GetString (Value->Value.string, FormSet->HiiHandle);
        if (StrPtr == NULL) {
          Status = EFI_INVALID_PARAMETER;
          goto Done;
        }

        Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
        Value->Value.u64 = StrLen (StrPtr);
        FreePool (StrPtr);
      } else {
        Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
        Value->Value.u64 = Value->BufferLen;
        FreePool (Value->Buffer);
      }
      break;

    case EFI_IFR_NOT_OP:
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      if (Value->Type != EFI_IFR_TYPE_BOOLEAN) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }
      Value->Value.b = (BOOLEAN) (!Value->Value.b);
      break;

    case EFI_IFR_QUESTION_REF2_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Validate the expression value
      //
      if ((Value->Type > EFI_IFR_TYPE_NUM_SIZE_64) || (Value->Value.u64 > 0xffff)) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Question = IdToQuestion (FormSet, Form, Value->Value.u16);
      if (Question == NULL) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Value = &Question->HiiValue;
      break;

    case EFI_IFR_STRING_REF2_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Validate the expression value
      //
      if ((Value->Type > EFI_IFR_TYPE_NUM_SIZE_64) || (Value->Value.u64 > 0xffff)) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Value->Type = EFI_IFR_TYPE_STRING;
      StrPtr = GetString (Value->Value.u16, FormSet->HiiHandle);
      if (StrPtr == NULL) {
        //
        // If String not exit, push an empty string
        //
        Value->Value.string = NewString (gEmptyString, FormSet->HiiHandle);
      } else {
        Index = (UINT16) Value->Value.u64;
        Value->Value.string = Index;
        FreePool (StrPtr);
      }
      break;

    case EFI_IFR_TO_BOOLEAN_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Convert an expression to a Boolean
      //
      if (Value->Type <= EFI_IFR_TYPE_DATE) {
        //
        // When converting from an unsigned integer, zero will be converted to
        // FALSE and any other value will be converted to TRUE.
        //
        Value->Value.b = (BOOLEAN) (Value->Value.u64 != 0);

        Value->Type = EFI_IFR_TYPE_BOOLEAN;
      } else if (Value->Type == EFI_IFR_TYPE_STRING) {
        //
        // When converting from a string, if case-insensitive compare
        // with "true" is True, then push True. If a case-insensitive compare
        // with "false" is True, then push False. Otherwise, push Undefined.
        //
        StrPtr = GetString (Value->Value.string, FormSet->HiiHandle);
        if (StrPtr == NULL) {
          Status = EFI_INVALID_PARAMETER;
          goto Done;
        }

        IfrStrToUpper (StrPtr);
        if (StrCmp (StrPtr, L"TRUE") == 0){
          Value->Value.b = TRUE;
          Value->Type = EFI_IFR_TYPE_BOOLEAN;
        } else if (StrCmp (StrPtr, L"FALSE") == 0) {
          Value->Value.b = FALSE;
          Value->Type = EFI_IFR_TYPE_BOOLEAN;
        } else {
          Value->Type = EFI_IFR_TYPE_UNDEFINED;
        }
        FreePool (StrPtr);
      } else if (Value->Type == EFI_IFR_TYPE_BUFFER) {
        //
        // When converting from a buffer, if the buffer is all zeroes,
        // then push False. Otherwise push True.
        //
        for (Index =0; Index < Value->BufferLen; Index ++) {
          if (Value->Buffer[Index] != 0) {
            break;
          }
        }

        if (Index >= Value->BufferLen) {
          Value->Value.b = FALSE;
        } else {
          Value->Value.b = TRUE;
        }
        Value->Type = EFI_IFR_TYPE_BOOLEAN;
        FreePool (Value->Buffer);
      }
      break;

    case EFI_IFR_TO_STRING_OP:
      Status = IfrToString (FormSet, OpCode->Format, Value);
      break;

    case EFI_IFR_TO_UINT_OP:
      Status = IfrToUint (FormSet, Value);
      break;

    case EFI_IFR_TO_LOWER_OP:
    case EFI_IFR_TO_UPPER_OP:
      Status = InitializeUnicodeCollationProtocol ();
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      if (Value->Type != EFI_IFR_TYPE_STRING) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      StrPtr = GetString (Value->Value.string, FormSet->HiiHandle);
      if (StrPtr == NULL) {
        Status = EFI_NOT_FOUND;
        goto Done;
      }

      if (OpCode->Operand == EFI_IFR_TO_LOWER_OP) {
        mUnicodeCollation->StrLwr (mUnicodeCollation, StrPtr);
      } else {
        mUnicodeCollation->StrUpr (mUnicodeCollation, StrPtr);
      }
      Value->Value.string = NewString (StrPtr, FormSet->HiiHandle);
      FreePool (StrPtr);
      break;

    case EFI_IFR_BITWISE_NOT_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      if (Value->Type > EFI_IFR_TYPE_DATE) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;
      Value->Value.u64 = ~Value->Value.u64;
      break;

    case EFI_IFR_SET_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (Value);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      Data1.Type = EFI_IFR_TYPE_BOOLEAN;
      Data1.Value.b = FALSE;
      //
      // Set value to var storage buffer
      //
      if (OpCode->VarStorage != NULL) {
        switch (OpCode->VarStorage->Type) {
        case EFI_HII_VARSTORE_BUFFER:
        case EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER:
          CopyMem (OpCode->VarStorage->EditBuffer + OpCode->VarStoreInfo.VarOffset, &Value->Value, OpCode->ValueWidth);
          Data1.Value.b = TRUE;
          break;
        case EFI_HII_VARSTORE_NAME_VALUE:
          if (OpCode->ValueType != EFI_IFR_TYPE_STRING) {
            NameValue = AllocateZeroPool ((OpCode->ValueWidth * 2 + 1) * sizeof (CHAR16));
            ASSERT (Value != NULL);
            //
            // Convert Buffer to Hex String
            //
            TempBuffer = (UINT8 *) &Value->Value + OpCode->ValueWidth - 1;
            StrPtr = NameValue;
            for (Index = 0; Index < OpCode->ValueWidth; Index ++, TempBuffer --) {
              UnicodeValueToStringS (
                StrPtr,
                (OpCode->ValueWidth * 2 + 1) * sizeof (CHAR16) - ((UINTN)StrPtr - (UINTN)NameValue),
                PREFIX_ZERO | RADIX_HEX,
                *TempBuffer,
                2
                );
              StrPtr += StrnLenS (StrPtr, OpCode->ValueWidth * 2 + 1 - ((UINTN)StrPtr - (UINTN)NameValue) / sizeof (CHAR16));
            }
            Status = SetValueByName (OpCode->VarStorage, OpCode->ValueName, NameValue, GetSetValueWithEditBuffer, NULL);
            FreePool (NameValue);
            if (!EFI_ERROR (Status)) {
              Data1.Value.b = TRUE;
            }
          }
          break;
        case EFI_HII_VARSTORE_EFI_VARIABLE:
          Status = gRT->SetVariable (
                          OpCode->ValueName,
                          &OpCode->VarStorage->Guid,
                          OpCode->VarStorage->Attributes,
                          OpCode->ValueWidth,
                          &Value->Value
                          );
          if (!EFI_ERROR (Status)) {
            Data1.Value.b = TRUE;
          }
          break;
        default:
          //
          // Not recognize storage.
          //
          Status = EFI_UNSUPPORTED;
          goto Done;
          break;
        }
      } else {
        //
        // For Time/Date Data
        //
        if (OpCode->ValueType != EFI_IFR_TYPE_DATE && OpCode->ValueType != EFI_IFR_TYPE_TIME) {
          //
          // Only support Data/Time data when storage doesn't exist.
          //
          Status = EFI_UNSUPPORTED;
          goto Done;
        }
        Status = gRT->GetTime (&EfiTime, NULL);
        if (!EFI_ERROR (Status)) {
          if (OpCode->ValueType == EFI_IFR_TYPE_DATE) {
            switch (OpCode->VarStoreInfo.VarOffset) {
            case 0x00:
              EfiTime.Year = Value->Value.u16;
              break;
            case 0x02:
              EfiTime.Month = Value->Value.u8;
              break;
            case 0x03:
              EfiTime.Day = Value->Value.u8;
              break;
            default:
              //
              // Invalid Date field.
              //
              Status = EFI_INVALID_PARAMETER;
              goto Done;
            }
          } else {
            switch (OpCode->VarStoreInfo.VarOffset) {
            case 0x00:
              EfiTime.Hour = Value->Value.u8;
              break;
            case 0x01:
              EfiTime.Minute = Value->Value.u8;
              break;
            case 0x02:
              EfiTime.Second = Value->Value.u8;
              break;
            default:
              //
              // Invalid Time field.
              //
              Status = EFI_INVALID_PARAMETER;
              goto Done;
            }
          }
          Status = gRT->SetTime (&EfiTime);
          if (!EFI_ERROR (Status)) {
            Data1.Value.b = TRUE;
          }
        }
      }
      Value = &Data1;
      break;

    //
    // binary-op
    //
    case EFI_IFR_ADD_OP:
    case EFI_IFR_SUBTRACT_OP:
    case EFI_IFR_MULTIPLY_OP:
    case EFI_IFR_DIVIDE_OP:
    case EFI_IFR_MODULO_OP:
    case EFI_IFR_BITWISE_AND_OP:
    case EFI_IFR_BITWISE_OR_OP:
    case EFI_IFR_SHIFT_LEFT_OP:
    case EFI_IFR_SHIFT_RIGHT_OP:
      //
      // Pop an expression from the expression stack
      //
      Status = PopExpression (&Data2);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Pop another expression from the expression stack
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      if (Data2.Type > EFI_IFR_TYPE_DATE) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }


      if (Data1.Type > EFI_IFR_TYPE_DATE) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Value->Type = EFI_IFR_TYPE_NUM_SIZE_64;

      switch (OpCode->Operand) {
        case EFI_IFR_ADD_OP:
          Value->Value.u64 = Data1.Value.u64 + Data2.Value.u64;
          break;

        case EFI_IFR_SUBTRACT_OP:
          Value->Value.u64 = Data1.Value.u64 - Data2.Value.u64;
          break;

        case EFI_IFR_MULTIPLY_OP:
          Value->Value.u64 = MultU64x32 (Data1.Value.u64, (UINT32) Data2.Value.u64);
          break;

        case EFI_IFR_DIVIDE_OP:
          Value->Value.u64 = DivU64x32 (Data1.Value.u64, (UINT32) Data2.Value.u64);
          break;

        case EFI_IFR_MODULO_OP:
          DivU64x32Remainder  (Data1.Value.u64, (UINT32) Data2.Value.u64, &TempValue);
          Value->Value.u64 = TempValue;
          break;

        case EFI_IFR_BITWISE_AND_OP:
          Value->Value.u64 = Data1.Value.u64 & Data2.Value.u64;
          break;

        case EFI_IFR_BITWISE_OR_OP:
          Value->Value.u64 = Data1.Value.u64 | Data2.Value.u64;
          break;

        case EFI_IFR_SHIFT_LEFT_OP:
          Value->Value.u64 = LShiftU64 (Data1.Value.u64, (UINTN) Data2.Value.u64);
          break;

        case EFI_IFR_SHIFT_RIGHT_OP:
          Value->Value.u64 = RShiftU64 (Data1.Value.u64, (UINTN) Data2.Value.u64);
          break;

        default:
          break;
      }
      break;

    case EFI_IFR_AND_OP:
    case EFI_IFR_OR_OP:
      //
      // Two Boolean operator
      //
      Status = PopExpression (&Data2);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Pop another expression from the expression stack
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      if (Data2.Type != EFI_IFR_TYPE_BOOLEAN) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      if (Data1.Type != EFI_IFR_TYPE_BOOLEAN) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      if (OpCode->Operand == EFI_IFR_AND_OP) {
        Value->Value.b = (BOOLEAN) (Data1.Value.b && Data2.Value.b);
      } else {
        Value->Value.b = (BOOLEAN) (Data1.Value.b || Data2.Value.b);
      }
      break;

    case EFI_IFR_EQUAL_OP:
    case EFI_IFR_NOT_EQUAL_OP:
    case EFI_IFR_GREATER_EQUAL_OP:
    case EFI_IFR_GREATER_THAN_OP:
    case EFI_IFR_LESS_EQUAL_OP:
    case EFI_IFR_LESS_THAN_OP:
      //
      // Compare two integer, string, boolean or date/time
      //
      Status = PopExpression (&Data2);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Pop another expression from the expression stack
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      if (Data2.Type > EFI_IFR_TYPE_BOOLEAN &&
          Data2.Type != EFI_IFR_TYPE_STRING &&
          Data2.Type != EFI_IFR_TYPE_BUFFER) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      if (Data1.Type > EFI_IFR_TYPE_BOOLEAN &&
          Data1.Type != EFI_IFR_TYPE_STRING &&
          Data1.Type != EFI_IFR_TYPE_BUFFER) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      Status = CompareValue (&Data1, &Data2, &Result, FormSet->HiiHandle);
      if (Data1.Type == EFI_IFR_TYPE_BUFFER) {
        FreePool (Data1.Buffer);
        FreePool (Data2.Buffer);
      }

      if (Status == EFI_UNSUPPORTED) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        Status = EFI_SUCCESS;
        break;
      }

      if (EFI_ERROR (Status)) {
        goto Done;
      }

      switch (OpCode->Operand) {
      case EFI_IFR_EQUAL_OP:
        Value->Value.b = (BOOLEAN) ((Result == 0) ? TRUE : FALSE);
        break;

      case EFI_IFR_NOT_EQUAL_OP:
        Value->Value.b = (BOOLEAN) ((Result != 0) ? TRUE : FALSE);
        break;

      case EFI_IFR_GREATER_EQUAL_OP:
        Value->Value.b = (BOOLEAN) ((Result >= 0) ? TRUE : FALSE);
        break;

      case EFI_IFR_GREATER_THAN_OP:
        Value->Value.b = (BOOLEAN) ((Result > 0) ? TRUE : FALSE);
        break;

      case EFI_IFR_LESS_EQUAL_OP:
        Value->Value.b = (BOOLEAN) ((Result <= 0) ? TRUE : FALSE);
        break;

      case EFI_IFR_LESS_THAN_OP:
        Value->Value.b = (BOOLEAN) ((Result < 0) ? TRUE : FALSE);
        break;

      default:
        break;
      }
      break;

    case EFI_IFR_MATCH_OP:
      Status = InitializeUnicodeCollationProtocol ();
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      Status = IfrMatch (FormSet, Value);
      break;

    case EFI_IFR_CATENATE_OP:
      Status = IfrCatenate (FormSet, Value);
      break;

    //
    // ternary-op
    //
    case EFI_IFR_CONDITIONAL_OP:
      //
      // Pop third expression from the expression stack
      //
      Status = PopExpression (&Data3);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Pop second expression from the expression stack
      //
      Status = PopExpression (&Data2);
      if (EFI_ERROR (Status)) {
        goto Done;
      }

      //
      // Pop first expression from the expression stack
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      if (Data1.Type != EFI_IFR_TYPE_BOOLEAN) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        break;
      }

      if (Data1.Value.b) {
        Value = &Data3;
      } else {
        Value = &Data2;
      }
      break;

    case EFI_IFR_FIND_OP:
      Status = IfrFind (FormSet, OpCode->Format, Value);
      break;

    case EFI_IFR_MID_OP:
      Status = IfrMid (FormSet, Value);
      break;

    case EFI_IFR_TOKEN_OP:
      Status = IfrToken (FormSet, Value);
      break;

    case EFI_IFR_SPAN_OP:
      Status = IfrSpan (FormSet, OpCode->Flags, Value);
      break;

    case EFI_IFR_MAP_OP:
      //
      // Pop the check value
      //
      Status = PopExpression (&Data1);
      if (EFI_ERROR (Status)) {
        goto Done;
      }
      //
      // Check MapExpression list is valid.
      //
      if (OpCode->MapExpressionList.ForwardLink == NULL) {
        Status = EFI_INVALID_PARAMETER;
        goto Done;
      }
      //
      // Go through map expression list.
      //
      SubExpressionLink = GetFirstNode(&OpCode->MapExpressionList);
      while (!IsNull (&OpCode->MapExpressionList, SubExpressionLink)) {
        SubExpression = FORM_EXPRESSION_FROM_LINK (SubExpressionLink);
        //
        // Evaluate the first expression in this pair.
        //
        Status = EvaluateExpression (FormSet, Form, SubExpression);
        if (EFI_ERROR (Status)) {
          goto Done;
        }
        //
        // Compare the expression value with current value
        //
        if ((CompareValue (&Data1, &SubExpression->Result, &Result, NULL) == EFI_SUCCESS) && (Result == 0)) {
          //
          // Try get the map value.
          //
          SubExpressionLink = GetNextNode (&OpCode->MapExpressionList, SubExpressionLink);
          if (IsNull (&OpCode->MapExpressionList, SubExpressionLink)) {
            Status = EFI_INVALID_PARAMETER;
            goto Done;
          }
          SubExpression = FORM_EXPRESSION_FROM_LINK (SubExpressionLink);
          Status = EvaluateExpression (FormSet, Form, SubExpression);
          if (EFI_ERROR (Status)) {
            goto Done;
          }
          Value = &SubExpression->Result;
          break;
        }
        //
        // Skip the second expression on this pair.
        //
        SubExpressionLink = GetNextNode (&OpCode->MapExpressionList, SubExpressionLink);
        if (IsNull (&OpCode->MapExpressionList, SubExpressionLink)) {
          Status = EFI_INVALID_PARAMETER;
          goto Done;
        }
        //
        // Goto the first expression on next pair.
        //
        SubExpressionLink = GetNextNode (&OpCode->MapExpressionList, SubExpressionLink);
      }

      //
      // No map value is found.
      //
      if (IsNull (&OpCode->MapExpressionList, SubExpressionLink)) {
        Value->Type = EFI_IFR_TYPE_UNDEFINED;
        Value->Value.u8 = 0;
      }
      break;

    default:
      break;
    }
    if (EFI_ERROR (Status) || Value->Type == EFI_IFR_TYPE_UNDEFINED) {
      goto Done;
    }

    Status = PushExpression (Value);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

  //
  // Pop the final result from expression stack
  //
  Value = &Data1;
  Status = PopExpression (Value);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  //
  // After evaluating an expression, there should be only one value left on the expression stack
  //
  if (PopExpression (Value) != EFI_ACCESS_DENIED) {
    Status = EFI_INVALID_PARAMETER;
  }

Done:
  RestoreExpressionEvaluationStackOffset (StackOffset);
  if (!EFI_ERROR (Status)) {
    CopyMem (&Expression->Result, Value, sizeof (EFI_HII_VALUE));
  }

  return Status;
}

/**
  Check whether the result is TRUE or FALSE.

  For the EFI_HII_VALUE value type is numeric, return TRUE if the
  value is not 0.

  @param  Result             Input the result data.

  @retval TRUE               The result is TRUE.
  @retval FALSE              The result is FALSE.

**/
BOOLEAN
IsTrue (
  IN EFI_HII_VALUE     *Result
  )
{
  switch (Result->Type) {
  case EFI_IFR_TYPE_BOOLEAN:
    return Result->Value.b;

  case EFI_IFR_TYPE_NUM_SIZE_8:
    return (BOOLEAN)(Result->Value.u8 != 0);

  case EFI_IFR_TYPE_NUM_SIZE_16:
    return (BOOLEAN)(Result->Value.u16 != 0);

  case EFI_IFR_TYPE_NUM_SIZE_32:
    return (BOOLEAN)(Result->Value.u32 != 0);

  case EFI_IFR_TYPE_NUM_SIZE_64:
    return (BOOLEAN)(Result->Value.u64 != 0);

  default:
    return FALSE;
  }
}

/**
  Return the result of the expression list. Check the expression list and
  return the highest priority express result.
  Priority: DisableIf > SuppressIf > GrayOutIf > FALSE

  @param  [in] ExpList     The input expression list.
  @param  [in] Evaluate    Whether need to evaluate the expression first.
  @param  [in] FormSet     FormSet associated with this expression.
  @param  [in] Form        Form associated with this expression.

  @retval EXPRESS_RESULT      Return the higher priority express result.
                              DisableIf > SuppressIf > GrayOutIf > FALSE

**/
EXPRESS_RESULT
EvaluateExpressionList (
  IN FORM_EXPRESSION_LIST *ExpList,
  IN BOOLEAN              Evaluate,
  IN FORM_BROWSER_FORMSET *FormSet OPTIONAL,
  IN FORM_BROWSER_FORM    *Form OPTIONAL
  )
{
  UINTN              Index;
  EXPRESS_RESULT     ReturnVal;
  EXPRESS_RESULT     CompareOne;
  EFI_STATUS         Status;

  if (ExpList == NULL) {
    return ExpressFalse;
  }

  ASSERT(ExpList->Signature == FORM_EXPRESSION_LIST_SIGNATURE);
  Index     = 0;

  //
  // Check whether need to evaluate the expression first.
  //
  if (Evaluate) {
    while (ExpList->Count > Index) {
      Status = EvaluateExpression (FormSet, Form, ExpList->Expression[Index++]);
      if (EFI_ERROR (Status)) {
        return ExpressFalse;
      }
    }
  }

  //
  // Run the list of expressions.
  //
  ReturnVal = ExpressFalse;
  for (Index = 0; Index < ExpList->Count; Index++) {
    if (IsTrue (&ExpList->Expression[Index]->Result)) {
      switch (ExpList->Expression[Index]->Type) {
        case EFI_HII_EXPRESSION_SUPPRESS_IF:
          CompareOne = ExpressSuppress;
          break;

        case EFI_HII_EXPRESSION_GRAY_OUT_IF:
          CompareOne = ExpressGrayOut;
          break;

        case EFI_HII_EXPRESSION_DISABLE_IF:
          CompareOne = ExpressDisable;
          break;

        default:
          return ExpressFalse;
      }

      ReturnVal = ReturnVal < CompareOne ? CompareOne : ReturnVal;
    }
  }

  return ReturnVal;
}
