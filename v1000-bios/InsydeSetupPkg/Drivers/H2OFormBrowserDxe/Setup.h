/** @file
Private MACRO, structure and function definitions for Setup Browser module.

;******************************************************************************
;* Copyright (c) 2013 - 2019, Insyde Software Corp. All Rights Reserved.
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

#ifndef _SETUP_H_
#define _SETUP_H_


#include <PiDxe.h>

#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/FormBrowserEx2.h>
#include <Protocol/DisplayProtocol.h>
#include <Protocol/DevicePath.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include <Protocol/UserManager.h>
#include <Protocol/DevicePathFromText.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/HiiFormMapMethodGuid.h>
#include <Guid/H2OCp.h>

#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/H2OCpLib.h>

#include "Colors.h"
#include <Library/UefiHiiServicesLib.h>
#include <Protocol/H2OFormBrowser.h>
//
// This is the generated header file which includes whatever needs to be exported (strings + IFR)
//
#include "H2OFormBrowserDxeStrDefs.h"

//extern BOOLEAN    mNvUpdateRequiredInOtherFormSet;
extern UINT8  H2OFormBrowserDxeStrings[];
extern UINT32 H2OFormBrowserDxeStringsTotalSize;

#define UI_ACTION_NONE               0
#define UI_ACTION_REFRESH_FORM       1
#define UI_ACTION_REFRESH_FORMSET    2
#define UI_ACTION_EXIT               3

//
// Do nothing.
//
#define BROWSER_ACTION_NONE         BIT16
//
// ESC Exit
//
#define BROWSER_ACTION_FORM_EXIT    BIT17

#define BROWSER_SUCCESS             0x0
#define BROWSER_ERROR               BIT31
#define BROWSER_SUBMIT_FAIL         BROWSER_ERROR | 0x01
#define BROWSER_NO_SUBMIT_IF        BROWSER_ERROR | 0x02
#define BROWSER_FORM_NOT_FOUND      BROWSER_ERROR | 0x03
#define BROWSER_FORM_SUPPRESS       BROWSER_ERROR | 0x04
#define BROWSER_PROTOCOL_NOT_FOUND  BROWSER_ERROR | 0x05
#define BROWSER_INCONSISTENT_IF     BROWSER_ERROR | 0x06
#define BROWSER_WARNING_IF          BROWSER_ERROR | 0x07

#define FORM_DISPLAY_ENGINE_STATEMENT_VERSION_1  0x10000
#define FORM_DISPLAY_ENGINE_VERSION_1            0x10000

//
// Screen definitions
//
#define BANNER_HEIGHT                 6
#define BANNER_COLUMNS                3
#define BANNER_LEFT_COLUMN_INDENT     1

#define FRONT_PAGE_HEADER_HEIGHT      6
#define NONE_FRONT_PAGE_HEADER_HEIGHT 3
#define LEFT_SKIPPED_COLUMNS          3
#define FOOTER_HEIGHT                 4
#define STATUS_BAR_HEIGHT             1
#define SCROLL_ARROW_HEIGHT           1
#define POPUP_PAD_SPACE_COUNT         5
#define POPUP_FRAME_WIDTH             2

//
// Definition for function key setting
//
#define NONE_FUNCTION_KEY_SETTING     0
#define ENABLE_FUNCTION_KEY_SETTING   1

#define UINT64_MAX                    0xFFFFFFFFFFFFFFFFULL
typedef struct {
  EFI_GUID  FormSetGuid;
  UINTN     KeySetting;
} FUNCTIION_KEY_SETTING;


typedef enum {
  UnlockPasswordState,
  ModifyPasswordState,
  NewPasswordState,
  PasswordStateMax
} PASSWORD_STATE;

//
// Character definitions
//
#define CHAR_SPACE              0x0020
#define UPPER_LOWER_CASE_OFFSET 0x20

//
// Time definitions
//
#define ONE_SECOND  10000000

//
// Display definitions
//
#define LEFT_HYPER_DELIMITER      L'<'
#define RIGHT_HYPER_DELIMITER     L'>'

#define LEFT_ONEOF_DELIMITER      L'<'
#define RIGHT_ONEOF_DELIMITER     L'>'

#define LEFT_NUMERIC_DELIMITER    L'['
#define RIGHT_NUMERIC_DELIMITER   L']'

#define LEFT_CHECKBOX_DELIMITER   L'['
#define RIGHT_CHECKBOX_DELIMITER  L']'

#define CHECK_ON                  L'X'
#define CHECK_OFF                 L' '

#define TIME_SEPARATOR            L':'
#define DATE_SEPARATOR            L'/'

#define YES_ANSWER                L'Y'
#define NO_ANSWER                 L'N'

//
// This is the Input Error Message
//
#define INPUT_ERROR 1

//
// This is the NV RAM update required Message
//
#define NV_UPDATE_REQUIRED  2

//
// Refresh the Status Bar with flags
//
#define REFRESH_STATUS_BAR  0xff

//
// Incremental string lenght of ConfigRequest
//
#define CONFIG_REQUEST_STRING_INCREMENTAL  1024

//
// HII value compare result
//
#define HII_VALUE_UNDEFINED     0
#define HII_VALUE_EQUAL         1
#define HII_VALUE_LESS_THAN     2
#define HII_VALUE_GREATER_THAN  3

//
// Incremental size of stack for expression
//
#define EXPRESSION_STACK_SIZE_INCREMENT    0x100

#define EFI_IFR_SPECIFICATION_VERSION  (UINT16) (((EFI_SYSTEM_TABLE_REVISION >> 16) << 8) | (((EFI_SYSTEM_TABLE_REVISION & 0xFFFF) / 10) << 4) | ((EFI_SYSTEM_TABLE_REVISION & 0xFFFF) % 10))


#define SETUP_DRIVER_SIGNATURE SIGNATURE_32 ('F', 'B', 'D', 'V')
typedef struct {
  UINT32                             Signature;

  EFI_HANDLE                         Handle;

  //
  // Produced protocol
  //
  EFI_FORM_BROWSER2_PROTOCOL            FormBrowser2;
  EFI_FORM_BROWSER_EXTENSION_PROTOCOL   FormBrowserEx;

  EDKII_FORM_BROWSER_EXTENSION2_PROTOCOL FormBrowserEx2;

} SETUP_DRIVER_PRIVATE_DATA;

typedef struct {
  EFI_STRING_ID  Banner[BANNER_HEIGHT][BANNER_COLUMNS];
} BANNER_DATA;

//
// IFR relative definition
//
#define EFI_HII_EXPRESSION_INCONSISTENT_IF   0
#define EFI_HII_EXPRESSION_NO_SUBMIT_IF      1
#define EFI_HII_EXPRESSION_GRAY_OUT_IF       2
#define EFI_HII_EXPRESSION_SUPPRESS_IF       3
#define EFI_HII_EXPRESSION_DISABLE_IF        4
#define EFI_HII_EXPRESSION_VALUE             5
#define EFI_HII_EXPRESSION_RULE              6
#define EFI_HII_EXPRESSION_READ              7
#define EFI_HII_EXPRESSION_WRITE             8
#define EFI_HII_EXPRESSION_WARNING_IF        9

#define EFI_HII_VARSTORE_BUFFER              0
#define EFI_HII_VARSTORE_NAME_VALUE          1
#define EFI_HII_VARSTORE_EFI_VARIABLE        2    // EFI Varstore type follow UEFI spec before 2.3.1.
#define EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER 3    // EFI varstore type follow UEFI spec 2.3.1 and later.

#define FORM_INCONSISTENT_VALIDATION         0
#define FORM_NO_SUBMIT_VALIDATION            1

#define FORMSET_CLASS_PLATFORM_SETUP         0x0001
#define FORMSET_CLASS_FRONT_PAGE             0x0002

#define NAME_VALUE_NODE_SIGNATURE  SIGNATURE_32 ('N', 'V', 'S', 'T')

typedef struct {
  UINTN            Signature;
  LIST_ENTRY       Link;
  CHAR16           *Name;
  CHAR16           *Value;
  CHAR16           *EditValue;
} NAME_VALUE_NODE;

#define NAME_VALUE_NODE_FROM_LINK(a)  CR (a, NAME_VALUE_NODE, Link, NAME_VALUE_NODE_SIGNATURE)

#define BROWSER_STORAGE_SIGNATURE  SIGNATURE_32 ('B', 'S', 'T', 'G')

typedef struct {
  UINTN            Signature;
  LIST_ENTRY       Link;

  UINT8            Type;           // Storage type

  BOOLEAN          Initialized;    // Whether this varstore is initialized, efi varstore not used.

  EFI_HII_HANDLE   HiiHandle;      // HiiHandle for this varstore, efi varstore not used.
  EFI_GUID         Guid;

  CHAR16           *Name;          // For EFI_IFR_VARSTORE
  UINT16           Size;
  UINT8            *Buffer;
  UINT8            *EditBuffer;    // Edit copy for Buffer Storage

  LIST_ENTRY       NameValueListHead; // List of NAME_VALUE_NODE

  UINT32           Attributes;     // For EFI_IFR_VARSTORE_EFI: EFI Variable attribute

  CHAR16           *ConfigHdr;     // <ConfigHdr>
  CHAR16           *ConfigRequest; // <ConfigRequest> = <ConfigHdr> + <RequestElement>
                                   // <RequestElement> includes all fields which is used by current form sets.
  UINTN            SpareStrLen;    // Spare length of ConfigRequest string buffer
} BROWSER_STORAGE;

#define BROWSER_STORAGE_FROM_LINK(a)  CR (a, BROWSER_STORAGE, Link, BROWSER_STORAGE_SIGNATURE)

#define FORMSET_STORAGE_SIGNATURE  SIGNATURE_32 ('F', 'S', 'T', 'G')

typedef struct {
  UINTN            Signature;
  LIST_ENTRY       Link;

  UINT16           VarStoreId;

  BROWSER_STORAGE  *BrowserStorage;

  CHAR16           *ConfigRequest; // <ConfigRequest> = <ConfigHdr> + <RequestElement>
  UINTN            ElementCount;   // Number of <RequestElement> in the <ConfigRequest>
  UINTN            SpareStrLen;    // Spare length of ConfigRequest string buffer
} FORMSET_STORAGE;

#define FORMSET_STORAGE_FROM_LINK(a)  CR (a, FORMSET_STORAGE, Link, FORMSET_STORAGE_SIGNATURE)

typedef union {
  EFI_STRING_ID         VarName;
  UINT16                VarOffset;
} VAR_STORE_INFO;

#define EXPRESSION_OPCODE_SIGNATURE  SIGNATURE_32 ('E', 'X', 'O', 'P')

typedef struct {
  UINTN             Signature;
  LIST_ENTRY        Link;

  UINT8             Operand;

  UINT8             Format;      // For EFI_IFR_TO_STRING, EFI_IFR_FIND
  UINT8             Flags;       // For EFI_IFR_SPAN
  UINT8             RuleId;      // For EFI_IFR_RULE_REF

  EFI_HII_VALUE     Value;       // For EFI_IFR_EQ_ID_VAL, EFI_IFR_UINT64, EFI_IFR_UINT32, EFI_IFR_UINT16, EFI_IFR_UINT8, EFI_IFR_STRING_REF1

  EFI_QUESTION_ID   QuestionId;  // For EFI_IFR_EQ_ID_ID, EFI_IFR_EQ_ID_VAL_LIST, EFI_IFR_QUESTION_REF1
  EFI_QUESTION_ID   QuestionId2;

  UINT16            ListLength;  // For EFI_IFR_EQ_ID_VAL_LIST
  UINT16            *ValueList;

  EFI_STRING_ID     DevicePath;  // For EFI_IFR_QUESTION_REF3_2, EFI_IFR_QUESTION_REF3_3
  EFI_GUID          Guid;

  BROWSER_STORAGE   *VarStorage; // For EFI_IFR_SET, EFI_IFR_GET
  VAR_STORE_INFO    VarStoreInfo;// For EFI_IFR_SET, EFI_IFR_GET
  UINT8             ValueType;   // For EFI_IFR_SET, EFI_IFR_GET
  UINT8             ValueWidth;  // For EFI_IFR_SET, EFI_IFR_GET
  CHAR16            *ValueName;  // For EFI_IFR_SET, EFI_IFR_GET
  VOID              *GuidOpCodeData;  // For EFI_IFR_GUID_OP
  LIST_ENTRY        MapExpressionList;   // nested expressions inside of Map opcode.
} EXPRESSION_OPCODE;

#define EXPRESSION_OPCODE_FROM_LINK(a)  CR (a, EXPRESSION_OPCODE, Link, EXPRESSION_OPCODE_SIGNATURE)

#define FORM_EXPRESSION_SIGNATURE  SIGNATURE_32 ('F', 'E', 'X', 'P')

typedef struct {
  UINTN             Signature;
  LIST_ENTRY        Link;

  UINT8             Type;            // Type for this expression

  UINT8             RuleId;          // For EFI_IFR_RULE only
  EFI_STRING_ID     Error;           // For EFI_IFR_NO_SUBMIT_IF, EFI_IFR_INCONSISTENT_IF only

  EFI_HII_VALUE     Result;          // Expression evaluation result

  UINT8             TimeOut;         // For EFI_IFR_WARNING_IF
  EFI_IFR_OP_HEADER *OpCode;         // Save the opcode buffer.

  LIST_ENTRY        OpCodeListHead;  // OpCodes consist of this expression (EXPRESSION_OPCODE)
} FORM_EXPRESSION;

#define FORM_EXPRESSION_FROM_LINK(a)  CR (a, FORM_EXPRESSION, Link, FORM_EXPRESSION_SIGNATURE)

#define FORM_EXPRESSION_LIST_SIGNATURE  SIGNATURE_32 ('F', 'E', 'X', 'R')

typedef struct {
    UINTN               Signature;
    UINTN               Count;
    FORM_EXPRESSION    *Expression[1];   // Array[Count] of expressions
} FORM_EXPRESSION_LIST;

#define QUESTION_DEFAULT_SIGNATURE  SIGNATURE_32 ('Q', 'D', 'F', 'T')

typedef struct {
  UINTN               Signature;
  LIST_ENTRY          Link;

  UINT16              DefaultId;
  EFI_HII_VALUE       Value;              // Default value

  FORM_EXPRESSION     *ValueExpression;   // Not-NULL indicates default value is provided by EFI_IFR_VALUE
} QUESTION_DEFAULT;

#define QUESTION_DEFAULT_FROM_LINK(a)  CR (a, QUESTION_DEFAULT, Link, QUESTION_DEFAULT_SIGNATURE)

#define QUESTION_OPTION_SIGNATURE  SIGNATURE_32 ('Q', 'O', 'P', 'T')

typedef struct {
  UINTN                Signature;
  LIST_ENTRY           Link;

  EFI_IFR_ONE_OF_OPTION  *OpCode;   // OneOfOption Data

  EFI_STRING_ID        Text;
  UINT8                Flags;
  EFI_HII_VALUE        Value;
  EFI_IMAGE_ID         ImageId;

  FORM_EXPRESSION_LIST *SuppressExpression; // Non-NULL indicates nested inside of SuppressIf
  LIST_ENTRY           PropertyListHead;
} QUESTION_OPTION;

#define QUESTION_OPTION_FROM_LINK(a)  CR (a, QUESTION_OPTION, Link, QUESTION_OPTION_SIGNATURE)

typedef enum {
  ExpressFalse = 0,
  ExpressGrayOut,
  ExpressSuppress,
  ExpressDisable
} EXPRESS_RESULT;

typedef enum {
  ExpressNone = 0,
  ExpressForm,
  ExpressStatement,
  ExpressOption
} EXPRESS_LEVEL;

typedef struct _FORM_BROWSER_STATEMENT FORM_BROWSER_STATEMENT;
typedef struct _FORM_BROWSER_FORM FORM_BROWSER_FORM;
typedef struct _FORM_BROWSER_STATEMENT_BUFFER  FORM_BROWSER_STATEMENT_BUFFER;
typedef struct _FORM_BROWSER_EXPRESSION_BUFFER FORM_BROWSER_EXPRESSION_BUFFER;

typedef struct {
  struct _FORM_BROWSER_FORMSET  *SrcFormSet;
  FORM_BROWSER_FORM             *SrcForm;
  FORM_BROWSER_STATEMENT        *SrcStatement;
} FB_IMPORT_INFO;

#define FORM_BROWSER_STATEMENT_BUFFER_SIGNATURE  SIGNATURE_32 ('F', 'B', 'S', 'B')
struct _FORM_BROWSER_STATEMENT_BUFFER {
  UINTN                     Signature;
  LIST_ENTRY                Link;

  FORM_BROWSER_STATEMENT    *StatementBufferList;
  UINT32                    StatementBufferListIndex;
  UINT32                    StatementBufferListCount;
};
#define FORM_BROWSER_STATEMENT_BUFFER_FROM_LINK(a)  CR (a, FORM_BROWSER_STATEMENT_BUFFER, Link, FORM_BROWSER_STATEMENT_BUFFER_SIGNATURE)

#define FORM_BROWSER_EXPRESSION_BUFFER_SIGNATURE  SIGNATURE_32 ('F', 'B', 'E', 'B')
struct _FORM_BROWSER_EXPRESSION_BUFFER {
  UINTN                     Signature;
  LIST_ENTRY                Link;

  EXPRESSION_OPCODE         *ExpressionBufferList;
  UINT32                    ExpressionBufferListIndex;
  UINT32                    ExpressionBufferListCount;
};
#define FORM_BROWSER_EXPRESSION_BUFFER_FROM_LINK(a)  CR (a, FORM_BROWSER_EXPRESSION_BUFFER, Link, FORM_BROWSER_EXPRESSION_BUFFER_SIGNATURE)

#define FORM_BROWSER_STATEMENT_SIGNATURE  SIGNATURE_32 ('F', 'S', 'T', 'A')

struct _FORM_BROWSER_STATEMENT{
  UINTN                 Signature;
  LIST_ENTRY            Link;

  UINT8                 Operand;        // The operator (first byte) of this Statement or Question
  EFI_IFR_OP_HEADER     *OpCode;

  //
  // Statement Header
  //
  EFI_STRING_ID         Prompt;
  EFI_STRING_ID         Help;
  EFI_STRING_ID         TextTwo;          // For EFI_IFR_TEXT

  //
  // Fake Question Id, used for statement not has true QuestionId.
  //
  EFI_QUESTION_ID       FakeQuestionId;

  //
  // Question Header
  //
  EFI_QUESTION_ID       QuestionId;       // The value of zero is reserved
  EFI_VARSTORE_ID       VarStoreId;       // A value of zero indicates no variable storage
  BROWSER_STORAGE       *Storage;
  VAR_STORE_INFO        VarStoreInfo;
  UINT16                StorageWidth;
  UINT8                 QuestionFlags;
  CHAR16                *VariableName;    // Name/Value or EFI Variable name
  CHAR16                *BlockName;       // Buffer storage block name: "OFFSET=...WIDTH=..."

  EFI_HII_VALUE         HiiValue;         // Edit copy for checkbox, numberic, oneof
  UINT8                 *BufferValue;     // Edit copy for string, password, orderedlist
  UINT8                 ValueType;        // Data type for orderedlist value array

  //
  // OpCode specific members
  //
  UINT8                 Flags;            // for EFI_IFR_CHECKBOX, EFI_IFR_DATE, EFI_IFR_NUMERIC, EFI_IFR_ONE_OF,
                                          // EFI_IFR_ORDERED_LIST, EFI_IFR_STRING,EFI_IFR_SUBTITLE,EFI_IFR_TIME, EFI_IFR_BANNER
  UINT32                ExtFlags;         // For H2O_IFR_EXT_EXT_FLAGS

  UINT8                 MaxContainers;    // for EFI_IFR_ORDERED_LIST

  UINT16                BannerLineNumber; // for EFI_IFR_BANNER, 1-based line number
  EFI_STRING_ID         QuestionConfig;   // for EFI_IFR_ACTION, if 0 then no configuration string will be processed

  UINT64                Minimum;          // for EFI_IFR_ONE_OF/EFI_IFR_NUMERIC, it's Min/Max value
  UINT64                Maximum;          // for EFI_IFR_STRING/EFI_IFR_PASSWORD, it's Min/Max length
  UINT64                Step;

  EFI_DEFAULT_ID        DefaultId;        // for EFI_IFR_RESET_BUTTON
  EFI_GUID              RefreshGuid;      // for EFI_IFR_REFRESH_ID
  BOOLEAN               Locked;           // Whether this statement is locked.
  BOOLEAN               ValueChanged;     // Whether this statement's value is changed.
  //
  // Get from IFR parsing
  //
  FORM_EXPRESSION       *ValueExpression;    // nested EFI_IFR_VALUE, provide Question value and indicate Question is ReadOnly
  LIST_ENTRY            DefaultListHead;     // nested EFI_IFR_DEFAULT list (QUESTION_DEFAULT), provide default values

  UINTN                 NumberOfOptions;
  LIST_ENTRY            OptionListHead;      // nested EFI_IFR_ONE_OF_OPTION list (QUESTION_OPTION)

  EFI_IMAGE_ID          ImageId;             // nested EFI_IFR_IMAGE
  UINT8                 RefreshInterval;     // nested EFI_IFR_REFRESH, refresh interval(in seconds) for Question value, 0 means no refresh

  FORM_BROWSER_STATEMENT *ParentStatement;

  LIST_ENTRY            InconsistentListHead;// nested inconsistent expression list (FORM_EXPRESSION)
  LIST_ENTRY            NoSubmitListHead;    // nested nosubmit expression list (FORM_EXPRESSION)
  LIST_ENTRY            WarningListHead;     // nested warning expression list (FORM_EXPRESSION)
  FORM_EXPRESSION_LIST  *Expression;         // nesting inside of GrayOutIf/DisableIf/SuppressIf

  FORM_EXPRESSION       *ReadExpression;     // nested EFI_IFR_READ, provide this question value by read expression.
  FORM_EXPRESSION       *WriteExpression;    // nested EFI_IFR_WRITE, evaluate write expression after this question value is set.

  H2O_STATEMENT_ID      StatementId;
  H2O_FORM_BROWSER_S    Statement;
  LIST_ENTRY            PropertyListHead;
  FB_IMPORT_INFO        *ImportInfo;
};

#define FORM_BROWSER_STATEMENT_FROM_LINK(a)       CR (a, FORM_BROWSER_STATEMENT, Link, FORM_BROWSER_STATEMENT_SIGNATURE)
#define FORM_BROWSER_STATEMENT_FROM_H2O_STATEMENT(a) CR (a, FORM_BROWSER_STATEMENT, Statement, FORM_BROWSER_STATEMENT_SIGNATURE)

#define FORM_BROWSER_CONFIG_REQUEST_SIGNATURE  SIGNATURE_32 ('F', 'C', 'R', 'S')
typedef struct {
  UINTN                 Signature;
  LIST_ENTRY            Link;

  CHAR16                *ConfigRequest; // <ConfigRequest> = <ConfigHdr> + <RequestElement>
  UINTN                 ElementCount;   // Number of <RequestElement> in the <ConfigRequest>
  UINTN                 SpareStrLen;

  BROWSER_STORAGE       *Storage;
} FORM_BROWSER_CONFIG_REQUEST;
#define FORM_BROWSER_CONFIG_REQUEST_FROM_LINK(a)  CR (a, FORM_BROWSER_CONFIG_REQUEST, Link, FORM_BROWSER_CONFIG_REQUEST_SIGNATURE)

#define FORM_BROWSER_FORM_SIGNATURE  SIGNATURE_32 ('F', 'F', 'R', 'M')
#define STANDARD_MAP_FORM_TYPE 0x01

struct _FORM_BROWSER_FORM {
  UINTN                Signature;
  LIST_ENTRY           Link;

  struct _FORM_BROWSER_FORMSET *FormSet;

  UINT16               FormId;               // FormId of normal form or formmap form.
  EFI_STRING_ID        FormTitle;            // FormTile of normal form, or FormMapMethod title of formmap form.
  UINT16               FormType;             // Specific form type for the different form.

  EFI_IMAGE_ID         ImageId;

  BOOLEAN              ModalForm;            // Whether this is a modal Form.
  BOOLEAN              Locked;               // Whether this Form is locked.
  EFI_GUID             RefreshGuid;          // Form refresh event guid.

  LIST_ENTRY           ExpressionListHead;   // List of Expressions (FORM_EXPRESSION)

  UINT16               NumberOfStatement;
  LIST_ENTRY           StatementListHead;    // List of Statements and Questions (FORM_BROWSER_STATEMENT)
  LIST_ENTRY           ConfigRequestHead;    // List of configreques for all storage.
  FORM_EXPRESSION_LIST *SuppressExpression;  // nesting inside of SuppressIf

  H2O_FORM_BROWSER_P   PageInfo;

  LIST_ENTRY           PropertyListHead;
};

#define FORM_BROWSER_FORM_FROM_LINK(a)            CR (a, FORM_BROWSER_FORM, Link, FORM_BROWSER_FORM_SIGNATURE)
#define FORM_BROWSER_FORM_FROM_H2O_PAGE(a)        CR (a, FORM_BROWSER_FORM, PageInfo, FORM_BROWSER_FORM_SIGNATURE)


#define FORMSET_DEFAULTSTORE_SIGNATURE  SIGNATURE_32 ('F', 'D', 'F', 'S')

typedef struct {
  UINTN            Signature;
  LIST_ENTRY       Link;

  UINT16           DefaultId;
  EFI_STRING_ID    DefaultName;
} FORMSET_DEFAULTSTORE;

#define FORMSET_DEFAULTSTORE_FROM_LINK(a)  CR (a, FORMSET_DEFAULTSTORE, Link, FORMSET_DEFAULTSTORE_SIGNATURE)

#define FORM_BROWSER_FORMSET_SIGNATURE  SIGNATURE_32 ('F', 'B', 'F', 'S')

typedef struct _FORM_BROWSER_FORMSET FORM_BROWSER_FORMSET;

struct _FORM_BROWSER_FORMSET {
  UINTN                           Signature;
  LIST_ENTRY                      Link;           // for gOldFormSet
  LIST_ENTRY                      DisplayLink;    // for H2O_FORM_BROWSER_PRIVATE_DATA.FormSetList
  EFI_HII_HANDLE                  HiiHandle;      // unique id for FormSet.
  EFI_HANDLE                      DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_DEVICE_PATH_PROTOCOL        *DevicePath;

  UINTN                           IfrBinaryLength;
  UINT8                           *IfrBinaryData;

  BOOLEAN                         QuestionInited;   // Have finished question initilization?
  EFI_GUID                        Guid;
  EFI_STRING_ID                   FormSetTitle;
  EFI_STRING_ID                   Help;
  UINT8                           NumberOfClassGuid;
  EFI_GUID                        ClassGuid[3];         // Up to three ClassGuid
  UINT16                          Class;                // Tiano extended Class code
  UINT16                          SubClass;             // Tiano extended Subclass code
  EFI_IMAGE_ID                    ImageId;

  FORM_BROWSER_STATEMENT          *StatementBuffer;     // Buffer for all Statements and Questions
  EXPRESSION_OPCODE               *ExpressionBuffer;    // Buffer for all Expression OpCode

  LIST_ENTRY                      StatementListOSF;     // Statement list out side of the form.
  LIST_ENTRY                      StorageListHead;      // Storage list (FORMSET_STORAGE)
  LIST_ENTRY                      DefaultStoreListHead; // DefaultStore list (FORMSET_DEFAULTSTORE)
  LIST_ENTRY                      FormListHead;         // Form list (FORM_BROWSER_FORM)
  LIST_ENTRY                      ExpressionListHead;   // List of Expressions (FORM_EXPRESSION)
  BANNER_DATA                     BannerData;
  LIST_ENTRY                      PropertyListHead;

  LIST_ENTRY                      LinkStatementListHead;
  LIST_ENTRY                      FBStatementBufferListHead;
  LIST_ENTRY                      FBExpressionBufferListHead;
};

#define FORM_BROWSER_FORMSET_FROM_LINK(a)  CR (a, FORM_BROWSER_FORMSET, Link, FORM_BROWSER_FORMSET_SIGNATURE)
#define FORM_BROWSER_FORMSET_FROM_DISPLAY_LINK(a)  CR (a, FORM_BROWSER_FORMSET, DisplayLink, FORM_BROWSER_FORMSET_SIGNATURE)

typedef struct {
  LIST_ENTRY                      Link;
  FORM_BROWSER_STATEMENT          *Statement;
  EFI_IFR_REF4                    Ref4Opcode;
} LINK_STATEMENT_NODE;

#define LINK_STATEMENT_NODE_FROM_LINK(a) BASE_CR (a, LINK_STATEMENT_NODE, Link)

typedef struct {
  LIST_ENTRY   Link;
  EFI_EVENT    RefreshEvent;
} FORM_BROWSER_REFRESH_EVENT_NODE;

#define FORM_BROWSER_REFRESH_EVENT_FROM_LINK(a) BASE_CR (a, FORM_BROWSER_REFRESH_EVENT_NODE, Link)


typedef struct _UI_MENU_SELECTION UI_MENU_SELECTION;

struct _UI_MENU_SELECTION {
  EFI_HII_HANDLE  Handle;

  //
  // Target formset/form/Question information
  //
  EFI_GUID        FormSetGuid;
  UINT16          FormId;
  UINT16          QuestionId;
  UINTN           Sequence;  // used for time/date only.

  UINTN           TopRow;
  UINTN           BottomRow;
  UINTN           PromptCol;
  UINTN           OptionCol;
  UINTN           CurrentRow;

  //
  // Ation for Browser to taken:
  //   UI_ACTION_NONE            - navigation inside a form
  //   UI_ACTION_REFRESH_FORM    - re-evaluate expressions and repaint form
  //   UI_ACTION_REFRESH_FORMSET - re-parse formset IFR binary
  //
  UINTN           Action;

  //
  // Current selected fomset/form/Question
  //
  FORM_BROWSER_FORMSET    *FormSet;
  FORM_BROWSER_FORM       *Form;
  FORM_BROWSER_STATEMENT  *Statement;

  //
  // Whether the Form is editable
  //
  BOOLEAN                 FormEditable;

  FORM_ENTRY_INFO            *CurrentMenu;
};

#define BROWSER_CONTEXT_SIGNATURE  SIGNATURE_32 ('B', 'C', 'T', 'X')

typedef struct {
  UINTN                 Signature;
  LIST_ENTRY            Link;

  //
  // Globals defined in Setup.c
  //
  BANNER_DATA           *BannerData;
  BOOLEAN               ResetRequired;
  BOOLEAN               ExitRequired;
  EFI_HII_HANDLE        HiiHandle;
  UINT16                Direction;
  EFI_GUID              FormSetGuid;
  EFI_FORM_ID           FormId;
  UI_MENU_SELECTION     *Selection;
  EFI_SCREEN_DESCRIPTOR ScreenDimensions;
  CHAR16                *EnterString;
  CHAR16                *ConfirmPassword;
  CHAR16                *PasswordInvalid;
  CHAR16                *YesResponse;
  CHAR16                *NoResponse;

  LIST_ENTRY           FormHistoryList;
} BROWSER_CONTEXT;

#define BROWSER_CONTEXT_FROM_LINK(a)  CR (a, BROWSER_CONTEXT, Link, BROWSER_CONTEXT_SIGNATURE)

#define NEW_STRING_ID_STORE_SIGNATURE  SIGNATURE_32 ('N', 'S', 'I', 'S')

typedef struct {
  UINTN                 Signature;
  LIST_ENTRY            Link;

  EFI_HII_HANDLE        HiiHandle;

  LIST_ENTRY            UsedStringIdList;
  LIST_ENTRY            FreeStringIdList;
} NEW_STRING_ID_STORE;

#define NEW_STRING_ID_STORE_FROM_LINK(a)  CR (a, NEW_STRING_ID_STORE, Link, NEW_STRING_ID_STORE_SIGNATURE)

#define NEW_STRING_ID_INFO_SIGNATURE  SIGNATURE_32 ('N', 'S', 'I', 'I')

typedef struct {
  UINTN                 Signature;
  LIST_ENTRY            Link;

  EFI_STRING_ID         StringId;
} NEW_STRING_ID_INFO;

#define NEW_STRING_ID_INFO_FROM_LINK(a)  CR (a, NEW_STRING_ID_INFO, Link, NEW_STRING_ID_INFO_SIGNATURE)



//
// Scope for get defaut value. It may be GetDefaultForNoStorage, GetDefaultForStorage or GetDefaultForAll.
//
typedef enum {
  GetDefaultForNoStorage,       // Get default value for question which not has storage.
  GetDefaultForStorage,         // Get default value for question which has storage.
  GetDefaultForAll,             // Get default value for all questions.
  GetDefaultForMax              // Invalid value.
} BROWSER_GET_DEFAULT_VALUE;

//
// Get/set question value from/to.
//
typedef enum {
  GetSetValueWithEditBuffer,       // Get/Set question value from/to editbuffer in the storage.
  GetSetValueWithBuffer,           // Get/Set question value from/to buffer in the storage.
  GetSetValueWithHiiDriver,        // Get/Set question value from/to HII driver.
  GetSetValueWithMax               // Invalid value.
} GET_SET_QUESTION_VALUE_WITH;


extern EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL *mPathFromText;

extern UINT32                mClassOfVfr;
extern UINT32                mFunctionKeySetting;
extern BOOLEAN               gResetRequired;
extern BOOLEAN               gExitRequired;
extern EFI_HII_HANDLE        mHiiHandle;
extern EFI_SCREEN_DESCRIPTOR mScreenDimensions;

extern LIST_ENTRY            gBrowserFormSetList;
extern LIST_ENTRY            gBrowserHotKeyList;
extern BROWSER_SETTING_SCOPE gBrowserSettingScope;
extern EXIT_HANDLER          ExitHandlerFunction;
extern EFI_HII_HANDLE        mCurrentHiiHandle;
//extern SETUP_DRIVER_PRIVATE_DATA mPrivateData;
//
// Browser Global Strings
//

extern CHAR16            *gPressEnter;
extern CHAR16            *gEmptyString;
extern CHAR16            *gAreYouSure;
extern CHAR16            *gYesResponse;
extern CHAR16            *gNoResponse;
extern CHAR16            *gSaveChanges;
extern CHAR16            *gSpaceString;
extern CHAR16            *gSaveFailed;
extern CHAR16            *gFormNotFound;
extern CHAR16            *gFormSuppress;
extern CHAR16            *gProtocolNotFound;
extern CHAR16            *gBrowserError;

extern EFI_GUID          gTianoHiiIfrGuid;
extern EFI_GUID          gZeroGuid;
extern UI_MENU_SELECTION  *gCurrentSelection;
extern LIST_ENTRY        gBrowserStorageList;

//
// Global Procedure Defines
//
#include "Expression.h"

/**
  Initialize the HII String Token to the correct values.

**/
VOID
InitializeBrowserStrings (
  VOID
  );

