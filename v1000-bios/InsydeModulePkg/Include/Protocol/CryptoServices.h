/** @file
  Crypto Service Protocol

;******************************************************************************
;* Copyright (c) 2012 - 2019, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++
  The crypto service protocol.
  Only limited crypto primitives (SHA-256 and RSA) are provided for runtime
  authenticated variable service.

Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

--*/

#ifndef __CRYPTO_SERVICES_PROTOCOL_H__
#define __CRYPTO_SERVICES_PROTOCOL_H__

#include <Library/BaseCryptLib.h>

//
// Crypto Service Protocol GUID.
//
#define CRYPTO_SERVICES_PROTOCOL_GUID \
  { \
    0xe1475e0c, 0x1746, 0x4802, { 0x86, 0x2e, 0x1, 0x1c, 0x2c, 0x2d, 0x9d, 0x86 } \
  }

//
// Crypto Service2 Protocol GUID.
//
#define CRYPTO_SERVICES2_PROTOCOL_GUID \
  { \
    0xc67fd6da, 0x1d11, 0x4dc6, { 0x96, 0x36, 0x2c, 0xbe, 0xe9, 0x85, 0x60, 0xae } \
  }


typedef struct _CRYPTO_SERVICES_PROTOCOL  CRYPTO_SERVICES_PROTOCOL;
typedef struct _CRYPTO_SERVICES2_PROTOCOL CRYPTO_SERVICES2_PROTOCOL;

typedef
UINTN
(EFIAPI *CRYPTO_SERVICES_GET_CONTEXT_SIZE) (
  VOID
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DUPLICATE) (
  IN   CONST VOID                           *Context,
  OUT  VOID                                 *NewContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_UPDATE) (
  IN OUT  VOID                              *Context,
  IN      CONST VOID                        *Data,
  IN      UINTN                             DataSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_FINAL) (
  IN OUT  VOID                              *Context,
  OUT     UINT8                             *Value
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_SHA1_INIT) (
  OUT  VOID  *Sha1Context
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_SM3_INIT) (
  OUT  VOID  *Sm3Context
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_INIT) (
  OUT  VOID                                 *Context,
  IN   CONST UINT8                          *Key,
  IN   UINTN                                KeyLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_ECB_ENCRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_ECB_DECRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_CBC_ENCRYPT) (
  IN   VOID                                 *Context,
  IN   CONST UINT8                          *Input,
  IN   UINTN                                InputSize,
  IN   CONST UINT8                          *Ivec,
  OUT  UINT8                                *Output
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_CBC_DECRYPT) (
  IN   VOID                                  *Context,
  IN   CONST UINT8                           *Input,
  IN   UINTN                                 InputSize,
  IN   CONST UINT8                           *Ivec,
  OUT  UINT8                                 *Output
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_GET_KEY) (
  IN OUT VOID                                  *RsaContext,
  IN     RSA_KEY_TAG                           KeyTag,
  OUT    UINT8                                 *BigNumber,
  IN OUT UINTN                                 *BnLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_PKCS1_SIGN) (
  IN      VOID                              *RsaContext,
  IN      CONST UINT8                       *MessageHash,
  IN      UINTN                             HashSize,
  OUT     UINT8                             *Signature,
  IN OUT  UINTN                             *SigSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_GET_PRIVATE_KEY_FROM_PEM) (
  IN   CONST UINT8                                       *PemData,
  IN   UINTN                                             PemSize,
  IN   CONST CHAR8                                       *Password,
  OUT  VOID                                              **RsaContext
  );

/**
  Translate certificate content from PEM to DER-encoded.

  @param[in]      PemData       Pointer to the PEM file of certificate.
  @param[in]      PemSize       Size of the OEM file of certificate in bytes.
  @param[out]     DerData       Pointer to the binary in DER-encoded.
  @param[out]     DerDataSize   Size of DER-encoded binary in bytes.

  @retval  TRUE   The certificate binary translate from PEM to DER successfully.
  @retval  FALSE  Invalid PEM content of certificate.

**/
typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_CA_CERT_FROM_PEM_TO_DER) (
  IN   CONST UINT8                     *PemData,
  IN   UINTN                           PemSize,
  OUT  UINT8                           **DerData,
  OUT  UINTN                           *DerDataSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_GET_PUBLIC_KEY_FROM_X509) (
  IN   CONST UINT8                                       *Cert,
  IN   UINTN                                             CertSize,
  OUT  VOID                                              **RsaContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_GET_SUBJECT_NAME) (
  IN      CONST UINT8                                    *Cert,
  IN      UINTN                                          CertSize,
  OUT     UINT8                                          *CertSubject,
  IN OUT  UINTN                                          *SubjectSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_VERIFY_CERT) (
  IN  CONST UINT8                                    *Cert,
  IN  UINTN                                          CertSize,
  IN  CONST UINT8                                    *CACert,
  IN  UINTN                                          CACertSize
  );

