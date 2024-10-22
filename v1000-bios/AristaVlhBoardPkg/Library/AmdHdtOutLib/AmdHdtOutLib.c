/** @file
 AmdHdtOutLib for Dumping Report Status Code

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>

#include <Guid/StatusCodeDataTypeId.h>

#define MAX_LOCAL_BUFFER_SIZE   512
#define BUFFER_OVERFLOW         0xFFFF

// return status for debug print
typedef enum {
  IDS_DEBUG_PRINT_SUCCESS = 0,
  IDS_DEBUG_PRINT_BUFFER_OVERFLOW,
} IDS_DEBUG_PRINT_STATUS;

CHAR8 STATIC HexStr[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

//
// Also support coding convention rules for var arg macros
//
#define _INT_SIZE_OF(n) ((sizeof (n) + sizeof (UINTN) - 1) &~(sizeof (UINTN) - 1))
typedef CHAR8 *VA_LIST;
#undef VA_START
#undef VA_ARG
#undef VA_END
#define VA_START(ap, v) (ap = (VA_LIST) & (v) + _INT_SIZE_OF (v))
#define VA_ARG(ap, t)   (*(t *) ((ap += _INT_SIZE_OF (t)) - _INT_SIZE_OF (t)))
#define VA_END(ap)      (ap = (VA_LIST) 0)

#define LEFT_JUSTIFY    0x01
#define PREFIX_SIGN     0x02
#define PREFIX_BLANK    0x04
#define COMMA_TYPE      0x08
#define LONG_TYPE       0x10
#define PREFIX_ZERO     0x20

//Note a is from 0 to 63
#define DEBUG_PRINT_SHIFT(a)   ((UINT64)1 << a)
//If you change the Bitmap definition below, please change the Hash in ParseFilter of hdtout2008.pl accordingly
//Memory Masks
#define MEM_SETREG                                  DEBUG_PRINT_SHIFT (0)
#define MEM_GETREG                                  DEBUG_PRINT_SHIFT (1)
#define MEM_FLOW                                    DEBUG_PRINT_SHIFT (2)
#define MEM_STATUS                                  DEBUG_PRINT_SHIFT (3)
#define MEM_UNDEF_BF                                DEBUG_PRINT_SHIFT (4)
#define MEMORY_TRACE_RSV2                           DEBUG_PRINT_SHIFT (5)
#define MEMORY_TRACE_RSV3                           DEBUG_PRINT_SHIFT (6)
#define MEMORY_TRACE_RSV4                           DEBUG_PRINT_SHIFT (7)
#define MEMORY_TRACE_RSV5                           DEBUG_PRINT_SHIFT (8)
#define MEMORY_TRACE_RSV6                           DEBUG_PRINT_SHIFT (9)

//CPU Masks
#define CPU_TRACE                                   DEBUG_PRINT_SHIFT (10)
#define CPU_TRACE_RSV1                              DEBUG_PRINT_SHIFT (11)
#define CPU_TRACE_RSV2                              DEBUG_PRINT_SHIFT (12)
#define CPU_TRACE_RSV3                              DEBUG_PRINT_SHIFT (13)
#define CPU_TRACE_RSV4                              DEBUG_PRINT_SHIFT (14)
#define CPU_TRACE_RSV5                              DEBUG_PRINT_SHIFT (15)
#define CPU_TRACE_RSV6                              DEBUG_PRINT_SHIFT (16)
#define CPU_TRACE_RSV7                              DEBUG_PRINT_SHIFT (17)
#define CPU_TRACE_RSV8                              DEBUG_PRINT_SHIFT (18)
#define CPU_TRACE_RSV9                              DEBUG_PRINT_SHIFT (19)

//GNB Masks
#define GNB_TRACE                                   DEBUG_PRINT_SHIFT (20)
#define PCIE_MISC                                   DEBUG_PRINT_SHIFT (21)
#define PCIE_PORTREG_TRACE                          DEBUG_PRINT_SHIFT (22)
#define PCIE_HOSTREG_TRACE                          DEBUG_PRINT_SHIFT (23)
#define GNB_TRACE_RSV2                              DEBUG_PRINT_SHIFT (24)
#define NB_MISC                                     DEBUG_PRINT_SHIFT (25)
#define GNB_TRACE_RSV3                              DEBUG_PRINT_SHIFT (26)
#define GFX_MISC                                    DEBUG_PRINT_SHIFT (27)
#define NB_SMUREG_TRACE                             DEBUG_PRINT_SHIFT (28)
#define GNB_TRACE_RSV1                              DEBUG_PRINT_SHIFT (29)

//Topology Masks
#define TOPO_TRACE                                  DEBUG_PRINT_SHIFT (30)
#define TOPO_TRACE_RSV1                             DEBUG_PRINT_SHIFT (31)
#define TOPO_TRACE_RSV2                             DEBUG_PRINT_SHIFT (32)
#define TOPO_TRACE_RSV3                             DEBUG_PRINT_SHIFT (33)
#define TOPO_TRACE_RSV4                             DEBUG_PRINT_SHIFT (34)
#define TOPO_TRACE_RSV5                             DEBUG_PRINT_SHIFT (35)
#define TOPO_TRACE_RSV6                             DEBUG_PRINT_SHIFT (36)
#define TOPO_TRACE_RSV7                             DEBUG_PRINT_SHIFT (37)
#define TOPO_TRACE_RSV8                             DEBUG_PRINT_SHIFT (38)
#define TOPO_TRACE_RSV9                             DEBUG_PRINT_SHIFT (39)

//FCH Masks
#define FCH_TRACE                                    DEBUG_PRINT_SHIFT (40)
#define FCH_TRACE_RSV1                               DEBUG_PRINT_SHIFT (41)
#define FCH_TRACE_RSV2                               DEBUG_PRINT_SHIFT (42)
#define FCH_TRACE_RSV3                               DEBUG_PRINT_SHIFT (43)
#define FCH_TRACE_RSV4                               DEBUG_PRINT_SHIFT (44)
#define FCH_TRACE_RSV5                               DEBUG_PRINT_SHIFT (45)
#define FCH_TRACE_RSV6                               DEBUG_PRINT_SHIFT (46)
#define FCH_TRACE_RSV7                               DEBUG_PRINT_SHIFT (47)
#define FCH_TRACE_RSV8                               DEBUG_PRINT_SHIFT (48)
#define FCH_TRACE_RSV9                               DEBUG_PRINT_SHIFT (49)

//Other Masks
#define MAIN_FLOW                                    DEBUG_PRINT_SHIFT (50)
#define EVENT_LOG                                    DEBUG_PRINT_SHIFT (51)
#define PERFORMANCE_ANALYSE                          DEBUG_PRINT_SHIFT (52)

//Ids Masks
#define IDS_TRACE                                    DEBUG_PRINT_SHIFT (53)
#define BVM_TRACE                                    DEBUG_PRINT_SHIFT (54)
#define IDS_TRACE_RSV2                               DEBUG_PRINT_SHIFT (55)
#define IDS_TRACE_RSV3                               DEBUG_PRINT_SHIFT (56)

//S3
#define S3_TRACE                                     DEBUG_PRINT_SHIFT (57)

//Library function to read/write PCI/MSR registers
#define LIB_PCI_RD                                   DEBUG_PRINT_SHIFT (58)
#define LIB_PCI_WR                                   DEBUG_PRINT_SHIFT (59)

//AGESA test points
#define TEST_POINT                                   DEBUG_PRINT_SHIFT (60)

//Reserved for platform log
#define PLAT_RSV1                                    DEBUG_PRINT_SHIFT (61)
#define PLAT_RSV2                                    DEBUG_PRINT_SHIFT (62)
#define PLAT_RSV3                                    DEBUG_PRINT_SHIFT (63)

#define GNB_TRACE_DEFAULT\
      (\
      GNB_TRACE | PCIE_MISC | NB_MISC | GFX_MISC \
      )

#define GNB_TRACE_REG\
      (\
      PCIE_PORTREG_TRACE | PCIE_HOSTREG_TRACE | \
      NB_SMUREG_TRACE | GNB_TRACE_RSV1 \
      )

#define GNB_TRACE_ALL\
      (\
      GNB_TRACE_DEFAULT | GNB_TRACE_REG \
      )

#define CPU_TRACE_ALL\
      (\
      CPU_TRACE | CPU_TRACE_RSV1 | CPU_TRACE_RSV2 | CPU_TRACE_RSV3 | \
      CPU_TRACE_RSV4 | CPU_TRACE_RSV5 | CPU_TRACE_RSV6 | CPU_TRACE_RSV7 | \
      CPU_TRACE_RSV8 | CPU_TRACE_RSV9\
      )

#define MEMORY_TRACE_ALL\
      (\
      MEM_FLOW | MEM_GETREG | MEM_SETREG | MEM_STATUS | \
      MEMORY_TRACE_RSV1 | MEMORY_TRACE_RSV2 | MEMORY_TRACE_RSV3 | MEMORY_TRACE_RSV4 | \
      MEMORY_TRACE_RSV5 | MEMORY_TRACE_RSV6\
      )

#define TOPO_TRACE_ALL\
      (\
      TOPO_TRACE | TOPO_TRACE_RSV1 | TOPO_TRACE_RSV2 | TOPO_TRACE_RSV3 | \
      TOPO_TRACE_RSV4 | TOPO_TRACE_RSV5 | TOPO_TRACE_RSV6 | TOPO_TRACE_RSV7 | \
      TOPO_TRACE_RSV8 | TOPO_TRACE_RSV9\
      )

#define FCH_TRACE_ALL\
      (\
      FCH_TRACE | FCH_TRACE_RSV1 | FCH_TRACE_RSV2 | FCH_TRACE_RSV3 | \
      FCH_TRACE_RSV4 | FCH_TRACE_RSV5 | FCH_TRACE_RSV6 | FCH_TRACE_RSV7 | \
      FCH_TRACE_RSV8 | FCH_TRACE_RSV9\
      )

#define IDS_TRACE_ALL\
      (\
      IDS_TRACE | BVM_TRACE | IDS_TRACE_RSV2 | IDS_TRACE_RSV3\
      )

#define OTHER_TRACE_ALL\
      (\
      MAIN_FLOW | EVENT_LOG | PERFORMANCE_ANALYSE\
      )

#define IDS_TRACE_DEFAULT (0)
#define TRACE_MASK_ALL (0xFFFFFFFFFFFFFFFFull)

#define IDS_DEBUG_PRINT_MASK (GNB_TRACE_DEFAULT  | CPU_TRACE_ALL | MEM_FLOW | MEM_STATUS | TOPO_TRACE_ALL | FCH_TRACE_ALL | MAIN_FLOW | IDS_TRACE_DEFAULT | TEST_POINT)

/**
 *  Parses flag and width information from the Format string and returns the next index
 *  into the Format string that needs to be parsed. See file headed for details of Flag and Width.
 *
 *  @param[in]  Format  Current location in the AvSPrint format string.
 *  @param[out] Flags  Returns flags
 *  @param[out] Width  Returns width of element
 *  @param[out] Marker Vararg list that may be partially consumed and returned.
 *
 *  @retval Pointer indexed into the Format string for all the information parsed by this routine.
 *
 **/
