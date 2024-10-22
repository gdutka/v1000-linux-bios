/** @file
  CryptoService common function defination

;******************************************************************************
;* Copyright (c) 2012 - 2015, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __CRYPTO_PROTOCOL_H__
#define __CRYPTO_PROTOCOL_H__

#include <Protocol/Hash.h>
#include <Protocol/CryptoServices.h>
#include <Protocol/Pkcs7Verify.h>
#include <Library/BaseCryptLib.h>

#define CERT_BUFFER_SIZE               0x2000

#define CRYPTO_SERVICE_SIGNATURE       SIGNATURE_32 ('c', 'r', 'y', 'p')
#define SET_CRYPTO_FUNCTION(FunctionName)  \
   {   Crypto->FunctionName        = (!ByProtocol) ? FunctionName        : CryptoProtocol->FunctionName; \
       Crypto2->FunctionName       = (!ByProtocol) ? FunctionName        : Crypto2Protocol->FunctionName;}

#define CONVERT_CRYPTO_FUNCTION(FunctionName)  \
   {   gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->CryptoServiceProtocol.FunctionName)); \
       gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID **) &(ProtocolServices->CryptoService2Protocol.FunctionName));}

typedef struct {
  UINTN                                Signature;
  EFI_HANDLE                           Handle;
  CRYPTO_HASH_INSTANCE                 HashInstance;
  HASH2_INSTANCE_DATA                  Hash2Instance;
  CRYPTO_SERVICES_PROTOCOL             CryptoServiceProtocol;
  CRYPTO_SERVICES2_PROTOCOL            CryptoService2Protocol;
} CRYPTO_SERVICE;


VOID
SetupCryptoService (
  IN     EFI_HANDLE                    ImageHandle,
  IN OUT CRYPTO_SERVICE                *ProtocolServices,
  IN     BOOLEAN                       ByProtocol,
  IN     EFI_HASH_PROTOCOL             *HashProtocol,
  IN     EFI_HASH2_PROTOCOL            *Hash2Protocol,
  IN     CRYPTO_SERVICES_PROTOCOL      *CryptoService,
  IN     CRYPTO_SERVICES2_PROTOCOL     *CryptoProtocol2
  );
#endif
