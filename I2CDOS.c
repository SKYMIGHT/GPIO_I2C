
//

#include "sys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Bus 0, Device 17, Function 0
#define PCIAddr     0x80008800  
#define PCICFGADDR  0xCF8
#define PCICFGDATA  0xCFC

#define MAX_NUM_RETRY    20

#define VX11_PCI_Offset        0x94
#define PMIOBaseOffset  0x88

// Offser of PMIO
#define PMIO_GPIO_IN    0x48   // offset 0x49 for input pin
#define PMIO_GPIO_OUT   0x4C   // offset 0x4D for output pin/Open Drain control

// delay time
#define I2C_WAIT_TIME    200L

// GPIO[0]
#define I2C_GPIO_SDA_MASK       0x00000400        // Rx49[2]   read
#define I2C_GPIO_SDA_DATA       0x00000800        // Rx4D[3]   R/W
// GPIO[1]
#define I2C_GPIO_SCL_MASK       0x00000800        // Rx49[3]   read
#define I2C_GPIO_SCL_DATA       0x00001000        // Rx4D[4]   R/W

#define EEPROM_READ    0xA1  //The slave address "0xA1" to read data
#define EEPROM_WRITE   0xA0  //The slave address "0xA0" to write data
tU4 dwPMIOBase = 0;


tU4 SetGPIOPinSelect();
void WriteDevSerialBus(int SlaveAddr, int iSubAddress, tU1 Data);
int ReadDevSerialBus(int SlaveAddr, int iSubAddress);
tU2 Check();

tU4 main(tUi argc, const tU1 *argv[])
{
        
        printf("I2C_DOS version:1.0 \n");
        SetGPIOPinSelect();
        
        return(Check());        
}

tU4 SetGPIOPinSelect()
{
        tU4 dwValue;

        SysPortPutU4(PCICFGADDR, PCIAddr|PMIOBaseOffset);
        dwPMIOBase = SysPortGetU4(PCICFGDATA);
        dwPMIOBase = dwPMIOBase & 0x0000FF80;
        //printf("dwPMIOBase =0x%X\n",dwPMIOBase);

        // initial GPIO[0] and GPIO[1]
        SysPortPutU4(PCICFGADDR, PCIAddr|VX11_PCI_Offset);
        dwValue = SysPortGetU4(PCICFGDATA);

        // enable GPIO[0], GPIO[1] by set offset 0x95 bit-2 and 3 to "1".
        dwValue = dwValue | 0x00000600;
        SysPortPutU4(PCICFGDATA, dwValue);

        if(!dwPMIOBase)
                return 0;

        return dwPMIOBase;
}

void delayIn_usec(tU4 dwDelayTime)
{
    tU4 dwIndex;

    for(dwIndex = dwDelayTime; dwIndex!=0; dwIndex--) 
    {
        SysPortGetU4(0x3c3);
    }
}


//////////////////////////////////////
/// Set SCL
void SetSCLStatus(tU4 iStatus)
{
        tU4     dwTmp = 0;
//    SerialDataAccessControl(I2C_WRITE_SCL, iStatus);
    dwTmp = SysPortGetU4( (dwPMIOBase|PMIO_GPIO_OUT));
            
    if(iStatus)
    {
        SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwTmp | I2C_GPIO_SCL_DATA));
    }
    else     
    {
        SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwTmp & (~I2C_GPIO_SCL_DATA)));
    }
    
    if(iStatus) 
    {
        delayIn_usec(I2C_WAIT_TIME);
    }
}

//////////////////////////////////////
/// Set SDA
void SetSDAStatus(tU4 iStatus)
{
        tU4     dwTmp = 0;
//    SerialDataAccessControl(I2C_WRITE_SDA, iStatus);


    dwTmp = SysPortGetU4( (dwPMIOBase|PMIO_GPIO_OUT));
            
    if(iStatus)
    {
        SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwTmp | I2C_GPIO_SDA_DATA));
    }
    else     
    {
        SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwTmp & (~I2C_GPIO_SDA_DATA)));
    }


    delayIn_usec(I2C_WAIT_TIME);
}


//////////////////////////////////////
/// Get SDA
tU4  GetSDAStatus(void)
{
    tU4    dwData;

        // Open Drain control
//    dwData = SysPortGetU4( (dwPMIOBase|PMIO_GPIO_OUT));

//    SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwData | I2C_GPIO_SDA_DATA));

    dwData = SysPortGetU4( (dwPMIOBase|PMIO_GPIO_IN));

    if(dwData & I2C_GPIO_SDA_MASK) 
    {
        return 1;
    }
    else         
    {
        return 0;
    }
}

//////////////////////////////////////
/// START Condition
tU4 GPIOBusStart(void)
{
    SetSDAStatus(1);    
    SetSCLStatus(1);    
    SetSDAStatus(0);    
    SetSCLStatus(0);

    return 1; 
}

//////////////////////////////////////
/// STOP Condition
tU4 GPIOBusStop(void)
{
    SetSDAStatus(0);
    SetSCLStatus(1);
    SetSDAStatus(1);

//    SerialDataAccessControl(I2C_RELEASE, 0);

    return 1;
}

//////////////////////////////////////
/// Send NACK
tU4 SendNoneAcknowledge(void)
{
    SetSDAStatus(1);
    SetSCLStatus(1);
    SetSCLStatus(0);
    delayIn_usec(I2C_WAIT_TIME);

    return 1;
}

//////////////////////////////////////
/// /CHCheck ACK
tU4 ChkAcknowledge(void)
{
    tU4 Status;
    tU4    dwData;

//    SerialDataAccessControl(I2C_READ_SDA, 0);
        // Open Drain control
    dwData = SysPortGetU4( (dwPMIOBase|PMIO_GPIO_OUT));
    SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwData | I2C_GPIO_SDA_DATA));

    delayIn_usec(I2C_WAIT_TIME);
    SetSCLStatus(1);
    Status = GetSDAStatus();