CONST CHAR8 *
GetFlagsAndWidthEx (
  IN       CONST CHAR8 *Format,
     OUT   UINTN       *Flags,
     OUT   UINTN       *Width,
  IN OUT   VA_LIST     *Marker
  )
{
  UINTN   Count;
  BOOLEAN Done;

  *Flags = 0;
  *Width = 0;
  for (Done = FALSE; !Done; ) {
    Format++;

    switch (*Format) {

    case '-':
      *Flags |= LEFT_JUSTIFY;
      break;
    case '+':
      *Flags |= PREFIX_SIGN;
      break;
    case ' ':
      *Flags |= PREFIX_BLANK;
      break;
    case ',':
      *Flags |= COMMA_TYPE;
      break;
    case 'L':
    case 'l':
      *Flags |= LONG_TYPE;
      break;

    case '*':
      *Width = VA_ARG (*Marker, UINTN);
      break;

    case '0':
      *Flags |= PREFIX_ZERO;
      break;

    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      Count = 0;
      do {
        Count = (Count * 10) + *Format - '0';
        Format++;
      } while ((*Format >= '0')  &&  (*Format <= '9'));
      Format--;
      *Width = Count;
      break;

    default:
      Done = TRUE;
    }
  }
  return Format;
}

/**
 *
 *  @param[in,out] Buffer Location to place ascii hex string of Value.
 *  @param[in] Value  - Hex value to convert to a string in Buffer.
 *  @param[in] Flags  - Flags to use in printing Hex string, see file header for details.
 *  @param[in] Width  - Width of hex value.
 *  @param[in,out] BufferSize  - Size of input buffer
 *
 *  @retval Number of characters printed.
 **/