/**
  Parse opcodes in the FormSet IFR binary.

  @param  FormSet                Pointer of the FormSet data structure.

  @retval EFI_SUCCESS            Opcode parse success.
  @retval Other                  Opcode parse fail.

**/
EFI_STATUS
ParseOpCodes (
  IN FORM_BROWSER_FORMSET              *FormSet
  );

EFI_STATUS
InitializeRequestElement (
  IN OUT FORM_BROWSER_FORMSET     *FormSet,
  IN OUT FORM_BROWSER_STATEMENT   *Question,
  IN OUT FORM_BROWSER_FORM        *Form
  );

VOID
DeleteInvalidBrowserStorage (
  VOID
  );

VOID
DestroyForm (
  IN     FORM_BROWSER_FORMSET  *FormSet,
  IN OUT FORM_BROWSER_FORM     *Form
  );

/**
  Free resources allocated for a FormSet.

  @param  FormSet                Pointer of the FormSet

**/
VOID
DestroyFormSet (
  IN OUT FORM_BROWSER_FORMSET  *FormSet
  );


/**
  Create a new string in HII Package List.

  @param  String                 The String to be added
  @param  HiiHandle              The package list in the HII database to insert the
                                 specified string.

  @return The output string.

**/
EFI_STRING_ID
NewString (
  IN  CHAR16                   *String,
  IN  EFI_HII_HANDLE           HiiHandle
  );

