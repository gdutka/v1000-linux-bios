/* $NoKeywords:$ */
/**
 * @file
 *
 * Agesa structures and definitions
 *
 * Contains AMD AGESA core interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision: 317069 $   @e \$Date: 2015-04-20 11:25:50 -0700 (Mon, 20 Apr 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/


#ifndef _FCHXGBE_H_
#define _FCHXGBE_H_

#pragma pack (push, 1)

///
/// XGBE Port structure
///
typedef struct {
  BOOLEAN               XgbePortConfig;                 ///< XGbE controller Port Config Enable/disable
  UINT8                 XgbePortPlatformConfig;         ///< Platform Config
                                                        ///  @li   <b>0000</b> - Reserved
                                                        ///  @li   <b>0001</b> - 10G/1G Backplane
                                                        ///  @li   <b>0010</b> - 2.5G Backplane
                                                        ///  @li   <b>0011</b> - 1000Base-T
                                                        ///  @li   <b>0100</b> - 1000Base-X
                                                        ///  @li   <b>0101</b> - NBase-T
                                                        ///  @li   <b>0110</b> - 10GBase-T
                                                        ///  @li   <b>0111</b> - 10GBase-X
                                                        ///  @li   <b>1000</b> - SFP+
  UINT8                 XgbePortSupportedSpeed;         ///< Indicated Ethernet speeds supported on platform
                                                        ///  @li   <b>1xxx</b> - 10G
                                                        ///  @li   <b>x1xx</b> - 2.5G
                                                        ///  @li   <b>xx1x</b> - 1G
                                                        ///  @li   <b>xxx1</b> - 100M
  UINT8                 XgbePortConnectedType;          ///< PHY connected type
                                                        ///  @li   <b>000</b> - Port not used
                                                        ///  @li   <b>001</b> - SFP+
                                                        ///  @li   <b>010</b> - MDIO
                                                        ///  @li   <b>100</b> - Backplane connection
  UINT8                 XgbePortMdioId;                 ///< MDIO ID of the PHY associated with this port
  UINT8                 XgbePortMdioResetType;          ///< MDIO PHY reset type
                                                        ///  @li   <b>00</b> - None
                                                        ///  @li   <b>01</b> - I2C GPIO
                                                        ///  @li   <b>10</b> - Integrated GPIO
                                                        ///  @li   <b>11</b> - Reserved
  UINT8                 XgbePortResetGpioNum;           ///< GPIO used to control the reset
  UINT8                 XgbePortMdioResetI2cAddress;    ///< I2C address of PCA9535 MDIO reset GPIO
  UINT8                 XgbePortSfpI2cAddress;          ///< I2C address of PCA9535 for SFP
  UINT8                 XgbePortSfpTxFaultGpio;         ///< GPIO number for SFP+ TX_FAULT
  UINT8                 XgbePortSfpRsGpio;              ///< GPIO number for SFP+ RS
  UINT8                 XgbePortSfpModAbsGpio;          ///< GPIO number for SFP+ Mod_ABS
  UINT8                 XgbePortSfpRxLosGpio;           ///< GPIO number for SFP+ Rx_LOS
  UINT8                 XgbePortSfpGpioMask;            ///< GPIO Mask for SFP+
                                                        ///  @li   <b>1xxx</b> - Rx_LOS not supported
                                                        ///  @li   <b>x1xx</b> - Mod_ABS not supported
                                                        ///  @li   <b>xx1x</b> - RS not supported
                                                        ///  @li   <b>xxx1</b> - TX_FAULT not supported
  UINT8                 XgbePortSfpTwiAddress;          ///< Address of PCA9545 I2C multiplexor
  UINT8                 XgbePortSfpTwiBus;              ///< Downstream channel of PCA9545
  BOOLEAN               XgbaPortRedriverPresent;        ///< Redriver Present or not
  UINT8                 Reserve0[3];                    ///< Reserved
  UINT8                 XgbaPortRedriverModel;          ///< Redriver Model
                                                        ///  @li   <b>00</b> - InPhi 4223
                                                        ///  @li   <b>01</b> - InPhi 4227
  UINT8                 XgbaPortRedriverInterface;      ///< Redriver Interface
                                                        ///  @li   <b>00</b> - MDIO
                                                        ///  @li   <b>01</b> - I2C
  UINT8                 XgbaPortRedriverAddress;        ///< Redriver Address
  UINT8                 XgbaPortRedriverLane;           ///< Redriver Lane
  UINT8                 XgbaPortPadGpio;                ///< Portx_GPIO Pad selection
                                                        ///  @li   <b>001</b> - MDIO0 pin
                                                        ///  @li   <b>010</b> - MDIO1 pin
                                                        ///  @li   <b>100</b> - SFP pin
  UINT8                 XgbaPortPadMdio;                ///< Portx_Mdio Pad selection
                                                        ///  @li   <b>001</b> - MDIO0 pin
                                                        ///  @li   <b>010</b> - MDIO1 pin
                                                        ///  @li   <b>100</b> - SFP pin
  UINT8                 XgbaPortPadI2C;                 ///< Portx_I2C Pad selection
                                                        ///  @li   <b>001</b> - MDIO0 pin
                                                        ///  @li   <b>010</b> - MDIO1 pin
                                                        ///  @li   <b>100</b> - SFP pin
  UINT8                 Reserve1;                       ///< Reserved
} FCH_XGBE_PORT;

///
/// XGBE MAC Address
///
typedef struct {
  UINT32                XgbePortMacAddressLow;          ///< XGbE controller Port MAC address
  UINT32                XgbePortMacAddressHigh;         ///< XGbE controller Port MAC address
} FCH_XGBE_MAC;

///
/// XGBE Configure structure
///
typedef struct {
  FCH_XGBE_PORT         Port[4];                        ///
  FCH_XGBE_MAC          Mac[4];                         ///
  BOOLEAN               XgbeMdio0Enable;                ///< XGbe controller Sideband MDIO0 enable/disable
  BOOLEAN               XgbeMdio1Enable;                ///< XGbe controller Sideband MDIO1 enable/disable
  BOOLEAN               XgbeSfpEnable;                  ///< XGbe controller Sideband SFP enable/disable
  UINT32                MaxPortNum;                     ///
} FCH_XGBE;

#pragma pack (pop)

#endif // _FCHXGBE_H_

