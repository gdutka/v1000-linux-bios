/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <PiPei.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Ppi/I2cMaster.h>
#include "AmdXgbeWorkaround.h"
#include <AMD.h>
#include <Library/AmdBaseLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Ppi/FabricTopologyServices2Ppi.h>
#include <Ppi/PostBootScriptTable.h>
#include <Library/IoLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Dxe/AmdPbsSetupDxe/AmdPbsConfig.h>
#include <Library/AmdPbsConfigLib/AmdPbsConfigLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <SSFch.h>
#include "xGbEI2cLib.h"
#include <Ppi/Stall.h>

VOID          FchSmnRW8 (IN UINT32 IohcBus, IN UINT32 SmnAddress, IN UINT8 AndMask, IN UINT8 OrMask, IN AMD_CONFIG_PARAMS *StdHeader);

#define REV_A_BOARD 1

//I2C frequency
#define SS_SPEED           (100*1000)     //100kbps
#define PORT_I2C_MUTEX        0x1D080ul

EFI_PEI_STALL_PPI                 *gStallPpi=NULL;

EFI_STATUS
EFIAPI
S3ResumeXGBEWorkaround (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );


EFI_PEI_NOTIFY_DESCRIPTOR  mNotifyOnPostScriptList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiPostScriptTablePpiGuid,
  S3ResumeXGBEWorkaround
};



/**
 *---------------------------------------------------------------------------------------
 *
 *  getBitfield
 *
 *  Description:
 *    Read value of bits.
 *
 *  Parameters:
 *    @param[in]      Value       In coming value
 *
 *    @param[in]      *Lsb        Least significant bit to start reading
 *
 *    @param[in]      *Width      How many bits to read
 *
 *    @return         Value       Value after reading bits
 *
 *------------------------------------------------------------------------------------
 **/
STATIC
UINT32
getBitfield (
  IN UINT32 Value,
  IN UINT32 Lsb,
  IN UINT32 Width
  )
{
  UINT32 Mask;
  UINT32 i;

  Mask = 0;
  for (i = 0; i < Width; i++) {
    Mask |= 1 << i;
  }

  Value &= Mask << Lsb;

  return (Value >> Lsb);
}
/**
 *---------------------------------------------------------------------------------------
 *
 *  getBitfieldReg32
 *
 *  Description:
 *    Read value of bits.
 *
 *  Parameters:
 *    @param[in]      Addr       In coming Address
 *
 *    @param[in]      *Lsb        Least significant bit to start reading
 *
 *    @param[in]      *Width      How many bits to read
 *
 *    @return         Value       Value after reading bits
 *
 *------------------------------------------------------------------------------------
 **/
STATIC
UINT32
getBitfieldReg32 (
  IN UINT32 Addr,
  IN UINT32 Lsb,
  IN UINT32 Width
  )
{
  UINT32 Value;

  Value = MmioRead32 (Addr);
  return getBitfield (Value, Lsb, Width);
}

/*********************************************************************************
 * Name: XgbeI2cInit
 *
 * Description
 *   
 *
 * Input
 *   XgbeData : XGBE data to store base address for each XGBE port
 *
 * Output
 *
 *********************************************************************************/