/**
  Delete a string from HII Package List.

  @param  StringId               Id of the string in HII database.
  @param  HiiHandle              The HII package list handle.

  @retval EFI_SUCCESS            The string was deleted successfully.

**/
EFI_STATUS
DeleteString (
  IN  EFI_STRING_ID            StringId,
  IN  EFI_HII_HANDLE           HiiHandle
  );

/**
  Get the string based on the StringId and HII Package List Handle.

  @param  Token                  The String's ID.
  @param  HiiHandle              The package list in the HII database to search for
                                 the specified string.

  @return The output string.

**/
CHAR16 *
GetString (
  IN  EFI_STRING_ID                Token,
  IN  EFI_HII_HANDLE               HiiHandle
  );

/**
  Get Value for given Name from a NameValue Storage.

  @param  Storage                The NameValue Storage.
  @param  Name                   The Name.
  @param  Value                  The retured Value.
  @param  GetValueFrom           Where to get source value, from EditValue or Value.

  @retval EFI_SUCCESS            Value found for given Name.
  @retval EFI_NOT_FOUND          No such Name found in NameValue storage.

**/
EFI_STATUS
GetValueFromNameValueStorage (
  IN BROWSER_STORAGE             *Storage,
  IN CHAR16                      *Name,
  IN OUT CHAR16                  **Value,
  IN GET_SET_QUESTION_VALUE_WITH GetValueFrom
  );