typedef
VOID *
(EFIAPI *CRYPTO_SERVICES_DH_NEW) (
  VOID
  );

typedef
VOID
(EFIAPI *CRYPTO_SERVICES_DH_FREE) (
  IN  VOID                                               *DhContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DH_GENERATE_PARAMETER) (
  IN OUT  VOID                                           *DhContext,
  IN      UINTN                                          Generator,
  IN      UINTN                                          PrimeLength,
  OUT     UINT8                                          *Prime
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DH_SET_PARAMETER) (
  IN OUT  VOID                                           *DhContext,
  IN      UINTN                                          Generator,
  IN      UINTN                                          PrimeLength,
  IN      CONST UINT8                                    *Prime
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DH_GENERATE_KEY) (
  IN OUT  VOID                                           *DhContext,
  OUT     UINT8                                          *PublicKey,
  IN OUT  UINTN                                          *PublicKeySize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_DH_COMPUTE_KEY) (
  IN OUT  VOID                                           *DhContext,
  IN      CONST UINT8                                    *PeerPublicKey,
  IN      UINTN                                          PeerPublicKeySize,
  OUT     UINT8                                          *Key,
  IN OUT  UINTN                                          *KeySize
  );

typedef
VOID *
(EFIAPI *CRYPTO_SERVICES_RSA_NEW) (
  VOID
  );

typedef
VOID
(EFIAPI *CRYPTO_SERVICES_RSA_FREE) (
  IN  VOID                                  *RsaContext
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_SET_KEY) (
  IN OUT VOID                                  *RsaContext,
  IN     RSA_KEY_TAG                           KeyTag,
  IN     CONST UINT8                           *BigNumber,
  IN     UINTN                                 BnLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RSA_PKCS1_VERIFY) (
  IN  VOID                                  *RsaContext,
  IN  CONST UINT8                           *MessageHash,
  IN  UINTN                                 HashLength,
  IN  UINT8                                 *Signature,
  IN  UINTN                                 SigLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_PKCS7_VERIFY) (
  IN  CONST UINT8                           *P7Data,
  IN  UINTN                                 P7Length,
  IN  CONST UINT8                           *TrustedCert,
  IN  UINTN                                 CertLength,
  IN  CONST UINT8                           *InData,
  IN  UINTN                                 DataLength,
  IN  BOOLEAN                               AuthentiCodeFlag
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_PKCS7_VERIFY_USING_PUBKEY) (
  IN	CONST UINT8                                        *P7Data,
  IN  UINTN 	                                           P7Length,
  IN	CONST UINT8                                        *TrustedPubKey,
  IN	UINTN		                                           TrustedPubKeyLen,
  IN  CONST UINT8                                        *InData,
  IN  UINTN 	                                           DataLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_AUTHENTICODE_VERIFY) (
  IN  CONST UINT8                                        *AuthData,
  IN  UINTN                                              DataLength,
  IN  CONST UINT8                                        *TrustedCert,
  IN  UINTN                                              CertLength,
  IN  CONST UINT8                                        *ImageHash,
  IN  UINTN                                              HashLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RAMDOM_SEED) (
  IN  CONST  UINT8                                       *Seed  OPTIONAL,
  IN  UINTN                                              SeedSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_RANDOM_BYTES) (
  OUT  UINT8                                             *Output,
  IN   UINTN                                             Size
  );

typedef
BOOLEAN
(EFIAPI *EFI_CRYPTO_SERVICE_PKCS7_GET_SIGNERS) (
  IN  CONST UINT8  *P7Data,
  IN  UINTN        P7Length,
  OUT UINT8        **CertStack,
  OUT UINTN        *StackLength,
  OUT UINT8        **TrustedCert,
  OUT UINTN        *CertLength
  );

typedef
VOID
(EFIAPI *EFI_CRYPTO_SERVICE_PKCS7_FREE_SIGNERS) (
  IN  UINT8        *Certs
  );

