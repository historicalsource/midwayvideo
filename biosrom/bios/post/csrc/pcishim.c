#include <compiler.h>
#include <machine/idtcpu.h>
#include <machine/seattle.h>
#include <machine/gt64010.h>
#include "nsc_pcic.h"

typedef union _reg {
  mxU8      b[4];
  mxU16     w[2];
  mxU32     l;
} _REG;

static int      _gt64010Rev = 0;


int getGT64010Rev(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    _PCI_INTERNAL_CONFIG_ADDR   ca;
    _PCI_CONFIG_REG2            ccr;


    if (_gt64010Rev == 0) {
        ca.reg = RESET;

        ca.s.regNum    = GT_PCI_CLASS_CODE_REV;
        ca.s.devNum    = GT64010_DEVICE_NUMBER;
        ca.s.configEna = ALLOW_ACCESS;

        *((mxVU32 *)PHYS_TO_K1(GT64010_BASE+GT_PCI_CONFIG_ADDR)) = ca.reg;

        ccr.reg = *((mxVU32 *)PHYS_TO_K1(GT64010_BASE+GT_PCI_CONFIG_DATA));

        if (ccr.s.revision == 2) {
            _gt64010Rev = 2;
        } else {
            _gt64010Rev = 1;
        }
    }

    return (_gt64010Rev);
}


mxU32 get_pci_config_reg(mxU32 devNum, mxU32 regNum) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
    Return data read from PCI configuration register 'regNum' on the
    PCI device 'devNum'.

    devNum - PCI device number.
    regNum - register number.

    return - data read from device.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    _PCI_INTERNAL_CONFIG_ADDR   ca;


    ca.reg = RESET;

    ca.s.regNum    = regNum;
    ca.s.devNum    = devNum;
    ca.s.configEna = ALLOW_ACCESS;

    if (!devNum && _gt64010Rev == 1) ca.s.regNum <<= 2;

    *((mxVU32 *)PHYS_TO_K1(GT64010_BASE+GT_PCI_CONFIG_ADDR)) = ca.reg;

    return (*((mxVU32 *)PHYS_TO_K1(GT64010_BASE+GT_PCI_CONFIG_DATA)));
}


void put_pci_config_reg(mxU32 devNum, mxU32 regNum, mxU32 data) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
    Write 'data' to _PCI configuration register 'regNum'
    on PCI device 'devNum'.

    devNum - PCI device number.
    regNum - register number.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    _PCI_INTERNAL_CONFIG_ADDR   ca;


    ca.reg = RESET;

    ca.s.regNum    = regNum;
    ca.s.devNum    = devNum;
    ca.s.configEna = ALLOW_ACCESS;

    if (!devNum && _gt64010Rev == 1) ca.s.regNum <<= 2;

    *((mxVU32 *)PHYS_TO_K1(GT64010_BASE+GT_PCI_CONFIG_ADDR)) = ca.reg;
    *((mxVU32 *)PHYS_TO_K1(GT64010_BASE+GT_PCI_CONFIG_DATA)) = data;
}


mxU8 PCI_ReadConfigByte(DEVHANDLE devhand, mxU8 regaddr) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    _REG    regval;


  regval.l = get_pci_config_reg(devhand, (regaddr >> 2));

  return (regval.b[regaddr&0x03]);
}


void PCI_WriteConfigByte(DEVHANDLE devhand, mxU8 regaddr, mxU8 regval) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    _REG    curval;


    curval.l = get_pci_config_reg(devhand, (regaddr >> 2));

    curval.b[regaddr&0x03] = regval;

    put_pci_config_reg(devhand, (regaddr>>2), curval.l);
}


void PCI_WriteConfigWord(DEVHANDLE devhand, mxU8 regaddr, mxU16 regval) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    _REG    curval;


    curval.l = get_pci_config_reg(devhand, (regaddr >> 2));

    curval.w[regaddr&0x02] = regval;

    put_pci_config_reg(devhand, (regaddr >> 2), curval.l);
}


void PCI_WriteConfigDword(DEVHANDLE devhand, mxU8 regaddr, mxU32 regval) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    put_pci_config_reg(devhand, regaddr>>2, regval);
}


mxU32 get_sst_device_number(mxU32 vendorId) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU32   devNum;
    mxU32   id;


    for (devNum = PCI_SLOT_0_DEVICE_NUMBER; devNum <= PC87415_DEVICE_NUMBER; devNum++) {
        id = get_pci_config_reg(devNum, PCI_CR0) & 0xFFFF;
        if (id == vendorId) return (devNum);
    }

    return (-1);
}


void *get_sst_addr(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{

    return ((void *)PHYS_TO_K1(PCI_MEMORY_BASE));

}


void *pciMapCardMulti(mxU32 vendorID, mxU32 deviceID, mxS32 memsize, mxU32 *devNum, mxU32 brdNum) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
	void	*addr;
    mxU32   deviceNum;


    if (brdNum) return ((void *)0);

    deviceNum = get_sst_device_number(vendorID);

    if (deviceNum >= 0) {
        addr = get_sst_addr();

        get_pci_config_reg(deviceNum, PCI_CR4);
        put_pci_config_reg(deviceNum, PCI_CR4, ((mxU32)addr & 0x1FFFFFFF));
    } else {
        addr = (void *)0;
        deviceNum = 0;
    }

    *devNum = deviceNum;

    return (addr);
}