/**
  Set Value of given Name in a NameValue Storage.

  @param  Storage                The NameValue Storage.
  @param  Name                   The Name.
  @param  Value                  The Value to set.
  @param  SetValueTo             Whether update editValue or Value.
  @param  ReturnNode             The node use the input name.

  @retval EFI_SUCCESS            Value found for given Name.
  @retval EFI_NOT_FOUND          No such Name found in NameValue storage.

**/
EFI_STATUS
SetValueByName (
  IN BROWSER_STORAGE              *Storage,
  IN CONST CHAR16                 *Name,
  IN CONST CHAR16                 *Value,
  IN GET_SET_QUESTION_VALUE_WITH  SetValueTo,
  OUT NAME_VALUE_NODE             **ReturnNode
  );

/**
  Validate whether this question's value has changed.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               Question to be initialized.
  @param  GetValueFrom           Where to get value, may from editbuffer, buffer or hii driver.

  @retval TRUE                   Question's value has changed.
  @retval FALSE                  Question's value has not changed

**/
BOOLEAN
IsQuestionValueChanged (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT       *Question,
  IN GET_SET_QUESTION_VALUE_WITH      GetValueFrom
  );

/**
  Validate the FormSet. If the formset is not validate, remove it from the list.

  @param  FormSet                The input FormSet which need to validate.

  @retval TRUE                   The handle is validate.
  @retval FALSE                  The handle is invalidate.

**/
BOOLEAN
ValidateFormSet (
  FORM_BROWSER_FORMSET    *FormSet
  );