VOID
XgbeI2cInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN  XGBE_DATA *XgbeData,
  AMD_XGBE_I2C_MASTER_PRIVATE *I2cPrivate
  )
{
  //
  // I2C registers for ethernet is mapped to BAR0 @offset 1E000
  //
  
  I2cPrivate->I2cMasterPpi[0].I2cSdpAddress = XgbeData->Xgmac0BaseAdd + 0x1E000; 
  I2cPrivate->I2cMasterPpi[1].I2cSdpAddress = XgbeData->Xgmac1BaseAdd + 0x1E000; 

  I2cInit (&I2cPrivate->I2cMasterPpi[0]);
  I2cInit (&I2cPrivate->I2cMasterPpi[1]);

}
VOID
GetI2cOwnership (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN       UINT32 XgmacBase,
  IN       UINT8 MutexId
  )
{
  UINT32 MutexVal = 0;
  UINT32 MutexSet1 = 0;
  MutexVal |= MutexId;
  MutexVal <<= 29;
  MutexVal |= 1;

  if (XgmacBase == 0xFFFFFFFF || XgmacBase == 0) {
    DEBUG((DEBUG_INFO, "In GetI2cOwnership, returning with XgmacBase=%x line(%d)\n", XgmacBase, __LINE__));
    return;
  }

  MmioWrite32 (XgmacBase + PORT_I2C_MUTEX, 0x80000000);
  gStallPpi->Stall (PeiServices, gStallPpi, 1000);

  while(MmioRead32 (XgmacBase + PORT_I2C_MUTEX)) {
    DEBUG((DEBUG_INFO, "In GetI2cOwnership, waiting for mutex. line(%d)\n", __LINE__));
  }

  do {
    MmioWrite32 (XgmacBase + PORT_I2C_MUTEX, MutexVal);
    gStallPpi->Stall (PeiServices, gStallPpi, 1000);
    MutexSet1 = MmioRead32 (XgmacBase + PORT_I2C_MUTEX);
  } while (MutexSet1 != MutexVal);
}


VOID
PutI2cOwnership (
  IN       UINT32 XgmacBase,
  IN       UINT8 MutexId
  )
{
  if (XgmacBase == 0xFFFFFFFF || XgmacBase == 0) {
    DEBUG((DEBUG_INFO, "In PutI2cOwnership, returning with XgmacBase=%x line(%d)\n", XgmacBase, __LINE__));
    return;
  }

  if (getBitfieldReg32 (XgmacBase + PORT_I2C_MUTEX, 29, 2) == MutexId) {
    MmioWrite32 (XgmacBase + PORT_I2C_MUTEX, 0x80000000);
  }
}
/*********************************************************************************
 * Name: XgbeGetBaseAddr
 *
 * Description
 *   Get Base address of XGBE
 *
 * Input
 *   XgbeData : XGBE data to store base address for each XGBE port
 *
 * Output
 *
 *********************************************************************************/