/**
  Decrypt the RSA-SSA signature with EMSA-PKCS1-v1_5 encoding scheme defined in
  RSA PKCS#1.

  @param[in]     RsaContext     Pointer to RSA context for signature verification.
  @param[in]     Signature      Pointer to RSA PKCS1-v1_5 signature to be verified.
  @param[in]     SignatureSize  Size of signature in bytes.
  @param[out]    Message        Pointer to buffer to receive RSA PKCS1-v1_5 decrypted message.
  @param[in,out] MessageSize    Size of the decrypted message in bytes.

  @retval  EFI_SUCCESS            Decrypt input signature successfully.
  @retval  EFI_BUFFER_TOO_SMALL   The Message buffer was too small. The current buffer size needed to hold the
                                  message is returned in MessageSize.
  @retval  EFI_OUT_OF_RESOURCES   There is not enough pool memory to store the results.
  @retval  EFI_INVALID_PARAMETER  1. RsaContext is NULL, Signature is NULL or SignatureSize is 0
                                  2. MessageSize is NULL or the Content of MessageSize isn't 0 and Message is NULL.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_CRYPTO_SERVICE_RSA_PKCS1_DECRYPT) (
  IN     VOID        *RsaContext,
  IN     CONST UINT8 *Signature,
  IN     UINTN       SignatureSize,
  OUT    UINT8       *Message,
  IN OUT UINTN       *MessageSize
  );

/**
  Retrieve the issuer bytes from one X.509 certificate.

  @param[in]      Cert         Pointer to the DER-encoded X509 certificate.
  @param[in]      CertSize     Size of the X509 certificate in bytes.
  @param[out]     CertIssuer   Pointer to the retrieved certificate issuer bytes.
  @param[in, out] IssuerSize   The size in bytes of the CertIssuer buffer on input,
                               and the size of buffer returned CertIssuer on output.

  If Cert is NULL, then return FALSE.
  If SubjectSize is NULL, then return FALSE.

  @retval  TRUE   The certificate issuer retrieved successfully.
  @retval  FALSE  Invalid certificate, or the IssuerSize is too small for the result.
                  The IssuerSize will be updated with the required size.

**/
typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_GET_ISSUER_NAME) (
  IN      CONST UINT8                                    *Cert,
  IN      UINTN                                          CertSize,
  OUT     UINT8                                          *CertIssuer,
  IN OUT  UINTN                                          *IssuerSize
  );

/**
  Get subject content and translate to string from one X.509 certificate.
  e.g.
    /C=SL/ST=Western/L=Colombo/OU=Insyde/CN=Insyde.com

  @param[in]      Cert           Pointer to the DER-encoded X509 certificate.
  @param[in]      CertSize       Size of the X509 certificate in bytes.
  @param[out]     SubjectString  Pointer to the string of subject content in UTF-8
  @param[out]     SubjectSize    The size in bytes of the SubjectString buffer,

  If Cert is NULL, then return FALSE.
  If SubjectSize is NULL, then return FALSE.

  @retval  TRUE   The certificate subject retrieved successfully.
  @retval  FALSE  Invalid certificate, or the SubjectSize is too small for the result.
                  The SubjectSize will be updated with the required size.

**/
typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_GET_SUBJECT_NAME_EX) (
  IN      CONST UINT8                  *Cert,
  IN      UINTN                        CertSize,
  OUT     UINT16                       **SubjectString,
  OUT     UINTN                        *SubjectSize
  );

/**
  Get subject content and translate to string from one X.509 certificate.
  e.g.
    /C=SL/ST=Western/L=Colombo/OU=Insyde/CN=Insyde.com

  @param[in]      Cert           Pointer to the DER-encoded X509 certificate.
  @param[in]      CertSize       Size of the X509 certificate in bytes.
  @param[out]     IssuerString   Pointer to the string of subject content in UTF-8
  @param[out]     IssuerSize    The size in bytes of the SubjectString buffer,

  If Cert is NULL, then return FALSE.
  If IssuerSize is NULL, then return FALSE.

  @retval  TRUE   The certificate subject retrieved successfully.
  @retval  FALSE  Invalid certificate, or the IssuerSize is too small for the result.
                  The IssuerSize will be updated with the required size.

**/
typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_GET_ISSUER_NAME_EX) (
  IN      CONST UINT8                  *Cert,
  IN      UINTN                        CertSize,
  OUT     UINT16                       **IssuerString,
  OUT     UINTN                        *IssuerSize
  );

/**
  Verifies the validility of a RFC3161 Timestamp CounterSignature embedded in PE/COFF Authenticode
  signature.

  If AuthData is NULL, then return FALSE.

  @param[in]  AuthData     Pointer to the Authenticode Signature retrieved from signed
                           PE/COFF image to be verified.
  @param[in]  DataSize     Size of the Authenticode Signature in bytes.
  @param[in]  TsaCert      Pointer to a trusted/root TSA certificate encoded in DER, which
                           is used for TSA certificate chain verification.
  @param[in]  CertSize     Size of the trusted certificate in bytes.
  @param[out] SigningTime  Return the time of timestamp generation time if the timestamp
                           signature is valid.

  @retval  TRUE   The specified Authenticode includes a valid RFC3161 Timestamp CounterSignature.
  @retval  FALSE  No valid RFC3161 Timestamp CounterSignature in the specified Authenticode data.

**/
typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_IMAGE_TIMESTAMP_VERIFY) (
  IN  CONST UINT8          *AuthData,
  IN  UINTN                DataSize,
  IN  CONST UINT8          *TsaCert,
  IN  UINTN                CertSize,
  OUT EFI_TIME             *SigningTime
  );

