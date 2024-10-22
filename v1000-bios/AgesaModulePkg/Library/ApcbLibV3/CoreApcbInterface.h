/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB.h
 *
 * AGESA PSP Customization Block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 192403 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _CORE_APCB_INTERFACE_H_
#define _CORE_APCB_INTERFACE_H_

#define APCB_SIGNATURE  0x42435041ul

///
/// core APCB lib errors
///
typedef enum CORE_APCB_RET_CODE_T {
  CORE_APCB_OK                              = 0, ///< General Success
  CORE_APCB_ERR_GENERIC                     = 1, ///< Generic Error Code
  CORE_APCB_ERR_INVALID                     = 2, ///< APCB data is invalid
  CORE_APCB_ERR_GROUP_INVALID               = 3, ///< The group is invalid
  CORE_APCB_ERR_TYPE_INVALID                = 4, ///< The type is invalid
  CORE_APCB_ERR_SPACE_NOT_ENOUGH            = 5, ///< Space is not enough
  CORE_APCB_ERR_NO_ORIGIN_TYPE              = 6, ///< No origin type
  CORE_APCB_ERR_INVALID_TYPE_HANDLE         = 7, ///< Type handle is invalid
  CORE_APCB_ERR_LAST_TYPE                   = 8, ///< This is already the last APCB type within the current APCB binary
  CORE_APCB_ERR_TYPE_NOT_FOUND              = 9, ///< Type is not found
  CORE_APCB_ERR_TOKEN_NOT_FOUND             = 0xA, ///< Token is not found
  CORE_APCB_ERR_NON_APCB_TOKEN_CONTAINER    = 0xB, ///< Non APCB token container
  CORE_APCB_ERR_NO_MORE_TOKENS              = 0xC, ///< No more APCB tokens
} CORE_APCB_RET_CODE;

///
/// core APCB lib structure definitions
///
typedef struct {
  UINT32                token;
  union {
    BOOLEAN             bValue;
    UINT8               value8;
    UINT16              value16;
    UINT32              value32;
  } value;
} APCB_TOKEN_PAIR;

typedef struct _CORE_APCB_TYPE_ENTRY {
  UINT32                        typeHandle;
  UINT16                        groupId;
  UINT16                        typeId;
  UINT16                        instance;
  UINT8                         *dataBuf;
  UINT32                        dataSize;
  UINT8                         *apcbGroupEnding;
  UINT8                         *apcbEnding;
  struct _CORE_APCB_TYPE_ENTRY  *next;
} CORE_APCB_TYPE_ENTRY;

typedef struct _CORE_APCB_TOKEN_ENTRY {
  UINT32                        tokenHandle;
  APCB_TOKEN_PAIR               pair;
  UINT8                         attribute;
  struct _CORE_APCB_TOKEN_ENTRY *next;
} CORE_APCB_TOKEN_ENTRY;


///
/// core APCB lib function prototypes
///
CORE_APCB_RET_CODE
coreApcbGetVersion (
  IN         UINT8        *apcbBuf,
      OUT    UINT16       *StructVersion,
      OUT    UINT16       *DataVersion
  );


CORE_APCB_RET_CODE
coreApcbGetBool (
  IN         UINT8        *apcbBuf,
  IN         UINT8        priorityMask,
  IN         UINT16       boardMask,
  IN         UINT8        typeInstance,
  IN         UINT32       apcbToken,
      OUT    BOOLEAN      *bValue
  );

CORE_APCB_RET_CODE
coreApcbSetBool (
  IN  OUT     UINT8       *apcbBuf,
  IN          UINT32      sizeApcbBuf,
  IN          UINT8       priorityMask,
  IN          UINT16      boardMask,
  IN          UINT8       typeInstance,
  IN          UINT32      apcbToken,
  IN          BOOLEAN     bValue
  );

CORE_APCB_RET_CODE
coreApcbGet8 (
  IN         UINT8        *apcbBuf,
  IN         UINT8        priorityMask,
  IN         UINT16       boardMask,
  IN         UINT8        typeInstance,
  IN         UINT32       apcbToken,
      OUT    UINT8        *value8
  );