UINTN
ValueToHexStrEx (
  IN OUT   CHAR8   *Buffer,
  IN       UINT64      Value,
  IN       UINTN       Flags,
  IN       UINTN       Width,
  IN OUT   UINTN       *BufferSize
  )
{
  CHAR8   TempBuffer[30];
  CHAR8   *TempStr;
  CHAR8   Prefix;
  CHAR8   *BufferPtr;
  UINTN   Count;
  UINTN   Index;

  TempStr = TempBuffer;
  BufferPtr = Buffer;
  //
  // Count starts at one since we will null terminate. Each iteration of the
  // loop picks off one nibble. Oh yea TempStr ends up backwards
  //
  Count = 0;
  do {
    *(TempStr++) = HexStr[Value & 0x0f];
    Value >>= 4;
    Count++;
  } while (Value != 0);

  if (Flags & PREFIX_ZERO) {
    Prefix = '0';
  } else if (!(Flags & LEFT_JUSTIFY)) {
    Prefix = ' ';
  } else {
    Prefix = 0x00;
  }
  for (Index = Count; Index < Width; Index++) {
    *(TempStr++) = Prefix;
  }

  //
  // Reverse temp string into Buffer.
  //
  while (TempStr != TempBuffer) {
    (*BufferSize)--;
    if (*BufferSize == 0) {
      return BUFFER_OVERFLOW;
    }
    *(BufferPtr++) = *(--TempStr);
  }

  *BufferPtr = 0;
  return Index;
}