/**
  Retrieve the TBSCertificate from one given X.509 certificate.

  @param[in]      Cert         Pointer to the given DER-encoded X509 certificate.
  @param[in]      CertSize     Size of the X509 certificate in bytes.
  @param[out]     TBSCert      DER-Encoded To-Be-Signed certificate.
  @param[out]     TBSCertSize  Size of the TBS certificate in bytes.

  If Cert is NULL, then return FALSE.
  If TBSCert is NULL, then return FALSE.
  If TBSCertSize is NULL, then return FALSE.

  @retval  TRUE   The TBSCertificate was retrieved successfully.
  @retval  FALSE  Invalid X.509 certificate.

**/
typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_X509_GET_TBS_CERT) (
  IN  CONST UINT8       *Cert,
  IN  UINTN             CertSize,
  OUT UINT8             **TBSCert,
  OUT UINTN             *TBSCertSize
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICE_PKCS7_GET_CERTIFICATES_LIST) (
  IN  CONST UINT8  *P7Data,
  IN  UINTN        P7Length,
  OUT UINT8        **SignerChainCerts,
  OUT UINTN        *ChainLength,
  OUT UINT8        **UnchainCerts,
  OUT UINTN        *UnchainLength
  );

typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICES_PBKDF2_CREATE_KEY) (
  IN   CONST UINT8  *Salt,
  IN   UINTN        SaltLen,
  IN   UINTN        Algorithm,
  IN   UINTN        IterationCount,
  IN   CONST UINT8  *Password,
  IN   UINTN        PasswordLen,
  IN   UINTN        DerivedKeyLen,
  OUT  UINT8        *DerivedKey
  );

/**
  Carries out the RSA-SSA signature generation with EMSA-PKCS1-v1_5 encoding scheme.

  @param[in]       RsaContext    Pointer to RSA context for signature generation.
  @param[in]       Message       Pointer to octet message data to be signed.
  @param[in]       MessageSize   Size of the message data in bytes.
  @param[out]      Signature     Pointer to buffer to receive RSA PKCS1-v1_5 signature.
  @param[in, out]  SigSize       On input, the size of Signature buffer in bytes.
                                 On output,the size of data returned in Signature buffer in bytes.

  @retval  EFI_SUCCESS           Signature successfully generated in PKCS1-v1_5.
  @retval  EFI_BUFFER_TOO_SMALL  Signature generation failed.
  @retval  EFI_INVALID_PARAMETER 1. RsaContext is NULL, Signature is NULL, SigSize is NULL or Message is NULL.
                                 2. input signature size is not equal to key size.
                                 3. The message size is too large. The max size should be smaller than (keysize - 11)
                                    bytes. (ex: RSA2048 ---> Max message size is 245 == 2048/8 - 11)
**/
typedef
EFI_STATUS
(EFIAPI *CRYPTO_SERVICE_RSA_PKCS1_ENCRYPT) (
  IN      VOID         *RsaContext,
  IN      CONST UINT8  *Message,
  IN      UINTN        MessageSize,
  OUT     UINT8        *Signature,
  IN OUT  UINTN        *SigSize
  );


/**
  Carries out the RSA-SSA signature generation with RSAES-OAEP encoding scheme.
  (The supported hash algorithm is SHA1, SHA224, SHA256, SHA384,
  SHA512).

  @param[in]       RsaContext    Pointer to RSA context for signature generation.
  @param[in]       Algorithm     Specify hash algorithm.
  @param[in]       Message       Pointer to octet message data to be signed.
  @param[in]       MessageSize   Size of the message data in bytes.
  @param[out]      Signature     Pointer to buffer to receive RSA RSAES-OAEP signature.
  @param[in, out]  SigSize       On input, the size of Signature buffer in bytes.
                                 On output,the size of data returned in Signature buffer in bytes.

  @retval  EFI_SUCCESS           Signature successfully generated in RSAES-OAEP.
  @retval  EFI_BUFFER_TOO_SMALL  Signature generation failed.
  @retval  EFI_INVALID_PARAMETER 1. RsaContext is NULL, Signature is NULL, SigSize is NULL or Message is NULL.
                                    HashAlgorithm is unsupported.
                                 2. input signature size is not equal to key size.
                                 3. The message size is too large. The max size should be maller than
                                    (keysize - (2 * digest size)  - 2) bytes (ex: RSA2048 with SHA256 = 256 - (2 * 32) - 2 = 190 bytes)
**/
typedef
EFI_STATUS
(EFIAPI *CRYPTO_SERVICE_RSA_PKCS1_OAEP_ENCRYPT) (
  IN      VOID         *RsaContext,
  IN      EFI_GUID     *HashAlgorithm,
  IN      CONST UINT8  *Message,
  IN      UINTN        MessageSize,
  OUT     UINT8        *Signature,
  IN OUT  UINTN        *SigSize
  );