/**
  Update the ValueChanged status for questions.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  SettingScope           Setting Scope for Default action.

**/
VOID
UpdateStatementStatus (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN BROWSER_SETTING_SCOPE            SettingScope
  );

/**
  Get Question's current Value.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               Question to be initialized.
  @param  GetValueFrom           Where to get value, may from editbuffer, buffer or HII driver.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
GetQuestionValue (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT       *Question,
  IN GET_SET_QUESTION_VALUE_WITH      GetValueFrom
  );

/**
  Save Question Value to edit copy(cached) or Storage(uncached).

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               Pointer to the Question.
  @param  SetValueTo             Update the question value to editbuffer , buffer or HII driver.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
SetQuestionValue (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT       *Question,
  IN GET_SET_QUESTION_VALUE_WITH      SetValueTo
  );

/**
  Perform inconsistent check for a Form.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               The Question to be validated.
  @param  Type                   Validation type: InConsistent or NoSubmit

  @retval EFI_SUCCESS            Form validation pass.
  @retval other                  Form validation failed.

**/
EFI_STATUS
ValidateQuestion (
  IN  FORM_BROWSER_FORMSET            *FormSet,
  IN  FORM_BROWSER_FORM               *Form,
  IN  FORM_BROWSER_STATEMENT          *Question,
  IN  UINT32                           Type
  );


/**
  Discard data based on the input setting scope (Form, FormSet or System).

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  SettingScope           Setting Scope for Discard action.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupport SettingScope.

**/
EFI_STATUS
DiscardForm (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN BROWSER_SETTING_SCOPE            SettingScope
  );

/**
  Submit data based on the input Setting level (Form, FormSet or System).

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  SettingScope           Setting Scope for Submit action.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupport SettingScope.

**/
EFI_STATUS
SubmitForm (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN BROWSER_SETTING_SCOPE            SettingScope
  );

/**
  Reset Question to its default value.

  @param  FormSet                The Form Set.
  @param  Form                   The Form.
  @param  Question               The question.
  @param  DefaultId              The Class of the default.

  @retval EFI_SUCCESS            Question is reset to default value.

**/
EFI_STATUS
GetQuestionDefault (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN FORM_BROWSER_STATEMENT           *Question,
  IN UINT16                           DefaultId
  );

/**
  Get current setting of Questions.

  @param  FormSet                FormSet data structure.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
InitCurrentSetting (
  IN OUT FORM_BROWSER_FORMSET             *FormSet
  );

/**
  Initialize the internal data structure of a FormSet.

  @param  Handle                 PackageList Handle
  @param  FormSetGuid            GUID of a FormSet. If not specified (NULL or zero
                                 GUID), take the first FormSet found in package
                                 list.
  @param  FormSet                FormSet data structure.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_NOT_FOUND          The specified FormSet could not be found.

**/
EFI_STATUS
InitializeFormSet (
  IN  EFI_HII_HANDLE                   Handle,
  IN OUT EFI_GUID                      *FormSetGuid,
  OUT FORM_BROWSER_FORMSET             *FormSet
  );

/**
  Reset Questions to their initial value or default value in a Form, Formset or System.

  GetDefaultValueScope parameter decides which questions will reset
  to its default value.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  DefaultId              The Class of the default.
  @param  SettingScope           Setting Scope for Default action.
  @param  GetDefaultValueScope   Get default value scope.
  @param  Storage                Get default value only for this storage.
  @param  RetrieveValueFirst     Whether call the retrieve call back to
                                 get the initial value before get default
                                 value.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupport SettingScope.

**/
EFI_STATUS
ExtractDefault (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN UINT16                           DefaultId,
  IN BROWSER_SETTING_SCOPE            SettingScope,
  IN BROWSER_GET_DEFAULT_VALUE        GetDefaultValueScope,
  IN BROWSER_STORAGE                  *Storage,
  IN BOOLEAN                          RetrieveValueFirst
  );