EFI_STATUS
XgbeGetBaseAddr (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  OUT  XGBE_DATA *XgbeData
  )
{
  UINT32 SecBusDie0,i, port;
  UINT8 BusMasterEn;
  UINT32 Value,XgmacBase, Xgmac0_Base, Xgmac1_Base, Xpcs0_Base, Xpcs1_Base;
  UINTN BusNumberBase, SocketNum, DieNum; 
  PCI_ADDR PciAddress;
  EFI_STATUS Status = 0;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES2_PPI *FabricTopologyServices;
//[-start-220124-IB06070195-add]//
  UINT16 Vid, Did;
//[-end-220124-IB06070195-add]//

  XgmacBase = 0;
  Xgmac0_Base = 0;
  Xgmac1_Base = 0;
  Xpcs0_Base = 0;
  Xpcs1_Base = 0;
  BusNumberBase = 0;
  SocketNum = 0;
  DieNum = 0;
  BusMasterEn = 0;
  SecBusDie0 = 0;

  DEBUG((DEBUG_INFO, "In XgbeGetBaseAddr\n"));

  Status = (*PeiServices)->LocatePpi (
                                     PeiServices,
                                     &gAmdFabricTopologyServices2PpiGuid,
                                     0,
                                     NULL,
                                     (VOID**)&FabricTopologyServices
                                     );

  if (FabricTopologyServices->GetRootBridgeInfo (SocketNum, DieNum, 0, NULL, &BusNumberBase, NULL, NULL, NULL, NULL) != EFI_SUCCESS) {
    DEBUG ((DEBUG_INFO, "GetRootBridgeInfo returned Status = %r\n", Status));
    Status = EFI_DEVICE_ERROR;
  }

  DEBUG((DEBUG_INFO, "BusNumberBase = %x\n", BusNumberBase));

//[-start-220124-IB06070195-add]//
  //
  // Ethernet functions are under the Bridge@ Device 8, function2
  //
  PciAddress.AddressValue = (MAKE_SBDFO (0, BusNumberBase, 0x8, FUNC_2, 0x00));
  Vid = (UINT16)(PciRead32 (PciAddress.AddressValue) & 0xFFFF);
  Did = (UINT16)((PciRead32 (PciAddress.AddressValue) >> sizeof(UINT16)) & 0xFFFF);
  DEBUG((DEBUG_INFO, "Bridge@ Device 8, function2 Vid = %x, Did = %x\n", Vid, Did));
  if (PciRead32 (PciAddress.AddressValue) == 0xFFFFFFFF) {
    return EFI_DEVICE_ERROR;
  }
//[-end-220124-IB06070195-add]//

  //
  // Ethernet functions are under the Bridge@ Device 8, function2
  //
  PciAddress.AddressValue = (MAKE_SBDFO (0, BusNumberBase, 0x8, FUNC_2, 0x19));
  SecBusDie0 = PciRead8 (PciAddress.AddressValue);

  DEBUG((DEBUG_INFO, "BusNumberBase = %x, SecBusDie0 = %x\n", BusNumberBase, SecBusDie0));

  //
  // For Raven,
  // xGbE Controller Port0 => Bus @, Device 0, Function 1
  // xGbE Controller Port1 => Bus @, Device 0, Function 2
  //
  // Port0, Enable Bus master enable bit
  //
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_1, 0x4));
  BusMasterEn = PciRead8 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite8 (PciAddress.AddressValue, BusMasterEn);

  //
  // Get the MAC and the PCS base addresses from the config space
  //
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_1, 0x10));
  Xgmac0_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_1, 0x14));
  Xpcs0_Base = PciRead32 (PciAddress.AddressValue);

  DEBUG((DEBUG_INFO, "Xgmac0_Base = %x, Xpcs0_Base = %x\n", Xgmac0_Base, Xpcs0_Base));

  //
  // Port1, Enable the Bus master enable bit
  //
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_2, 0x4));
  BusMasterEn = PciRead8 (PciAddress.AddressValue);
  BusMasterEn |= 0x6;
  PciWrite8 (PciAddress.AddressValue, BusMasterEn);

  //
  // Get the MAC and PCS base addresses from the config space
  //
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_2, 0x10));
  Xgmac1_Base = PciRead32 (PciAddress.AddressValue);
  PciAddress.AddressValue = (MAKE_SBDFO (0, SecBusDie0, 0x0, FUNC_2, 0x14));
  Xpcs1_Base = PciRead32 (PciAddress.AddressValue);

  DEBUG((DEBUG_INFO, "Xgmac1_Base = %x, Xpcs1_Base = %x\n", Xgmac1_Base, Xpcs1_Base));

  XgbeData->Xgmac0BaseAdd = 0;
  XgbeData->Xgmac1BaseAdd = 0;

  //Program for XGBE ECC Interrupt.
  for (port = 0; port < NUMBER_OF_ETH_PORTS; port++){
    if (port == 0) { 
      if(Xgmac0_Base == 0xFFFFFFFF)
        continue;
      XgmacBase = Xgmac0_Base;
      XgbeData->Xgmac0BaseAdd = Xgmac0_Base;
    }
    if (port == 1) { 
      if(Xgmac1_Base == 0xFFFFFFFF)
        break;
      XgmacBase = Xgmac1_Base;
      XgbeData->Xgmac1BaseAdd = Xgmac1_Base;
    }

    if (getBitfieldReg32 (XgmacBase + 0x1d030, 0, 6) == 0 ) {
      DEBUG ((EFI_D_ERROR, " ECC already addressed for Port %x \n", port));
      continue;
    }


    MmioWrite32 (XgmacBase + 0x1008, 0x103);
    while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
    MmioWrite32 (XgmacBase + 0x1010, 0x0);
    //write rx_fifo
    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x380f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
    }

    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x80f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
    }
    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x180f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
    }

    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x340f);
      do {
        Value = MmioRead32 (XgmacBase + 0x100C);
        Value = getBitfieldReg32 (XgmacBase + 0x100C, 0, 1);
      } while (Value != 0);
      Value = MmioRead32 (XgmacBase + 0x1010);
    }

    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x40f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
      Value = MmioRead32 (XgmacBase + 0x1010);
    }
    for (i = 0; i <= 3; i++) {
      MmioWrite32 (XgmacBase + 0x1008, 0x140f);
      while (getBitfieldReg32 (XgmacBase + 0x100C, 0, 1) != 0);
      Value = MmioRead32 (XgmacBase + 0x1010);
    }
    MmioWrite32 (XgmacBase + 0x1d030, 0x3f);

  }

  return (Status);
}