/**
  Decrypt the RSA-SSA signature with RSAES-OAEP encoding scheme defined in
  RSA PKCS#1. (The supported hash algorithm is SHA1, SHA224, SHA256, SHA384,
  SHA512).

  @param[in]      RsaContext       Pointer to RSA context for signature verification.
  @param[in]      Algorithm       Specify hash algorithm.
  @param[in]      Signature        Pointer to RSA RSAES-OAEP signature to be verified.
  @param[in]      SignatureSize    Size of signature in bytes.
  @param[out]     Message          Pointer to buffer to receive RSA RSAES-OAEP decrypted message.
  @param[in,out]  MessageSize      Size of the decrypted message in bytes.

  @retval  EFI_SUCCESS             Decrypt input signature successfully.
  @retval  EFI_BUFFER_TOO_SMALL    The Message buffer was too small.  The current buffer size needed
                                   to hold the message is returned in MessageSize.
                                   2. MessageSize is NULL or the Content of MessageSize isn't 0 and Message is NULL.
                                      HashAlgorithm is unsupported.
  @retval  EFI_OUT_OF_RESOURCES    There is not enough pool memory to store the results.
  @retval  EFI_INVALID_PARAMETER   1. RsaContext is NULL, Signature is NULL or SignatureSize is 0
                                   2. MessageSize is NULL or the Content of MessageSize isn't 0 and Message is NULL.
**/
typedef
EFI_STATUS
(EFIAPI *CRYPTO_SERVICE_RSA_PKCS1_OAEP_DECRYPT) (
  IN     VOID        *RsaContext,
  IN     EFI_GUID    *HashAlgorithm,
  IN     CONST UINT8 *Signature,
  IN     UINTN       SignatureSize,
  OUT    UINT8       *Message,
  IN OUT UINTN       *MessageSize
  );

/**
  Verifies the RSA-SSA signature with EMSA-PSS encoding scheme defined in
  RSA PKCS#1. (The supported hash algorithm is SHA1, SHA224, SHA256, SHA384,
  SHA512).

  If RsaContext    is NULL, then return FALSE.
  If HashAlgorithm is NULL, then return FALSE.
  If Message       is NULL, then return FALSE.
  If Signature     is NULL, then return FALSE.

  @param[in]  RsaContext     Pointer to RSA context for signature verification.
  @param[in]  HashAlgorithm  Specify hash algorithm.
  @param[in]  Message        Pointer to octet message to be checked.
  @param[in]  MessageSize    Size of the message in bytes.
  @param[in]  Signature      Pointer to RSASSA-PSS signature to be verified.
  @param[in]  SigSize        Size of signature in bytes.
  @param[in]  SaltLen        The length of salt value in bytes.

  @retval  TRUE   Valid signature encoded in  EMSA-PSS.
  @retval  FALSE  Invalid signature or invalid RSA context.

**/
typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICE_RSA_PKCS1_PSS_VERIFY) (
  IN  VOID         *RsaContext,
  IN  EFI_GUID     *HashAlgorithm,
  IN  CONST UINT8  *Message,
  IN  UINTN        MessageSize,
  IN  UINT8        *Signature,
  IN  UINTN        SigSize,
  IN  UINTN        SaltLen
  );

/**
  Derive key data using HMAC-SHAxxx based KDF.

  @param[in]   HashAlgorithm    Specify hash algorithm. The GUID is defined in UEFI spec.
  @param[in]   Key              Pointer to the user-supplied key.
  @param[in]   KeySize          Key size in bytes.
  @param[in]   Salt             Pointer to the salt(non-secret) value.
  @param[in]   SaltSize         Salt size in bytes.
  @param[in]   Info             Pointer to the application specific info.
  @param[in]   InfoSize         Info size in bytes.
  @param[Out]  Out              Pointer to buffer to receive hkdf value.
  @param[in]   OutSize          Size of hkdf bytes to generate.

  @retval TRUE   Hkdf generated successfully.
  @retval FALSE  Hkdf generation failed.

**/
typedef
BOOLEAN
(EFIAPI *CRYPTO_SERVICE_HKDF_EXTRACT_AND_EXPAND) (
  IN   EFI_GUID     *HashAlgorithm,
  IN   CONST UINT8  *Key,
  IN   UINTN        KeySize,
  IN   CONST UINT8  *Salt,    OPTIONAL
  IN   UINTN        SaltSize,
  IN   CONST UINT8  *Info,    OPTIONAL
  IN   UINTN        InfoSize,
  OUT  UINT8        *Out,
  IN   UINTN        OutSize
  );