/**
 *  Prints a Value as a decimal number in Buffer
 *
 *  @param[in] Buffer  Location to place ascii decimal number string of Value.
 *  @param[in] Value  Decimal value to convert to a string in Buffer.
 *  @param[in] Flags  Flags to use in printing decimal string, see file header for details.
 *  @param[in,out] BufferSize  Size of input buffer
 *
 *  @retval Number of characters printed.
 *
**/

UINTN
ValueToStringEx (
  IN OUT  CHAR8   *Buffer,
  IN      INT32   Value,
  IN      UINTN   Flags,
  IN OUT  UINTN   *BufferSize
  )
{
  CHAR8           TempBuffer[30];
  CHAR8           *TempStr;
  CHAR8           *BufferPtr;
  UINTN           Count;
  UINTN           Remainder;

  ASSERT (*BufferSize);
  TempStr = TempBuffer;
  BufferPtr = Buffer;
  Count = 0;

  if (Value < 0) {
    (*BufferSize)--;
    if (*BufferSize == 0) {
      return BUFFER_OVERFLOW;
    }
    *(BufferPtr++) = '-';
    Value = - Value;
    Count++;
  }

  do {
    Remainder = Value % 10;
    Value /= 10;
    *(TempStr++) = (CHAR8)(Remainder + '0');
    Count++;
    if ((Flags & COMMA_TYPE) == COMMA_TYPE) {
      if (Count % 3 == 0) {
        *(TempStr++) = ',';
      }
    }
  } while (Value != 0);

  //
  // Reverse temp string into Buffer.
  //
  while (TempStr != TempBuffer) {
    (*BufferSize)--;
    if (*BufferSize == 0) {
      return BUFFER_OVERFLOW;
    }
    *(BufferPtr++) = *(--TempStr);
  }

  *BufferPtr = 0;
  return Count;
}