/**
  Initialize Question's Edit copy from Storage.

  @param  Selection              Selection contains the information about
                                 the Selection, Form and FormSet to be displayed.
                                 Selection action may be updated in retrieve callback.
                                 If Selection is NULL, only initialize Question value.
  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
LoadFormConfig (
  IN OUT UI_MENU_SELECTION    *Selection,
  IN FORM_BROWSER_FORMSET     *FormSet,
  IN FORM_BROWSER_FORM        *Form
  );

/**
  Initialize Question's Edit copy from Storage for the whole FormSet.

  @param  Selection              Selection contains the information about
                                 the Selection, Form and FormSet to be displayed.
                                 Selection action may be updated in retrieve callback.
                                 If Selection is NULL, only initialize Question value.
  @param  FormSet                FormSet data structure.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
LoadFormSetConfig (
  IN OUT UI_MENU_SELECTION    *Selection,
  IN     FORM_BROWSER_FORMSET *FormSet
  );

/**
  Convert setting of Buffer Storage or NameValue Storage to <ConfigResp>.

  @param  Storage                The Storage to be conveted.
  @param  ConfigResp             The returned <ConfigResp>.
  @param  ConfigRequest          The ConfigRequest string.

  @retval EFI_SUCCESS            Convert success.
  @retval EFI_INVALID_PARAMETER  Incorrect storage type.

**/
EFI_STATUS
StorageToConfigResp (
  IN BROWSER_STORAGE         *Storage,
  IN CHAR16                  **ConfigResp,
  IN CHAR16                  *ConfigRequest,
  IN BOOLEAN                 GetEditBuf
  );

/**
  Convert <ConfigResp> to settings in Buffer Storage or NameValue Storage.

  @param  Storage                The Storage to receive the settings.
  @param  ConfigResp             The <ConfigResp> to be converted.

  @retval EFI_SUCCESS            Convert success.
  @retval EFI_INVALID_PARAMETER  Incorrect storage type.

**/
EFI_STATUS
ConfigRespToStorage (
  IN BROWSER_STORAGE         *Storage,
  IN CHAR16                  *ConfigResp
  );

/**
  Fill storage's edit copy with settings requested from Configuration Driver.

  @param  FormSet                FormSet data structure.
  @param  Storage                Buffer Storage.

**/
VOID
LoadStorage (
  IN FORM_BROWSER_FORMSET    *FormSet,
  IN FORMSET_STORAGE         *Storage
  );

/**
  Fetch the Ifr binary data of a FormSet.

  @param  Handle                 PackageList Handle
  @param  FormSetGuid            GUID of a FormSet. If not specified (NULL or zero
                                 GUID), take the first FormSet found in package
                                 list.
  @param  BinaryLength           The length of the FormSet IFR binary.
  @param  BinaryData             The buffer designed to receive the FormSet.

  @retval EFI_SUCCESS            Buffer filled with the requested FormSet.
                                 BufferLength was updated.
  @retval EFI_INVALID_PARAMETER  The handle is unknown.
  @retval EFI_NOT_FOUND          A Form or FormSet on the requested handle cannot
                                 be found with the requested FormId.

**/
EFI_STATUS
GetIfrBinaryData (
  IN  EFI_HII_HANDLE   Handle,
  IN OUT EFI_GUID      *FormSetGuid,
  OUT UINTN            *BinaryLength,
  OUT UINT8            **BinaryData
  );

/**
  Save globals used by previous call to SendForm(). SendForm() may be called from
  HiiConfigAccess.Callback(), this will cause SendForm() be reentried.
  So, save globals of previous call to SendForm() and restore them upon exit.

**/
VOID
SaveBrowserContext (
  VOID
  );

/**
  Restore globals used by previous call to SendForm().

**/
VOID
RestoreBrowserContext (
  VOID
  );

/**
  This is the routine which an external caller uses to direct the browser
  where to obtain it's information.


  @param This            The Form Browser protocol instanse.
  @param Handles         A pointer to an array of Handles.  If HandleCount > 1 we
                         display a list of the FormSets for the handles specified.
  @param HandleCount     The number of Handles specified in Handle.
  @param FormSetGuid     This field points to the EFI_GUID which must match the Guid
                         field in the EFI_IFR_FORM_SET op-code for the specified
                         forms-based package. If FormSetGuid is NULL, then this
                         function will display the first found forms package.
  @param FormId          This field specifies which EFI_IFR_FORM to render as the first
                         displayable page. If this field has a value of 0x0000, then
                         the forms browser will render the specified forms in their encoded order.
                         ScreenDimenions - This allows the browser to be called so that it occupies a
                         portion of the physical screen instead of dynamically determining the screen dimensions.
                         ActionRequest   - Points to the action recommended by the Form.
  @param ScreenDimensions Points to recommended form dimensions, including any non-content area, in
                          characters.
  @param ActionRequest       Points to the action recommended by the Form.

  @retval  EFI_SUCCESS            The function completed successfully.
  @retval  EFI_INVALID_PARAMETER  One of the parameters has an invalid value.
  @retval  EFI_NOT_FOUND          No valid forms could be found to display.

**/
EFI_STATUS
EFIAPI
SendForm (
  IN  CONST EFI_FORM_BROWSER2_PROTOCOL *This,
  IN  EFI_HII_HANDLE                   *Handles,
  IN  UINTN                            HandleCount,
  IN  EFI_GUID                         *FormSetGuid OPTIONAL,
  IN  UINT16                           FormId OPTIONAL,
  IN  CONST EFI_SCREEN_DESCRIPTOR      *ScreenDimensions OPTIONAL,
  OUT EFI_BROWSER_ACTION_REQUEST       *ActionRequest  OPTIONAL
  );

/**
  This function is called by a callback handler to retrieve uncommitted state
  data from the browser.

  @param  This                   A pointer to the EFI_FORM_BROWSER2_PROTOCOL
                                 instance.
  @param  ResultsDataSize        A pointer to the size of the buffer associated
                                 with ResultsData.
  @param  ResultsData            A string returned from an IFR browser or
                                 equivalent. The results string will have no
                                 routing information in them.
  @param  RetrieveData           A BOOLEAN field which allows an agent to retrieve
                                 (if RetrieveData = TRUE) data from the uncommitted
                                 browser state information or set (if RetrieveData
                                 = FALSE) data in the uncommitted browser state
                                 information.
  @param  VariableGuid           An optional field to indicate the target variable
                                 GUID name to use.
  @param  VariableName           An optional field to indicate the target
                                 human-readable variable name.

  @retval EFI_SUCCESS            The results have been distributed or are awaiting
                                 distribution.
  @retval EFI_BUFFER_TOO_SMALL   The ResultsDataSize specified was too small to
                                 contain the results data.

**/
EFI_STATUS
EFIAPI
BrowserCallback (
  IN CONST EFI_FORM_BROWSER2_PROTOCOL  *This,
  IN OUT UINTN                         *ResultsDataSize,
  IN OUT EFI_STRING                    ResultsData,
  IN BOOLEAN                           RetrieveData,
  IN CONST EFI_GUID                    *VariableGuid  OPTIONAL,
  IN CONST CHAR16                      *VariableName  OPTIONAL
  );

/**
  Find menu which will show next time.

  @param Selection       On input, Selection tell setup browser the information
                         about the Selection, form and formset to be displayed.
                         On output, Selection return the screen item that is selected
                         by user.
  @param SettingLevel    Input Settting level, if it is FormLevel, just exit current form.
                         else, we need to exit current formset.

  @retval TRUE           Exit current form.
  @retval FALSE          User press ESC and keep in current form.
**/
BOOLEAN
FindNextMenu (
  IN OUT UI_MENU_SELECTION        *Selection,
  IN       BROWSER_SETTING_SCOPE  SettingLevel
  );

/**
  check whether the form need to update the NV.

  @param  Form                Form data structure.

  @retval TRUE                   Need to update the NV.
  @retval FALSE                  No need to update the NV.
**/
BOOLEAN
IsNvUpdateRequiredForForm (
  IN FORM_BROWSER_FORM    *Form
  );

/**
  check whether the FormSet need to update the NV.

  @param  FormSet                FormSet data structure.

  @retval TRUE                   Need to update the NV.
  @retval FALSE                  No need to update the NV.
**/
BOOLEAN
IsNvUpdateRequiredForFormSet (
  IN FORM_BROWSER_FORMSET  *FormSet
  );