//
// Crypto Service Protocol Structure.
// NOTE: this protocol deprecated. Suggest using CRYPTO_SERVICES2_PROTOCO not Below
// CRYPTO_SERVICES_PROTOCOL.
struct _CRYPTO_SERVICES_PROTOCOL {
  CRYPTO_SERVICES_GET_CONTEXT_SIZE              Sha1GetContextSize;
  CRYPTO_SERVICES_SHA1_INIT                     Sha1Init;
  CRYPTO_SERVICES_DUPLICATE                     Sha1Duplicate;
  CRYPTO_SERVICES_UPDATE                        Sha1Update;
  CRYPTO_SERVICES_FINAL                         Sha1Final;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              HmacSha1GetContextSize;
  CRYPTO_SERVICES_INIT                          HmacSha1Init;
  CRYPTO_SERVICES_DUPLICATE                     HmacSha1Duplicate;
  CRYPTO_SERVICES_UPDATE                        HmacSha1Update;
  CRYPTO_SERVICES_FINAL                         HmacSha1Final;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              TdesGetContextSize;
  CRYPTO_SERVICES_INIT                          TdesInit;
  CRYPTO_SERVICES_ECB_ENCRYPT                   TdesEcbEncrypt;
  CRYPTO_SERVICES_ECB_DECRYPT                   TdesEcbDecrypt;
  CRYPTO_SERVICES_CBC_ENCRYPT                   TdesCbcEncrypt;
  CRYPTO_SERVICES_CBC_DECRYPT                   TdesCbcDecrypt;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              AesGetContextSize;
  CRYPTO_SERVICES_INIT                          AesInit;
  CRYPTO_SERVICES_ECB_ENCRYPT                   AesEcbEncrypt;
  CRYPTO_SERVICES_ECB_DECRYPT                   AesEcbDecrypt;
  CRYPTO_SERVICES_CBC_ENCRYPT                   AesCbcEncrypt;
  CRYPTO_SERVICES_CBC_DECRYPT                   AesCbcDecrypt;

  CRYPTO_SERVICES_RSA_NEW                       RsaNew;
  CRYPTO_SERVICES_RSA_FREE                      RsaFree;
  CRYPTO_SERVICES_RSA_GET_KEY                   RsaGetKey;
  CRYPTO_SERVICES_RSA_SET_KEY                   RsaSetKey;
  CRYPTO_SERVICES_RSA_PKCS1_SIGN                RsaPkcs1Sign;
  CRYPTO_SERVICES_RSA_PKCS1_VERIFY              RsaPkcs1Verify;
  CRYPTO_SERVICES_RSA_GET_PRIVATE_KEY_FROM_PEM  RsaGetPrivateKeyFromPem;
  CRYPTO_SERVICES_RSA_GET_PUBLIC_KEY_FROM_X509  RsaGetPublicKeyFromX509;
  CRYPTO_SERVICES_X509_GET_SUBJECT_NAME         X509GetSubjectName;
  CRYPTO_SERVICES_X509_VERIFY_CERT              X509VerifyCert;

  CRYPTO_SERVICES_PKCS7_VERIFY                  Pkcs7Verify;
  CRYPTO_SERVICES_PKCS7_VERIFY_USING_PUBKEY     Pkcs7VerifyUsingPubKey;

  CRYPTO_SERVICES_AUTHENTICODE_VERIFY           AuthenticodeVerify;

  CRYPTO_SERVICES_DH_NEW                        DhNew;
  CRYPTO_SERVICES_DH_FREE                       DhFree;
  CRYPTO_SERVICES_DH_GENERATE_PARAMETER         DhGenerateParameter;
  CRYPTO_SERVICES_DH_SET_PARAMETER              DhSetParameter;
  CRYPTO_SERVICES_DH_GENERATE_KEY               DhGenerateKey;
  CRYPTO_SERVICES_DH_COMPUTE_KEY                DhComputeKey;

  CRYPTO_SERVICES_RAMDOM_SEED                   RandomSeed;
  CRYPTO_SERVICES_RANDOM_BYTES                  RandomBytes;

