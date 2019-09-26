#include "bram.h"

XBram Bram;

/*****************************************************************************/
/**
 *
 * This is the entry point for the BRAM example.
 *
 * @param	DeviceId is the XPAR_<BRAM_instance>_DEVICE_ID value from
 *		xparameters.h
 *
 * @return
 *		- XST_SUCCESS to indicate success.
 *		- XST_FAILURE to indicate failure.
 *
 * @note		None.
 *
 ******************************************************************************/
int BramExample(u16 DeviceId) {
  int Status;
  XBram_Config *ConfigPtr;

  /*
   * Initialize the BRAM driver. If an error occurs then exit
   */

  /*
   * Lookup configuration data in the device configuration table.
   * Use this configuration info down below when initializing this
   * driver.
   */
  ConfigPtr = XBram_LookupConfig(DeviceId);
  if (ConfigPtr == (XBram_Config *)NULL) {
    return XST_FAILURE;
  }

  Status = XBram_CfgInitialize(&Bram, ConfigPtr, ConfigPtr->CtrlBaseAddress);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  InitializeECC(ConfigPtr, ConfigPtr->CtrlBaseAddress);

  /*
   * Execute the BRAM driver selftest.
   */
  Status = XBram_SelfTest(&Bram, 0);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE;
  }

  return XST_SUCCESS;
}

/****************************************************************************/
/**
 *
 * This function ensures that ECC in the BRAM is initialized if no hardware
 * initialization is available. The ECC bits are initialized by reading and
 * writing data in the memory. This code is not optimized to only read data
 * in initialized sections of the BRAM.
 *
 * @param	ConfigPtr is a reference to a structure containing information
 *		about a specific BRAM device.
 * @param 	EffectiveAddr is the device base address in the virtual memory
 *		address space.
 *
 * @return
 *		None
 *
 * @note		None.
 *
 *****************************************************************************/
void InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr) {
  u32 Addr;
  volatile u32 Data;

  if (ConfigPtr->EccPresent && ConfigPtr->EccOnOffRegister &&
      ConfigPtr->EccOnOffResetValue == 0 && ConfigPtr->WriteAccess != 0) {
    for (Addr = ConfigPtr->MemBaseAddress; Addr < ConfigPtr->MemHighAddress;
         Addr += 4) {
      Data = XBram_In32(Addr);
      XBram_Out32(Addr, Data);
    }
    XBram_WriteReg(EffectiveAddr, XBRAM_ECC_ON_OFF_OFFSET, 1);
  }
}

int BramCheck() { return BramExample(BRAM_DEVICE_ID); }