/*********************************************************************************
 * Name: XgbeReadEeprom
 *
 * Description
 *   Call to Read XGBE I2Cdevices
 *
 * Input
 *   SlaveAdd:      Slave Address of I2C
 *   Offset:        Offset to write to
 *   DataLength:    Size of Data to be Read from device.
 *
 * Output
 *   Data read from device
 *
 *********************************************************************************/
UINT16
XgbeReadEeprom(
  XGBE_I2C_MASTER_PRIVATE *XgbEI2cMaster,
  UINT8 SlaveAdd,
  UINT8 Offset,
  UINT16 DataLength
  )
{
  UINT8                     PacketBuffer[sizeof(EFI_I2C_REQUEST_PACKET) + sizeof(EFI_I2C_OPERATION)];
  EFI_I2C_REQUEST_PACKET    *RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  EFI_STATUS                Status;
  UINT8                     Data[2];
  UINT16                    Pca9555Data;

  Status =                  EFI_SUCCESS;
  Pca9555Data =             0;
  Data[0] =                 0;
  Data[1] =                 0;

 
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC Workaround: Could not Locate I2C Protocol.\n"));
  }
  
  RequestPacket = (EFI_I2C_REQUEST_PACKET*)PacketBuffer;
  Operation   = RequestPacket->Operation;
  RequestPacket->OperationCount = 2;
  Operation[0].Flags            = 0;
  Operation[0].LengthInBytes    = sizeof(UINT8);
  Operation[0].Buffer           = &Offset;
  Operation[1].Flags            = I2C_FLAG_READ;
  Operation[1].LengthInBytes    = DataLength;
  Operation[1].Buffer           = Data;

  Status = XgbeStartRequest(XgbEI2cMaster, SlaveAdd, RequestPacket);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed to XgbeStartRequest.\n"));
  }
  Pca9555Data = (Data[1] << 8) | Data[0];
  return Pca9555Data;
}

/*********************************************************************************
 * Name: XgbeWriteEeprom
 *
 * Description
 *   Call to write to XGBE I2C devices 

 *
 * Input
 *   SlaveAdd:     Slave Address of I2C
 *   Offset:       Offset to write to
 *   Data:         Data to be written on device.
 *
 * Output
 *
 *********************************************************************************/

EFI_STATUS
XgbeWriteEeprom(
  XGBE_I2C_MASTER_PRIVATE *XgbEI2cMaster,
  UINT8 SlaveAdd,
  UINT8 Offset,
  UINT16 Data
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_I2C_REQUEST_PACKET    RequestPacket;
  EFI_I2C_OPERATION         *Operation;
  UINT8                     Buffer[3];

  //Prepare write data buffer
  Buffer[0] = Offset;
  Buffer[1] = (UINT8)(Data & 0x00FF); //Low byte
  Buffer[2] = (UINT8)(Data >> 8);     //High byte

  //Write Byte Operation
  Operation   = RequestPacket.Operation;
  RequestPacket.OperationCount = 1;
  Operation[0].Flags           = 0;
  Operation[0].LengthInBytes   = 3;
  Operation[0].Buffer          = &Buffer[0];

  Status = XgbeStartRequest(XgbEI2cMaster, SlaveAdd, &RequestPacket);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "\nXGMAC: Failed to XgbeStartRequest.\n"));
  }
  return Status;
}