  //
  // For compatibility, add new supported funtion at the end of
  // EFI_CRYPTO_SERVICE_PROTOCOL.
  //
  EFI_CRYPTO_SERVICE_PKCS7_GET_SIGNERS          Pkcs7GetSigners;
  EFI_CRYPTO_SERVICE_PKCS7_FREE_SIGNERS         Pkcs7FreeSigners;
  EFI_CRYPTO_SERVICE_RSA_PKCS1_DECRYPT          RsaPkcs1Decrypt;
  CRYPTO_SERVICES_X509_GET_ISSUER_NAME          X509GetIssuerName;
  CRYPTO_SERVICES_IMAGE_TIMESTAMP_VERIFY        ImageTimestampVerify;
  CRYPTO_SERVICES_X509_GET_TBS_CERT             X509GetTBSCert;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              HmacSha256GetContextSize;
  CRYPTO_SERVICES_INIT                          HmacSha256Init;
  CRYPTO_SERVICES_DUPLICATE                     HmacSha256Duplicate;
  CRYPTO_SERVICES_UPDATE                        HmacSha256Update;
  CRYPTO_SERVICES_FINAL                         HmacSha256Final;
  CRYPTO_SERVICE_PKCS7_GET_CERTIFICATES_LIST    Pkcs7GetCertificatesList;
  CRYPTO_SERVICES_PBKDF2_CREATE_KEY             Pbkdf2CreateKey;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              Sm3GetContextSize;
  CRYPTO_SERVICES_SM3_INIT                      Sm3Init;
  CRYPTO_SERVICES_DUPLICATE                     Sm3Duplicate;
  CRYPTO_SERVICES_UPDATE                        Sm3Update;
  CRYPTO_SERVICES_FINAL                         Sm3Final;

  CRYPTO_SERVICE_RSA_PKCS1_ENCRYPT              RsaPkcs1Encrypt;
  CRYPTO_SERVICE_RSA_PKCS1_OAEP_ENCRYPT         RsaPkcs1OaepEncrypt;
  CRYPTO_SERVICE_RSA_PKCS1_OAEP_DECRYPT         RsaPkcs1OaepDecrypt;
  CRYPTO_SERVICE_RSA_PKCS1_PSS_VERIFY           RsaPkcs1PssVerify;
  CRYPTO_SERVICE_HKDF_EXTRACT_AND_EXPAND        HkdfExtractAndExpand;

  CRYPTO_SERVICES_X509_CA_CERT_FROM_PEM_TO_DER  X509GetCaCertificateFromPemToDer;

  CRYPTO_SERVICES_X509_GET_SUBJECT_NAME_EX      X509GetSubjectNameEx;
  CRYPTO_SERVICES_X509_GET_ISSUER_NAME_EX       X509GetIssuerNameEx;
};

//
// Crypto Service2 Protocol Structure.
//
struct _CRYPTO_SERVICES2_PROTOCOL {
  UINT32                                        Size;
  CRYPTO_SERVICES_GET_CONTEXT_SIZE              Sha1GetContextSize;
  CRYPTO_SERVICES_SHA1_INIT                     Sha1Init;
  CRYPTO_SERVICES_DUPLICATE                     Sha1Duplicate;
  CRYPTO_SERVICES_UPDATE                        Sha1Update;
  CRYPTO_SERVICES_FINAL                         Sha1Final;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              HmacSha1GetContextSize;
  CRYPTO_SERVICES_INIT                          HmacSha1Init;
  CRYPTO_SERVICES_DUPLICATE                     HmacSha1Duplicate;
  CRYPTO_SERVICES_UPDATE                        HmacSha1Update;
  CRYPTO_SERVICES_FINAL                         HmacSha1Final;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              TdesGetContextSize;
  CRYPTO_SERVICES_INIT                          TdesInit;
  CRYPTO_SERVICES_ECB_ENCRYPT                   TdesEcbEncrypt;
  CRYPTO_SERVICES_ECB_DECRYPT                   TdesEcbDecrypt;
  CRYPTO_SERVICES_CBC_ENCRYPT                   TdesCbcEncrypt;
  CRYPTO_SERVICES_CBC_DECRYPT                   TdesCbcDecrypt;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              AesGetContextSize;
  CRYPTO_SERVICES_INIT                          AesInit;
  CRYPTO_SERVICES_ECB_ENCRYPT                   AesEcbEncrypt;
  CRYPTO_SERVICES_ECB_DECRYPT                   AesEcbDecrypt;
  CRYPTO_SERVICES_CBC_ENCRYPT                   AesCbcEncrypt;
  CRYPTO_SERVICES_CBC_DECRYPT                   AesCbcDecrypt;