/**
  Call the call back function for the question and process the return action.

  @param Selection             On input, Selection tell setup browser the information
                               about the Selection, Form and FormSet to be displayed.
                               On output, Selection return the screen item that is selected
                               by user.
  @param Statement             The Question which need to call.
  @param Action                The action request.
  @param SkipSaveOrDiscard     Whether skip save or discard action.

  @retval EFI_SUCCESS          The call back function excutes successfully.
  @return Other value if the call back function failed to excute.
**/
EFI_STATUS
ProcessCallBackFunction (
  IN OUT UI_MENU_SELECTION               *Selection,
  IN     FORM_BROWSER_FORMSET            *FormSet,
  IN     FORM_BROWSER_FORM               *Form,
  IN     FORM_BROWSER_STATEMENT          *Question,
  IN     EFI_BROWSER_ACTION              Action,
  IN     BOOLEAN                         SkipSaveOrDiscard
  );

/**
  Call the retrieve type call back function for one question to get the initialize data.

  This function only used when in the initialize stage, because in this stage, the
  Selection->Form is not ready. For other case, use the ProcessCallBackFunction instead.

  @param ConfigAccess          The config access protocol produced by the HII driver.
  @param Statement             The Question which need to call.

  @retval EFI_SUCCESS          The call back function excutes successfully.
  @return Other value if the call back function failed to excute.
**/
EFI_STATUS
ProcessRetrieveForQuestion (
  IN     EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess,
  IN     FORM_BROWSER_STATEMENT          *Statement,
  IN     FORM_BROWSER_FORM               *Form,
  IN     FORM_BROWSER_FORMSET            *FormSet
  );

/**
  Find the matched FormSet context in the backup maintain list based on HiiHandle.

  @param Handle  The HII Handle.

  @return the found FormSet context. If no found, NULL will return.

**/
FORM_BROWSER_FORMSET *
GetFormSetFromHiiHandle (
  EFI_HII_HANDLE Handle
  );

/**
  Check whether the input HII handle is the FormSet that is being used.

  @param Handle  The HII Handle.

  @retval TRUE   HII handle is being used.
  @retval FALSE  HII handle is not being used.

**/
BOOLEAN
IsHiiHandleInBrowserContext (
  EFI_HII_HANDLE Handle
  );

/**
  Configure what scope the hot key will impact.
  All hot keys have the same scope. The mixed hot keys with the different level are not supported.
  If no scope is set, the default scope will be FormSet level.
  After all registered hot keys are removed, previous Scope can reset to another level.

  @param[in] Scope               Scope level to be set.

  @retval EFI_SUCCESS            Scope is set correctly.
  @retval EFI_INVALID_PARAMETER  Scope is not the valid value specified in BROWSER_SETTING_SCOPE.
  @retval EFI_UNSPPORTED         Scope level is different from current one that the registered hot keys have.

**/
EFI_STATUS
EFIAPI
SetScope (
  IN BROWSER_SETTING_SCOPE Scope
  );

/**
  Register the hot key with its browser action, or unregistered the hot key.
  Only support hot key that is not printable character (control key, function key, etc.).
  If the action value is zero, the hot key will be unregistered if it has been registered.
  If the same hot key has been registered, the new action and help string will override the previous ones.

  @param[in] KeyData     A pointer to a buffer that describes the keystroke
                         information for the hot key. Its type is EFI_INPUT_KEY to
                         be supported by all ConsoleIn devices.
  @param[in] Action      Action value that describes what action will be trigged when the hot key is pressed.
  @param[in] DefaultId   Specifies the type of defaults to retrieve, which is only for DEFAULT action.
  @param[in] HelpString  Help string that describes the hot key information.
                         Its value may be NULL for the unregistered hot key.

  @retval EFI_SUCCESS            Hot key is registered or unregistered.
  @retval EFI_INVALID_PARAMETER  KeyData is NULL.
  @retval EFI_NOT_FOUND          KeyData is not found to be unregistered.
  @retval EFI_UNSUPPORTED        Key represents a printable character. It is conflicted with Browser.
**/
EFI_STATUS
EFIAPI
RegisterHotKey (
  IN EFI_INPUT_KEY *KeyData,
  IN UINT32        Action,
  IN UINT16        DefaultId,
  IN EFI_STRING    HelpString OPTIONAL
  );

/**
  Register Exit handler function.
  When more than one handler function is registered, the latter one will override the previous one.
  When NULL handler is specified, the previous Exit handler will be unregistered.

  @param[in] Handler      Pointer to handler function.

**/
VOID
EFIAPI
RegiserExitHandler (
  IN EXIT_HANDLER Handler
  );

/**

  Check whether the browser data has been modified.

  @retval TRUE        Browser data is changed.
  @retval FALSE       No browser data is changed.

**/
BOOLEAN
EFIAPI
IsBrowserDataModified (
  VOID
  );

/**

  Execute the action requested by the Action parameter.

  @param[in] Action     Execute the request action.
  @param[in] DefaultId  The default Id info when need to load default value.

  @retval EFI_SUCCESS              Execute the request action succss.
  @retval EFI_INVALID_PARAMETER    The input action value is invalid.

**/
EFI_STATUS
EFIAPI
ExecuteAction (
  IN UINT32        Action,
  IN UINT16        DefaultId
  );

/**
  Create reminder to let user to choose save or discard the changed browser data.
  Caller can use it to actively check the changed browser data.

  @retval BROWSER_NO_CHANGES       No browser data is changed.
  @retval BROWSER_SAVE_CHANGES     The changed browser data is saved.
  @retval BROWSER_DISCARD_CHANGES  The changed browser data is discard.
  @retval BROWSER_KEEP_CURRENT     Browser keep current changes.

**/
UINT32
EFIAPI
SaveReminder (
  VOID
  );

/**
  Find the registered HotKey based on KeyData.

  @param[in] KeyData     A pointer to a buffer that describes the keystroke
                         information for the hot key.

  @return The registered HotKey context. If no found, NULL will return.
**/
BROWSER_HOT_KEY *
GetHotKeyFromRegisterList (
  IN EFI_INPUT_KEY *KeyData
  );

/**

  Get FORM_BROWSER_STATEMENT from FORM_DISPLAY_ENGINE_STATEMENT based on the OpCode info.

  @param DisplayStatement        The input FORM_DISPLAY_ENGINE_STATEMENT.

  @retval FORM_BROWSER_STATEMENT  The return FORM_BROWSER_STATEMENT info.

**/
FORM_BROWSER_STATEMENT *
GetBrowserStatement (
  IN FORM_DISPLAY_ENGINE_STATEMENT *DisplayStatement
  );

/**
  Password may be stored as encrypted by Configuration Driver. When change a
  password, user will be challenged with old password. To validate user input old
  password, we will send the clear text to Configuration Driver via Callback().
  Configuration driver is responsible to check the passed in password and return
  the validation result. If validation pass, state machine in password Callback()
  will transit from BROWSER_STATE_VALIDATE_PASSWORD to BROWSER_STATE_SET_PASSWORD.
  After user type in new password twice, Callback() will be invoked to send the
  new password to Configuration Driver.

  @param  Selection              Pointer to UI_MENU_SELECTION.
  @param  MenuOption             The MenuOption for this password Question.
  @param  String                 The clear text of password.

  @retval EFI_NOT_AVAILABLE_YET  Callback() request to terminate password input.
  @return In state of BROWSER_STATE_VALIDATE_PASSWORD:
  @retval EFI_SUCCESS            Password correct, Browser will prompt for new
                                 password.
  @retval EFI_NOT_READY          Password incorrect, Browser will show error
                                 message.
  @retval Other                  Browser will do nothing.
  @return In state of BROWSER_STATE_SET_PASSWORD:
  @retval EFI_SUCCESS            Set password success.
  @retval Other                  Set password failed.

**/
EFI_STATUS
PasswordCallback (
  IN  UI_MENU_SELECTION           *Selection,
  IN  CHAR16                      *String
  );

/**
  Display error message for invalid password.

**/
VOID
PasswordInvalid (
  VOID
  );

/**
  The worker function that send the displays to the screen. On output,
  the selection made by user is returned.

  @param Selection       On input, Selection tell setup browser the information
                         about the Selection, form and formset to be displayed.
                         On output, Selection return the screen item that is selected
                         by user.

  @retval EFI_SUCCESS    The page is displayed successfully.
  @return Other value if the page failed to be diplayed.

**/
EFI_STATUS
SetupBrowser (
  IN OUT UI_MENU_SELECTION    *Selection
  );

/**
  Free up the resource allocated for all strings required
  by Setup Browser.

**/
VOID
FreeBrowserStrings (
  VOID
  );