CORE_APCB_RET_CODE
coreApcbSet8 (
  IN  OUT    UINT8        *apcbBuf,
  IN         UINT32       sizeApcbBuf,
  IN         UINT8        priorityMask,
  IN         UINT16       boardMask,
  IN         UINT8        typeInstance,
  IN         UINT32       apcbToken,
  IN         UINT8        value8
  );

CORE_APCB_RET_CODE
coreApcbGet16 (
  IN         UINT8        *apcbBuf,
  IN         UINT8        priorityMask,
  IN         UINT16       boardMask,
  IN         UINT8        typeInstance,
  IN         UINT32       apcbToken,
      OUT    UINT16       *value16
  );

CORE_APCB_RET_CODE
coreApcbSet16 (
  IN  OUT    UINT8        *apcbBuf,
  IN         UINT32       sizeApcbBuf,
  IN         UINT8        priorityMask,
  IN         UINT16       boardMask,
  IN         UINT8        typeInstance,
  IN         UINT32       apcbToken,
  IN         UINT16       value16
  );

CORE_APCB_RET_CODE
coreApcbGet32 (
  IN          UINT8       *apcbBuf,
  IN          UINT8       priorityMask,
  IN          UINT16      boardMask,
  IN          UINT8       typeInstance,
  IN          UINT32      apcbToken,
      OUT     UINT32      *value32
  );

CORE_APCB_RET_CODE
coreApcbSet32 (
  IN  OUT     UINT8       *apcbBuf,
  IN          UINT32      sizeApcbBuf,
  IN          UINT8       priorityMask,
  IN          UINT16      boardMask,
  IN          UINT8       typeInstance,
  IN          UINT32      apcbToken,
  IN          UINT32      value32
  );;

VOID
coreApcbFreeTypeChain (
  IN          CORE_APCB_TYPE_ENTRY  *coreApcbType
  );

VOID
coreApcbFreeTokenChain (
  IN          CORE_APCB_TOKEN_ENTRY  *coreApcbToken
  );

CORE_APCB_RET_CODE
coreApcbGetFirstType (
  IN        UINT8       *apcbBuf,
      OUT   UINT16      *groupId,
      OUT   UINT16      *typeId,
      OUT   UINT8       *priorityMask,
      OUT   UINT16      *boardMask,
      OUT   UINT16      *instance,
      OUT   UINT8       **dataBuf,
      OUT   UINT32      *dataSize,
      OUT   UINT32      *typeHandle
  );

CORE_APCB_RET_CODE
coreApcbGetNextType (
  IN  OUT     UINT32      *typeHandle,
      OUT     UINT16      *groupId,
      OUT     UINT16      *typeId,
      OUT     UINT8       *priorityMask,
      OUT     UINT16      *boardMask,
      OUT     UINT16      *instance,
      OUT     UINT8       **dataBuf,
      OUT     UINT32      *dataSize
  );

CORE_APCB_RET_CODE
coreApcbGetType (
  IN           UINT8       *apcbBuf,
  IN           UINT16      groupId,
  IN           UINT16      typeId,
  IN           UINT8       priorityMask,
  IN           UINT16      boardMask,
  IN           UINT16      instance,
      OUT      UINT8       **dataBuf,
      OUT      UINT32      *dataSize
  );

CORE_APCB_RET_CODE
coreApcbSetType (
  IN           UINT8       *apcbBuf,
  IN           UINT32      sizeApcbBuf,
  IN           UINT16      groupId,
  IN           UINT16      typeId,
  IN           UINT8       priorityMask,
  IN           UINT16      boardMask,
  IN           UINT16      instance,
  IN           UINT8       *dataBuf,
  IN           UINT32      dataSize
  );

CORE_APCB_RET_CODE
coreApcbGetFirstToken (
  IN          UINT32      typeHandle,
      OUT     UINT32      *token,
      OUT     VOID        *value,
      OUT     UINT8       *attribute,
      OUT     UINT32      *tokenHandle
  );

CORE_APCB_RET_CODE
coreApcbGetNextToken (
  IN  OUT     UINT32      *tokenHandle,
      OUT     UINT32      *token,
      OUT     VOID        *value,
      OUT     UINT8       *attribute
  );


#endif