  CRYPTO_SERVICES_RSA_NEW                       RsaNew;
  CRYPTO_SERVICES_RSA_FREE                      RsaFree;
  CRYPTO_SERVICES_RSA_GET_KEY                   RsaGetKey;
  CRYPTO_SERVICES_RSA_SET_KEY                   RsaSetKey;
  CRYPTO_SERVICES_RSA_PKCS1_SIGN                RsaPkcs1Sign;
  CRYPTO_SERVICES_RSA_PKCS1_VERIFY              RsaPkcs1Verify;
  CRYPTO_SERVICES_RSA_GET_PRIVATE_KEY_FROM_PEM  RsaGetPrivateKeyFromPem;
  CRYPTO_SERVICES_RSA_GET_PUBLIC_KEY_FROM_X509  RsaGetPublicKeyFromX509;
  CRYPTO_SERVICES_X509_GET_SUBJECT_NAME         X509GetSubjectName;
  CRYPTO_SERVICES_X509_VERIFY_CERT              X509VerifyCert;

  CRYPTO_SERVICES_PKCS7_VERIFY                  Pkcs7Verify;
  CRYPTO_SERVICES_PKCS7_VERIFY_USING_PUBKEY     Pkcs7VerifyUsingPubKey;

  CRYPTO_SERVICES_AUTHENTICODE_VERIFY           AuthenticodeVerify;

  CRYPTO_SERVICES_DH_NEW                        DhNew;
  CRYPTO_SERVICES_DH_FREE                       DhFree;
  CRYPTO_SERVICES_DH_GENERATE_PARAMETER         DhGenerateParameter;
  CRYPTO_SERVICES_DH_SET_PARAMETER              DhSetParameter;
  CRYPTO_SERVICES_DH_GENERATE_KEY               DhGenerateKey;
  CRYPTO_SERVICES_DH_COMPUTE_KEY                DhComputeKey;

  CRYPTO_SERVICES_RAMDOM_SEED                   RandomSeed;
  CRYPTO_SERVICES_RANDOM_BYTES                  RandomBytes;

  //
  // For compatibility, add new supported funtion at the end of
  // EFI_CRYPTO_SERVICE_PROTOCOL.
  //
  EFI_CRYPTO_SERVICE_PKCS7_GET_SIGNERS          Pkcs7GetSigners;
  EFI_CRYPTO_SERVICE_PKCS7_FREE_SIGNERS         Pkcs7FreeSigners;
  EFI_CRYPTO_SERVICE_RSA_PKCS1_DECRYPT          RsaPkcs1Decrypt;
  CRYPTO_SERVICES_X509_GET_ISSUER_NAME          X509GetIssuerName;
  CRYPTO_SERVICES_IMAGE_TIMESTAMP_VERIFY        ImageTimestampVerify;
  CRYPTO_SERVICES_X509_GET_TBS_CERT             X509GetTBSCert;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              HmacSha256GetContextSize;
  CRYPTO_SERVICES_INIT                          HmacSha256Init;
  CRYPTO_SERVICES_DUPLICATE                     HmacSha256Duplicate;
  CRYPTO_SERVICES_UPDATE                        HmacSha256Update;
  CRYPTO_SERVICES_FINAL                         HmacSha256Final;
  CRYPTO_SERVICE_PKCS7_GET_CERTIFICATES_LIST    Pkcs7GetCertificatesList;
  CRYPTO_SERVICES_PBKDF2_CREATE_KEY             Pbkdf2CreateKey;

  CRYPTO_SERVICES_GET_CONTEXT_SIZE              Sm3GetContextSize;
  CRYPTO_SERVICES_SM3_INIT                      Sm3Init;
  CRYPTO_SERVICES_DUPLICATE                     Sm3Duplicate;
  CRYPTO_SERVICES_UPDATE                        Sm3Update;
  CRYPTO_SERVICES_FINAL                         Sm3Final;

  CRYPTO_SERVICE_RSA_PKCS1_ENCRYPT              RsaPkcs1Encrypt;
  CRYPTO_SERVICE_RSA_PKCS1_OAEP_ENCRYPT         RsaPkcs1OaepEncrypt;
  CRYPTO_SERVICE_RSA_PKCS1_OAEP_DECRYPT         RsaPkcs1OaepDecrypt;
  CRYPTO_SERVICE_RSA_PKCS1_PSS_VERIFY           RsaPkcs1PssVerify;
  CRYPTO_SERVICE_HKDF_EXTRACT_AND_EXPAND        HkdfExtractAndExpand;
  
  CRYPTO_SERVICES_X509_CA_CERT_FROM_PEM_TO_DER  X509GetCaCertificateFromPemToDer;

  CRYPTO_SERVICES_X509_GET_SUBJECT_NAME_EX      X509GetSubjectNameEx;
  CRYPTO_SERVICES_X509_GET_ISSUER_NAME_EX       X509GetIssuerNameEx;
};

extern EFI_GUID gCryptoServicesProtocolGuid;
extern EFI_GUID gCryptoService2sProtocolGuid;

#endif