/**
  Create a menu with specified formset GUID and form ID, and add it as a child
  of the given parent menu.

  @param  HiiHandle              Hii handle related to this formset.
  @param  FormSetGuid            The Formset Guid of menu to be added.
  @param  FormId                 The Form ID of menu to be added.
  @param  QuestionId             The question id of this menu to be added.

  @return A pointer to the newly added menu or NULL if memory is insufficient.

**/
FORM_ENTRY_INFO *
UiAddMenuList (
  IN EFI_HII_HANDLE       HiiHandle,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId,
  IN UINT16               QuestionId
  );

/**
  Search Menu with given FormSetGuid and FormId in all cached menu list.

  @param  HiiHandle              HiiHandle for FormSet.
  @param  FormSetGuid            The Formset GUID of the menu to search.
  @param  FormId                 The Form ID of menu to search.

  @return A pointer to menu found or NULL if not found.

**/
FORM_ENTRY_INFO *
UiFindMenuList (
  IN EFI_HII_HANDLE       HiiHandle,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  );

/**
  Free Menu list linked list.

  @param  MenuListHead    One Menu list point in the menu list.

**/
VOID
UiFreeMenuList (
  LIST_ENTRY   *MenuListHead
  );

/**
  Find parent menu for current menu.

  @param  CurrentMenu    Current Menu

  @retval   The parent menu for current menu.
**/
FORM_ENTRY_INFO *
UiFindParentMenu (
  IN FORM_ENTRY_INFO  *CurrentMenu
  );

/**
  Search an Option of a Question by its value.

  @param  Question               The Question
  @param  OptionValue            Value for Option to be searched.

  @retval Pointer                Pointer to the found Option.
  @retval NULL                   Option not found.

**/
QUESTION_OPTION *
ValueToOption (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN EFI_HII_VALUE            *OptionValue
  );
/**
  Return data element in an Array by its Index.

  @param  Array                  The data array.
  @param  Type                   Type of the data in this array.
  @param  Index                  Zero based index for data in this array.

  @retval Value                  The data to be returned

**/
UINT64
GetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index
  );

/**
  Set value of a data element in an Array by its Index.

  @param  Array                  The data array.
  @param  Type                   Type of the data in this array.
  @param  Index                  Zero based index for data in this array.
  @param  Value                  The value to be set.

**/
VOID
SetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index,
  IN UINT64                   Value
  );

/**
  Check whether this value already in the array, if yes, return the index.

  @param  Array                  The data array.
  @param  Type                   Type of the data in this array.
  @param  Value                  The value to be find.
  @param  Index                  The index in the array which has same value with Value.

  @retval   TRUE Found the value in the array.
  @retval   FALSE Not found the value.

**/
BOOLEAN
FindArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINT64                   Value,
  OUT UINTN                   *Index OPTIONAL
  );

/**
   Compare two Hii value.

   @param  Value1                 Expression value to compare on left-hand.
   @param  Value2                 Expression value to compare on right-hand.
   @param  Result                 Return value after compare.
                                  retval 0                      Two operators equal.
                                  return Positive value if Value1 is greater than Value2.
                                  retval Negative value if Value1 is less than Value2.
   @param  HiiHandle              Only required for string compare.

   @retval other                  Could not perform compare on two values.
   @retval EFI_SUCCESS            Compare the value success.

**/
EFI_STATUS
CompareHiiValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  OUT INTN            *Result,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  );

/**
  Perform Password check.
  Passwork may be encrypted by driver that requires the specific check.

  @param  Form             Form where Password Statement is in.
  @param  Statement        Password statement
  @param  PasswordString   Password string to be checked. It may be NULL.
                           NULL means to restore password.
                           "" string can be used to checked whether old password does exist.

  @return Status     Status of Password check.
**/
EFI_STATUS
EFIAPI
PasswordCheck (
  IN FORM_DISPLAY_ENGINE_FORM      *Form,
  IN FORM_DISPLAY_ENGINE_STATEMENT *Statement,
  IN EFI_STRING                    PasswordString  OPTIONAL
  );

/**

  Get FORM_BROWSER_STATEMENT from FORM_DISPLAY_ENGINE_STATEMENT based on the OpCode info.

  @param DisplayStatement        The input FORM_DISPLAY_ENGINE_STATEMENT.

  @retval FORM_BROWSER_STATEMENT  The return FORM_BROWSER_STATEMENT info.

**/
FORM_BROWSER_STATEMENT *
GetBrowserStatement (
  IN FORM_DISPLAY_ENGINE_STATEMENT *DisplayStatement
  );

/**

  Initialize the Display form structure data.

**/
VOID
InitializeDisplayFormData (
  VOID
  );


/**
  Base on the current formset info, clean the ConfigRequest string in browser storage.

  @param  FormSet                Pointer of the FormSet

**/
VOID
CleanBrowserStorage (
  IN OUT FORM_BROWSER_FORMSET  *FormSet
  );

/**
  Find HII Handle in the HII database associated with given Device Path.

  If DevicePath is NULL, then ASSERT.

  @param  DevicePath             Device Path associated with the HII package list
                                 handle.
  @param  FormsetGuid            The formset guid for this formset.

  @retval Handle                 HII package list Handle associated with the Device
                                        Path.
  @retval NULL                   Hii Package list handle is not found.

**/
EFI_HII_HANDLE
DevicePathToHiiHandle (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  IN EFI_GUID                   *FormsetGuid
  );

/**
  Adjust the config request info, remove the request elements which already in AllConfigRequest string.

  @param  Storage                Form set Storage.
  @param  Request                The input request string.
  @param  RespString             Whether the input is ConfigRequest or ConfigResp format.

  @retval TRUE                   Has element not covered by current used elements, need to continue to call ExtractConfig
  @retval FALSE                  All elements covered by current used elements.

**/
BOOLEAN
ConfigRequestAdjust (
  IN  BROWSER_STORAGE         *Storage,
  IN  CHAR16                  *Request,
  IN  BOOLEAN                 RespString
  );

/**
  Perform question check.

  If one question has more than one check, process form high priority to low.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               The Question to be validated.

  @retval EFI_SUCCESS            Form validation pass.
  @retval other                  Form validation failed.

**/
EFI_STATUS
ValueChangedValidation (
  IN  FORM_BROWSER_FORMSET            *FormSet,
  IN  FORM_BROWSER_FORM               *Form,
  IN  FORM_BROWSER_STATEMENT          *Question
  );

/**
  Pop up the error info.

  @param      BrowserStatus    The input browser status.
  @param      HiiHandle        The HiiHandle for this error opcode.
  @param      OpCode           The opcode use to get the erro info and timeout value.
  @param      ErrorString      Error string used by BROWSER_NO_SUBMIT_IF.

**/
VOID
PopupErrorMessage (
  IN UINT32                BrowserStatus,
  IN EFI_HII_HANDLE        HiiHandle,
  IN EFI_IFR_OP_HEADER     *OpCode, OPTIONAL
  IN CHAR16                *ErrorString
  );

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
  );

UINTN
EFIAPI
ConfirmDataChange (
  VOID
  );

/**
  Compare two HII value.

  @param  Value1                 Expression value to compare on left-hand.
  @param  Value2                 Expression value to compare on right-hand.
  @param  Result                 Return value after compare.
                                 retval 0                      Two operators equal.
                                 return Positive value if Value1 is greater than Value2.
                                 retval Negative value if Value1 is less than Value2.
  @param  HiiHandle              Only required for string compare.

  @retval other                  Could not perform compare on two values.
  @retval EFI_SUCCESS            Compare the value success.

**/
EFI_STATUS
CompareValue (
  IN  EFI_HII_VALUE   *Value1,
  IN  EFI_HII_VALUE   *Value2,
  OUT INTN            *Result,
  IN  EFI_HII_HANDLE  HiiHandle OPTIONAL
  );

EFI_STATUS
ProcessGotoOpCode (
  IN OUT   FORM_BROWSER_STATEMENT      *Statement,
  IN OUT   UI_MENU_SELECTION           *Selection
  );

EFI_STATUS
ProcessQuestionConfig (
  IN  UI_MENU_SELECTION       *Selection,
  IN  FORM_BROWSER_STATEMENT  *Question
  );

EFI_STATUS
EFIAPI
FormDisplay (
  IN  FORM_DISPLAY_ENGINE_FORM  *FormData,
  OUT USER_INPUT                *UserInputData
  );

BOOLEAN
EFIAPI
FormExitPolicy (
  VOID
  );

EFI_HII_HANDLE
FormSetGuidToHiiHandle (
  EFI_GUID     *ComparingGuid
  );

#endif