/**
 *  Worker function for print string to buffer
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] *Format - format string
 *  @param[in] Marker  - Variable parameter
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[out] OutputStringLen  - output string length, include '\0' at the end
 *
 *  @retval    IDS_DEBUG_PRINT_SUCCESS      succeed
 *  @retval    IDS_DEBUG_PRINT_BUFFER_OVERFLOW      input buffer overflow
**/
IDS_DEBUG_PRINT_STATUS
AmdHdtOutDebugPrintWorker (
  IN  CONST   CHAR8   *Format,
  IN  VA_LIST         Marker,
  IN  CHAR8           *Buffer,
  IN  UINTN           BufferSize,
  OUT UINTN           *OutputStringLen
  )
{
  UINTN               Index;
  UINTN               Length;
  UINTN               Flags;
  UINTN               Width;
  UINT64              Value;
  CHAR8               *AsciiStr;

  //Init the default Value
  Index = 0;
  //
  // Process format string
  //
  for (; (*Format != '\0') && (BufferSize > 0); Format++) {
    if (*Format != '%') {
      Buffer[Index++] = *Format;
      BufferSize--;
    } else {
      Format = GetFlagsAndWidthEx (Format, &Flags, &Width, &Marker);
      switch (*Format) {
      case 'X':
        Flags |= PREFIX_ZERO;
        Width = sizeof (UINT64) * 2;
        //
        // break skipped on purpose
        //
      case 'x':
        if ((Flags & LONG_TYPE) == LONG_TYPE) {
          Value = VA_ARG (Marker, UINT64);
        } else {
          Value = VA_ARG (Marker, UINTN);
        }
        Length = ValueToHexStrEx (&Buffer[Index], Value, Flags, Width, &BufferSize);
        if (Length != BUFFER_OVERFLOW) {
          Index += Length;
        } else {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        break;

      case 'd':
        Value = (UINTN)VA_ARG (Marker, UINT32);
        Length = ValueToStringEx (&Buffer[Index], (UINT32)Value, Flags, &BufferSize);
        if (Length != BUFFER_OVERFLOW) {
          Index += Length;
        } else {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }

        break;

      case 's':
      case 'S':
        AsciiStr = (CHAR8 *)VA_ARG (Marker, CHAR8 *);
        while (*AsciiStr != '\0') {
          BufferSize--;
          if (BufferSize == 0) {
            return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
          }
          Buffer[Index++] = *AsciiStr++;
        }
        break;

      case 'c':
        BufferSize--;
        if (BufferSize == 0) {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        Buffer[Index++] = (CHAR8)VA_ARG (Marker, UINTN);
        break;

      case 'v':
        ASSERT (FALSE);   // %v is no longer supported
        break;

      case '%':
        BufferSize--;
        if (BufferSize == 0) {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        Buffer[Index++] = *Format;
        break;

      default:
        //
        // if the type is unknown print it to the screen
        //
        BufferSize--;
        if (BufferSize == 0) {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        Buffer[Index++] = '%';

        BufferSize--;
        if (BufferSize == 0) {
          return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
        }
        Buffer[Index++] = *Format;
        break;
      }
    }
  }
  if (BufferSize == 0) {
    return IDS_DEBUG_PRINT_BUFFER_OVERFLOW;
  }
  //Mark the end of word
  Buffer[Index] = 0;
  *OutputStringLen = Index;
  return IDS_DEBUG_PRINT_SUCCESS;
}

/**
 *  Process debug string
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] *Format - format string
 *  @param[in] Marker  - Variable parameter
 *
**/
VOID
AmdHdtOutDebugPrintProcess (
  IN  UINT64                Flag,
  IN  CONST CHAR8           *Format,
  IN  VA_LIST               Marker
  )
{
  UINT64                    Buffer[(MAX_LOCAL_BUFFER_SIZE / sizeof (UINT64))];
  CHAR8                     LocalBuffer[MAX_LOCAL_BUFFER_SIZE - 32];
  UINT64                    Filter;
  UINTN                     OutPutStringLen;
  UINTN                     TotalSize;
  CHAR8                     *Destination;
  CHAR8                     WarningString[] = "\n#BUFFER  OVERFLOW#\n";
  CHAR8                     *FormatString;
  EFI_STRING_TYPE           *StringTypePtr;
  EFI_STATUS_CODE_STRING    *StringPtr;
  UINT64                    MsrData;

  //
  // Read APIC_BASE register (0x1B), bit[8] returns 1 for BSP
  //
  MsrData = AsmReadMsr64 (0x1B);
  if ((MsrData & BIT8) == 0) {
    return;
  }

  Filter = IDS_DEBUG_PRINT_MASK;
  if ((Flag & Filter) == 0) {
    return;
  }

  if (AmdHdtOutDebugPrintWorker (Format, Marker, &LocalBuffer[0], sizeof (LocalBuffer), &OutPutStringLen) == IDS_DEBUG_PRINT_BUFFER_OVERFLOW) {
    Destination = LocalBuffer + sizeof (LocalBuffer) - sizeof (WarningString);
    CopyMem (Destination, WarningString, sizeof (WarningString));
    OutPutStringLen = sizeof (LocalBuffer);
  }
  
  //
  // Compute the total size of the record.
  // Note that the passing-in format string will be constructed to the following layout:
  //
  //         Buffer->|------------------------|
  //                 |     EFI_STRING_TYPE    | sizeof(EFI_STRING_TYPE)
  //      StringPtr->|------------------------|
  //                 | EFI_STATUS_CODE_STRING | sizeof(EFI_STATUS_CODE_STRING)
  //   FormatString->|------------------------|
  //                 |           ...          |
  //                 |      Format String     | sizeof (LocalBuffer)
  //                 |           ...          |
  //                 |------------------------|<- (UINT8 *)Buffer + sizeof(Buffer)
  //                
  TotalSize = sizeof (EFI_STRING_TYPE) + sizeof (EFI_STATUS_CODE_STRING) + AsciiStrSize (LocalBuffer);

  //
  // If the TotalSize is larger than the maximum record size, then return
  //
  if (TotalSize > sizeof (Buffer)) {
    return;
  }

  //
  // Fill in EFI_STATUS_CODE_STRING_DATA
  //
  StringTypePtr     = (EFI_STRING_TYPE *)Buffer;
  *StringTypePtr    = EfiStringAscii;
  
  StringPtr         = (EFI_STATUS_CODE_STRING *)(StringTypePtr + 1);
  FormatString      = (CHAR8 *)(StringPtr + 1);
  StringPtr->Ascii  = FormatString;

  //
  // Copy the Format string into the record
  //
  AsciiStrCpy (FormatString, LocalBuffer);

  //
  // Send the string data
  //
  REPORT_STATUS_CODE_EX (
    EFI_DEBUG_CODE,
    (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_DC_UNSPECIFIED),
    0,
    NULL,
    &gEfiStatusCodeDataTypeStringGuid,
    Buffer,
    TotalSize
    );
}

/**
 *  Prints string to debug host like printf in C
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] *Format - format string
 *  @param[in] ... Variable parameter
 *
**/
VOID
AmdHdtOutDebugPrint (
  IN UINT64         Flag,
  IN CONST  CHAR8   *Format,
  IN ...
  )
{
  VA_LIST Marker;
  VA_START (Marker, Format); //init marker to 1st dynamic parameters.
  AmdHdtOutDebugPrintProcess (Flag, Format, Marker);
  VA_END (Marker);
}