//    SerialDataAccessControl(I2C_WRITE_SDA, Status);
    dwData = SysPortGetU4( (dwPMIOBase|PMIO_GPIO_OUT));
//    SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwData & ~I2C_GPIO_SDA_DATA));
    if(Status) 
    {
                SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwData | I2C_GPIO_SDA_DATA));
    }
    else         
    {
                SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwData & ~I2C_GPIO_SDA_DATA));
    }

    SetSCLStatus(0);
    delayIn_usec(I2C_WAIT_TIME);

    if(Status) 
    {
        return 0;
    }
    else 
    {
        return 1;
    }
}

//////////////////////////////////////
/// Read Data(Bit by Bit) from I2C
tU1 ReadSerialBusData(void)
{
    tU4     i;
    tU1    Data = 0;
    tU4    dwData;

    for(i=0; i<8; i++) 
    {
        Data <<= 1;
//        SerialDataAccessControl(I2C_READ_SDA, 0);
                // Open Drain control
            dwData = SysPortGetU4( (dwPMIOBase|PMIO_GPIO_OUT));
            SysPortPutU4((dwPMIOBase|PMIO_GPIO_OUT),(dwData | I2C_GPIO_SDA_DATA));
        SetSCLStatus(1);
        if(GetSDAStatus()) Data++;
        SetSCLStatus(0);
        delayIn_usec(I2C_WAIT_TIME);
    }

    return  Data;
}

//////////////////////////////////////
/// Write Data(Bit by Bit) to I2C
tU4 WriteSerialBusData(tU1 Data)
{
    tU4     i;
    tU2    d;

    d = (tU2)Data;

    for(i=0; i<8; i++) 
    {
        delayIn_usec(I2C_WAIT_TIME/5);

        if((d&0x80)==0x80) SetSDAStatus(1);
        else               SetSDAStatus(0);

        delayIn_usec(I2C_WAIT_TIME/5);

        SetSCLStatus(1);
        SetSCLStatus(0);
        d <<= 1;
    }

    return ChkAcknowledge();
}

//////////////////////////////////////////////////////
/// Write Data(one tU1) to Desired Device on I2C
void WriteDevSerialBus(int SlaveAddr, int iSubAddress, tU1 Data)
{
    tU4 iRetryCount;
    
    for(iRetryCount = 1; iRetryCount <= MAX_NUM_RETRY; iRetryCount++)
    {
        GPIOBusStart();        
        if(!WriteSerialBusData(SlaveAddr)) {
            GPIOBusStop();
            continue;
        }
        
        if(!WriteSerialBusData(iSubAddress)) {
            GPIOBusStop();
            continue;
        }
        
        if(!WriteSerialBusData(Data&0xFF)) {
            GPIOBusStop();
            continue;
        }
        
        break;
    }
    GPIOBusStop();
}

//////////////////////////////////////////////////////
/// Read Data from Desired Device on I2C
int ReadDevSerialBus(int SlaveAddr, int iSubAddress)
{
    int ReadData = 0;
    tU4 iRetryCount;

    for(iRetryCount = 1; iRetryCount <= MAX_NUM_RETRY; iRetryCount++)
    {
        if(!GPIOBusStart())
        {
            continue;
        }

        if(!WriteSerialBusData(SlaveAddr & 0xFE))
        {
            GPIOBusStop();
            continue;
        }

        if(!WriteSerialBusData(iSubAddress))
        {
            GPIOBusStop();
            continue;
        }
        break;
    }

    for(iRetryCount = 1; iRetryCount <= MAX_NUM_RETRY; iRetryCount++)
    {
        if(!GPIOBusStart())
        {
            continue;
        }

        if(!WriteSerialBusData(SlaveAddr | 0x01))
        {
            GPIOBusStop();
            continue;
        }

        ReadData = ReadSerialBusData();

        SendNoneAcknowledge();
        GPIOBusStop();

        break;
    }

    return ReadData;
}

tU2 Check()
{                                  
        
        tU4 i, n = 0;
        
        printf("Start Check EEPROM AT24C04C Value by I2C!\n");
        printf("Write data to EEPROM AT24C04C...\n");
        for(i = 0; i < 512 ; i ++)
        {
                
                WriteDevSerialBus(EEPROM_WRITE, i, 0);
                
        }
        
        for(i = 0; i < 512 ; i ++)
        {
                if(i % 256 == 0 )
                {
                        n = 0;
                }
                WriteDevSerialBus(EEPROM_WRITE, i, n);
        
             
                //printf("address %d = %d\n",i,ReadDevSerialBus(EEPROM_READ, i));
                n++;
        }
        
        printf("Read data from EEPROM AT24C04C and Compare Value...\n");
        for(i = 0; i < 512 ; i ++)
        {
                if(i % 256 == 0 )
                {
                        n = 0;
                }
                
                if(ReadDevSerialBus(EEPROM_READ, i) != n)
                {
                        
                        //printf("Check EEPROM AT24C04 Value Wrong by I2C: offset 0x%X!!\n",i);
                        printf("Check EEPROM AT24C04C Value Failed by I2C!\n");
                        return 0;
                        
                }
                n++;
        }
        
        printf("Check EEPROM AT24C04C Value Pass by I2C!\n");
        
        for(i = 0; i < 512 ; i ++)
        {
                
                WriteDevSerialBus(EEPROM_WRITE, i, 0);
                
        }
        printf("Clean EEPROM AT24C04C by I2C.\n");
        
        return 1;
        
      
         
}