/*********************************************************************************
 * Name: XgbeTxDisableLow
 *
 * Description
 *   Programming IO expander PCA9535PWR which is connected to STACKED_SFP+_CONNECTOR.
 *   Setting TX_DISABLE pin to low.
 *
 * Input
 *   XgbeData : XGBE DATA holds the base address of each XGBE port
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/

EFI_STATUS
XgbeTxDisableLow (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  XGBE_DATA *XgbeData,
  AMD_XGBE_I2C_MASTER_PRIVATE *I2cPrivate
  )
{
  UINT16 Data;
  UINT8 SlaveAdd;
  UINT16 DataSize;
  UINT8 Offset;
  XGBE_I2C_MASTER_PRIVATE *XgbeI2cMasterPrivate = NULL;
  EFI_STATUS Status;

  DEBUG ((EFI_D_INFO, "In XgbeTxDisableLow Start \n"));
  DEBUG ((EFI_D_INFO, "XgbeData->Xgmac0BaseAdd=%x, XgbeData->Xgmac1BaseAdd=%x line(%d)\n", XgbeData->Xgmac0BaseAdd, XgbeData->Xgmac1BaseAdd, __LINE__));

  // Port 0
  Status = EFI_SUCCESS;
  if (XgbeData->Xgmac0BaseAdd) {
    SlaveAdd = (UINT8)(PCA9535_ADDRESS + getBitfieldReg32 (XgbeData->Xgmac0BaseAdd + ENET_PORT_PROPERTY_3, 8, 3));
    DEBUG ((EFI_D_ERROR, " PORT 0 PCA9535 SlaveAdd = 0x%x \n", SlaveAdd));
    DataSize = sizeof (UINT16);
    XgbeI2cMasterPrivate = &I2cPrivate->I2cMasterPpi[0];

    Offset = 6;       // PCA9535 Config register offset
    Data = 0;
    Data = XgbeReadEeprom(  XgbeI2cMasterPrivate, SlaveAdd, Offset, DataSize);
    DEBUG ((EFI_D_INFO, " PCA9535 Config register read data = %x \n", Data));
    
    // I/O 0_7 to I/O 0_0 (  0 => output and 1 => input)
    // (0111 1111 = 0x7F)
    // I/O 1_7 to I/O 1_0 (0,1,1,1,1,1,1,1) ( 0111 1111 = 0x7F)
    Data &= 0x7F7F; // This statement make sure output set as 0 properly
    Data |= 0x7F7F; // This statement make sure input set as 1 properly
    DEBUG ((EFI_D_INFO, " PCA9535 Config register before write data = %x \n", Data));
    Status = XgbeWriteEeprom(XgbeI2cMasterPrivate, SlaveAdd, Offset, Data);
    DEBUG ((EFI_D_ERROR, "XgbeWriteEeprom() Status = %r line(%d)\n", Status, __LINE__));

    Data = 0;
    Data = XgbeReadEeprom(  XgbeI2cMasterPrivate, SlaveAdd, Offset, DataSize);
    DEBUG ((EFI_D_INFO, " PCA9535 Config register read data = %x \n", Data));  

    Offset = 2;       // PCA9535 output port register. 
    Data = XgbeReadEeprom(  XgbeI2cMasterPrivate, SlaveAdd, Offset, DataSize);

    // SFP+_XGB1_DISABLE (I/O 0_7) =>  High:Disable the transmitter  Low : Enable the Transmiter
    // SFP+_XGB0_DISABLE (I/O 1_7) =>  High:Disable the transmitter  Low : Enable the Transmiter
    // Input port value is ignored even if we configure any value.
    Data &= 0x0000; 

    Status = XgbeWriteEeprom(XgbeI2cMasterPrivate, SlaveAdd, Offset, Data);
    DEBUG ((EFI_D_ERROR, "XgbeWriteEeprom() Status = %r line(%d)\n", Status, __LINE__));

    Data = 0;
    Data = XgbeReadEeprom(XgbeI2cMasterPrivate, SlaveAdd, Offset, DataSize);
    DEBUG ((EFI_D_ERROR, " Xgmac0 PCA9535 Output port register read data = %x \n", Data));
  }

  //Port 1
  Status = EFI_SUCCESS;
  if (XgbeData->Xgmac1BaseAdd) {
    if (XgbeData->Xgmac0BaseAdd == 0) {
      // if port-0 is disabled, then get the I2COwnership for port-1
      GetI2cOwnership (PeiServices, XgbeData->Xgmac1BaseAdd, 1);
    }

    SlaveAdd = (UINT8)(PCA9535_ADDRESS + getBitfieldReg32 (XgbeData->Xgmac1BaseAdd + ENET_PORT_PROPERTY_3, 8, 3));
    DEBUG ((EFI_D_ERROR, " PORT 1 PCA9535 SlaveAdd = 0x%x \n", SlaveAdd));
    DataSize = sizeof (UINT16);
    XgbeI2cMasterPrivate = &I2cPrivate->I2cMasterPpi[1];

    Offset = 6;       // PCA9535 Config register offset
    Data = XgbeReadEeprom(  XgbeI2cMasterPrivate, SlaveAdd, Offset, DataSize);
    DEBUG ((EFI_D_ERROR, " PCA9535 Config register read data = %x \n", Data));

    // I/O 0_7 to I/O 0_0 (  0 => output and 1 => input)
    // (0111 1111 = 0x7F)
    // I/O 1_7 to I/O 1_0 (0111 1111 = 0x7F)
    Data &= 0x7F7F; // This statement make sure output set as 0 properly
    Data |= 0x7F7F; // This statement make sure input set as 1 properly
    DEBUG ((EFI_D_ERROR, " PCA9535 Config register before write data = %x \n", Data));
    Status = XgbeWriteEeprom(XgbeI2cMasterPrivate, SlaveAdd, Offset, Data);
    DEBUG ((EFI_D_ERROR, "XgbeWriteEeprom() Status = %r line(%d)\n", Status, __LINE__));

    Data = 0;
    Data = XgbeReadEeprom(  XgbeI2cMasterPrivate, SlaveAdd, Offset, DataSize);
    DEBUG ((EFI_D_ERROR, " PCA9535 Config register read data = %x \n", Data));  

    Offset = 2;       // PCA9535 output port register. 
    Data = XgbeReadEeprom(XgbeI2cMasterPrivate, SlaveAdd, Offset, DataSize);

    // SFP+_XGB1_DISABLE (I/O 0_7) =>  High:Disable the transmitter  Low : Enable the Transmiter
    // SFP+_XGB0_DISABLE (I/O 1_7) =>  High:Disable the transmitter  Low : Enable the Transmiter
    Data &= 0x0000; 
    Status = XgbeWriteEeprom(XgbeI2cMasterPrivate, SlaveAdd, Offset, Data);
    DEBUG ((EFI_D_ERROR, "XgbeWriteEeprom() Status = %r line(%d)\n", Status, __LINE__));

    Data = 0;
    Data = XgbeReadEeprom(XgbeI2cMasterPrivate, SlaveAdd, Offset, DataSize);
    DEBUG ((EFI_D_ERROR, "Xgmac1 PCA9535 Output port register read data = %x \n", Data));

    if (XgbeData->Xgmac0BaseAdd == 0) {
      // if port-0 is disabled, then release the I2COwnership for port-1
      PutI2cOwnership (XgbeData->Xgmac1BaseAdd, 1);
    }
  }

  DEBUG ((EFI_D_ERROR, "In XgbeTxDisableLow End \n"));
  return Status;
}


/**
  @param  PeiServices                 Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor            Address of the notification descriptor data structure. Type
          EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param  Ppi                         Address of the PPI that was installed.

  @retval   EFI_STATUS                Returns EFI_SUCCESS by default.

**/
EFI_STATUS
EFIAPI
S3ResumeXGBEWorkaround (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{

  EFI_STATUS   Status = EFI_SUCCESS;
  XGBE_DATA    *XgbeData;
  AMD_XGBE_I2C_MASTER_PRIVATE *I2cPrivate;

  XgbeData = AllocateZeroPool (sizeof (XGBE_DATA));
  if (XgbeData == NULL) {
    DEBUG ((EFI_D_ERROR, "Could not allocate XGBE_DATA structures.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Create the XGBE_I2CMASTER Private Data
  //
  Status = (*PeiServices)->AllocatePool (
                             (CONST EFI_PEI_SERVICES**)PeiServices,
                             sizeof (AMD_XGBE_I2C_MASTER_PRIVATE),
                             (VOID**)&I2cPrivate
                             );
  ASSERT_EFI_ERROR (Status);
  Status = (**PeiServices).LocatePpi (
                               (CONST EFI_PEI_SERVICES**)PeiServices,
                               &gEfiPeiStallPpiGuid,
                               0,
                               NULL,
                               (VOID **)&gStallPpi
                               );
  Status = XgbeGetBaseAddr ((CONST EFI_PEI_SERVICES**)PeiServices , XgbeData); 
  XgbeI2cInit((CONST EFI_PEI_SERVICES**)PeiServices , XgbeData,I2cPrivate);
  Status = XgbeTxDisableLow ((CONST EFI_PEI_SERVICES**)PeiServices , XgbeData,I2cPrivate);
  return Status;
}



/*********************************************************************************
 * Name: AmdXgbeWorkaroundPeiInit
 *
 * Description
 *   Entry point of the AMD XGBE WORK AROUND Pei driver
 *   Perform the required work around needed for XGBE.
 *
 *  @param[in]         FileHandle          Handle of the file being invoked.
 *  @param[in]         PeiServices         Describes the list of possible PEI Services.
 *
 *  @retval            EFI_SUCCESS         Module initialized successfully.
 *  @retval            EFI_ERROR           Initialization failed (see error for more details).
 *
 *********************************************************************************/

EFI_STATUS
EFIAPI
AmdXgbeWorkaroundPeiInit (
  IN EFI_PEI_FILE_HANDLE             FileHandle,
  IN CONST EFI_PEI_SERVICES        **PeiServices
  )
{
  EFI_STATUS   Status = EFI_SUCCESS;
  EFI_BOOT_MODE   BootMode;

  DEBUG ((EFI_D_INFO, "In AmdXgbeWorkaroundPeiInit\n"));

  if(!PcdGetBool (PcdXgbeSfp)) {
    return EFI_SUCCESS;
  }

  if(PcdGet8 (PcdBoardRevision)== REV_A_BOARD) {
    DEBUG ((EFI_D_INFO, "Bilby Rev A board:skipping XGBE workaround\n"));
    return (Status);
  }

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);

  if (!EFI_ERROR (Status) && (BootMode != BOOT_ON_S3_RESUME)) {
    //
    // Do nothing except S3 Resume
    //
    return EFI_SUCCESS;
  }

  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    FchSmnRW8 (0, FCH_SS_SMN_IOMUX_BASE + 0x09,0x00, 0x2, NULL);
    FchSmnRW8 (0, FCH_SS_SMN_IOMUX_BASE + 0x17,0x00, 0x2, NULL);

    FchSmnRW8 (0, FCH_SS_SMN_IOMUX_BASE + 0x0A,0x00, 0x3, NULL);
    FchSmnRW8 (0, FCH_SS_SMN_IOMUX_BASE + 0x28,0x00, 0x2, NULL); //PLAT-18025

    Status = (*PeiServices)->NotifyPpi(PeiServices, &mNotifyOnPostScriptList);
    ASSERT_EFI_ERROR (Status);
  }

  return (Status);
}
