/* $NoKeywords:$ */
/**
 * @file
 *
 * SnpDxe.h
 *
 * Ethernet Driver DXE-Phase header file.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: FDK
 * @e sub-project: UEFI
 * @e version: $Revision: 298339 $ @e date: $Date: 2014-07-14 16:27:48 -0700 (Mon, 14 Jul 2014) $
 *
 */
/*
 ****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef __SNP_DXE_H__
#define __SNP_DXE_H__

  #include <Uefi.h>
  #include <Library/MemoryAllocationLib.h>
  #include <Library/BaseMemoryLib.h>
  #include <Library/DebugLib.h>
  #include <Library/BaseLib.h>
  #include <Library/UefiBootServicesTableLib.h>
  #include <Library/UefiRuntimeServicesTableLib.h>
  #include <Library/NetLib.h>
  #include <Library/TimerLib.h>
  #include <Library/PcdLib.h>
  #include <Library/DevicePathLib.h>
  #include <Protocol/SimpleNetwork.h>
  #include <Protocol/DevicePath.h>
  #include <Guid/GlobalVariable.h>
  #include <Guid/EventGroup.h>
  #include <Protocol/FabricTopologyServices2.h>
  #include <Library/AmdBaseLib.h>
  #include "SnpDxeConfig.h"
  #include "SnpDxeXgbe.h"

#define ENET_ADDR_LEN                    6
#define ENET_MAX_PACKET_SIZE             0x600
#define ETH_DATA_LEN                     0x5DC

#define ENET_NVRAM_SIZE                  0
#define ENET_NVRAM_ACCESS_SIZE           0
#define ENET_IFTYPE                      0x01
#define ENET_DRIVER_SIGNATURE            SIGNATURE_32 ('s', 's', 'p', '0')

  #pragma pack (push, 1)
  /// Ethernet Header
  typedef struct {
    UINT8   DstAddr[ENET_ADDR_LEN]; ///< Destination Address
    UINT8   SrcAddr[ENET_ADDR_LEN]; ///< Source Address
    UINT16  Type;                   ///< Type
  } ENET_HEADER;
  #pragma pack (pop)

  /// Ethernet Device Path
  typedef struct {
    MAC_ADDR_DEVICE_PATH EthDevicePath; ///< Ethernet Device Path
    EFI_DEVICE_PATH    EndDevicePath;   ///< EFI Device Path
  } ENET_DEVICE_PATH;

  /// Ethernet DMA Buffer
  typedef struct {
    PHYSICAL_ADDRESS DeviceAddress; ///< Device Address
    UINTN BufferSize;               ///< Buffer size
    VOID *HostAddress;              ///< Host address
    VOID *Mapping;                  ///< Mapping
  } ENET_DMA_BUFFER;

  typedef struct {
    UINT16                        XgbeBusFunc;
  } XGBE_PLATFORM_DATA;
  //
  //  Private data for driver.
  //
  /// Ethernet DXE Private Data
  struct _ENET_DXE_PRIVATE_DATA_
  {
    UINT32                        Signature;                      ///< Signature
    EFI_HANDLE                    DeviceHandle;                   ///< Device Handle
    EFI_SIMPLE_NETWORK_PROTOCOL   Snp;                            ///< Simple Network Protocol
    EFI_SIMPLE_NETWORK_MODE       Mode;                           ///< Simple Network Mode
    ENET_DEVICE_PATH              DevicePath;                     ///< Ethernet Device Path
    EFI_DEVICE_PATH_PROTOCOL      *FullDevicePath;                ///< Complete Device Path
    VOID                          *Db;                            ///< Device Block

    UINT32                        xpcs_regs;                       ///< XPCS Registers
    UINT32                        i2c_regs;                        ///< IC_CON Registers
    UINT8                         XgmacBus;
    xgmac_prv_data                xgmac_pdata;                     ///< XGMac Private Data
    UINT32                        desc_tx_index;                   ///< Descritor Transmit Index
    UINT32                        desc_rx_index;                   ///< Descritor Receive Index
    TX_NORMAL_DESC                *desc_tx;                        ///< Descritor Transmit
    ENET_DMA_BUFFER               desc_tx_dma;                     ///< Descritor Transmit DMA
    RX_NORMAL_DESC                *desc_rx;                        ///< Descritor Receive
    ENET_DMA_BUFFER               desc_rx_dma;                     ///< Descritor Receive DMA
    BOOLEAN                       desc_rx_interrupt[RX_DESC_CNT];  ///< Descritor Receive Interrupt
    ENET_DMA_BUFFER               pkt_tx_dma[TX_DESC_CNT];         ///< Ethernet DMA Transmit Buffer
    ENET_DMA_BUFFER               pkt_rx_dma[RX_DESC_CNT];         ///< Ehternet DMA Receive Buffer
    BOOLEAN                       field_card;                      ///< Is field card present
    UINT8                         PCA9545_ADDRESS_LOWER;           ///< Lower two bits of PCA9545
    VOID                          *phy_data;
    struct                        xgbe_phy phy;
    UINTN                         kr_redrv;
    UINT32                        xprop_regs;                      ///< XGBE property registers
    UINT32                        xi2c_regs;
    UINTN                         fec_ability;
    enum                          xgbe_an_mode  an_mode;
    enum                          xgbe_an an_result;
    enum                          xgbe_an an_state;
    enum                          xgbe_rx kr_state;
    enum                          xgbe_rx kx_state;

    BOOLEAN                       debugfs_an_cdr_workaround;
    BOOLEAN                       debugfs_an_cdr_track_early;
    UINT32                        an_supported;
    UINT32                        parallel_detect;
    UINT32                        irq_reissue_support;
    UINT32                        mode_switch;

    UINT32                        kr_done;
    UINT32                        cdr_delay_required;

  };
  typedef struct _ENET_DXE_PRIVATE_DATA_ ENET_DXE_PRIVATE_DATA;
 // typedef struct ENET_DXE_PRIVATE_DATA xgbe_prv_data;

#define ENET_DXE_PRIVATE_DATA_FROM_SNP_THIS(a) \
        CR (a, ENET_DXE_PRIVATE_DATA, Snp, ENET_DRIVER_SIGNATURE)

  VOID
  EnetDmaFreeBuffer (
    OUT ENET_DMA_BUFFER *DMA
  );

  EFI_STATUS
  EnetDmaAllocBuffer (
    IN      UINTN BufferSize,
       OUT  ENET_DMA_BUFFER *DMA
  );

//
// Implementation
//
  EFI_STATUS
  ImplementStart (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementStop (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementInit (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementReset (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementShutdown (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementRecvFilterEnable (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementRecvFilterDisable (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementRecvFilterRead (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementSetStnAddr (
    IN  ENET_DXE_PRIVATE_DATA *Private,
    IN  EFI_MAC_ADDRESS *NewMacAddr
  );

  EFI_STATUS
  ImplementStatistics (
    IN  ENET_DXE_PRIVATE_DATA *Private
  );

  EFI_STATUS
  ImplementIp2Mac (
    IN  ENET_DXE_PRIVATE_DATA *Private,
    IN  BOOLEAN             IPv6,
    IN  EFI_IP_ADDRESS      *IP,
    IN  EFI_MAC_ADDRESS     *MAC
  );

  EFI_STATUS
  ImplementNvDataRead (
    IN  ENET_DXE_PRIVATE_DATA *Private,
    IN  UINTN      Offset,
    IN  UINTN      BufferSize,
    IN  VOID       *Buffer
  );

  EFI_STATUS
  ImplementGetStatus (
    IN  ENET_DXE_PRIVATE_DATA *Private,
    IN  UINT32     *InterruptStatusPtr,
    IN  VOID       **TransmitBufferListPtr
  );

  EFI_STATUS
  ImplementFillHeader (
    IN  ENET_DXE_PRIVATE_DATA *Private,
    IN  VOID            *MacHeaderPtr,
    IN  UINTN           HeaderSize,
    IN  VOID            *Buffer,
    IN  UINTN           BufferSize,
    IN  EFI_MAC_ADDRESS *DestAddr,
    IN  EFI_MAC_ADDRESS *SrcAddr,
    IN  UINT16          *ProtocolPtr
  );

  EFI_STATUS
  ImplementTransmit (
    IN  ENET_DXE_PRIVATE_DATA *Private,
    IN  VOID       *Buffer,
    IN  UINTN      BufferSize
  );

  EFI_STATUS
  ImplementReceive (
    IN  ENET_DXE_PRIVATE_DATA *Private,
    IN  VOID            *Buffer,
    IN  UINTN           *BufferSize,
    IN  UINTN           *HeaderSize,
    IN  EFI_MAC_ADDRESS *SrcAddr,
    IN  EFI_MAC_ADDRESS *DestAddr,
    IN  UINT16          *Protocol
  );

#endif//__SNP_DXE_H__

