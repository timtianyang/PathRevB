/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
#include <xc.h>
#elif defined(__C30__)
#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif
#endif

#include <stdint.h>        /* Includes uint16_t definition   */
#include <stdbool.h>       /* Includes true/false definition */

/******************************************************************************/
/* Interrupt Vector Options                                                   */
/******************************************************************************/
/*                                                                            */
/* Refer to the C30 (MPLAB C Compiler for PIC24F MCUs and dsPIC33F DSCs) User */
/* Guide for an up to date list of the available interrupt options.           */
/* Alternately these names can be pulled from the device linker scripts.      */
/*                                                                            */

/* dsPIC33E Primary Interrupt Vector Names:                                   */
/*                                                                            */
/* _INT0Interrupt     _IC4Interrupt      _U4TXInterrupt                       */
/* _IC1Interrupt      _IC5Interrupt      _SPI3ErrInterrupt                    */
/* _OC1Interrupt      _IC6Interrupt      _SPI3Interrupt                       */
/* _T1Interrupt       _OC5Interrupt      _OC9Interrupt                        */
/* _DMA0Interrupt     _OC6Interrupt      _IC9Interrupt                        */
/* _IC2Interrupt      _OC7Interrupt      _PWM1Interrupt                       */
/* _OC2Interrupt      _OC8Interrupt      _PWM2Interrupt                       */
/* _T2Interrupt       _PMPInterrupt      _PWM3Interrupt                       */
/* _T3Interrupt       _DMA4Interrupt     _PWM4Interrupt                       */
/* _SPI1ErrInterrupt  _T6Interrupt       _PWM5Interrupt                       */
/* _SPI1Interrupt     _T7Interrupt       _PWM6Interrupt                       */
/* _U1RXInterrupt     _SI2C2Interrupt    _PWM7Interrupt                       */
/* _U1TXInterrupt     _MI2C2Interrupt    _DMA8Interrupt                       */
/* _AD1Interrupt      _T8Interrupt       _DMA9Interrupt                       */
/* _DMA1Interrupt     _T9Interrupt       _DMA10Interrupt                      */
/* _NVMInterrupt      _INT3Interrupt     _DMA11Interrupt                      */
/* _SI2C1Interrupt    _INT4Interrupt     _SPI4ErrInterrupt                    */
/* _MI2C1Interrupt    _C2RxRdyInterrupt  _SPI4Interrupt                       */
/* _CM1Interrupt      _C2Interrupt       _OC10Interrupt                       */
/* _CNInterrupt       _QEI1Interrupt     _IC10Interrupt                       */
/* _INT1Interrupt     _DCIEInterrupt     _OC11Interrupt                       */
/* _AD2Interrupt      _DCIInterrupt      _IC11Interrupt                       */
/* _IC7Interrupt      _DMA5Interrupt     _OC12Interrupt                       */
/* _IC8Interrupt      _RTCCInterrupt     _IC12Interrupt                       */
/* _DMA2Interrupt     _U1ErrInterrupt    _DMA12Interrupt                      */
/* _OC3Interrupt      _U2ErrInterrupt    _DMA13Interrupt                      */
/* _OC4Interrupt      _CRCInterrupt      _DMA14Interrupt                      */
/* _T4Interrupt       _DMA6Interrupt     _OC13Interrupt                       */
/* _T5Interrupt       _DMA7Interrupt     _IC13Interrupt                       */
/* _INT2Interrupt     _C1TxReqInterrupt  _OC14Interrupt                       */
/* _U2RXInterrupt     _C2TxReqInterrupt  _IC14Interrupt                       */
/* _U2TXInterrupt     _QEI2Interrupt     _OC15Interrupt                       */
/* _SPI2ErrInterrupt  _U3ErrInterrupt    _IC15Interrupt                       */
/* _SPI2Interrupt     _U3RXInterrupt     _OC16Interrupt                       */
/* _C1RxRdyInterrupt  _U3TXInterrupt     _IC16Interrupt                       */
/* _C1Interrupt       _USB1Interrupt     _ICDInterrupt                        */
/* _DMA3Interrupt     _U4ErrInterrupt    _PWMSpEventMatchInterrupt            */
/* _IC3Interrupt      _U4RXInterrupt     _PWMSecSpEventMatchInterrupt         */
/*                                                                            */
/* For alternate interrupt vector naming, simply add 'Alt' between the prim.  */
/* interrupt vector name '_' and the first character of the primary interrupt */
/* vector name.  There is no Alternate Vector or 'AIVT' for the 33E family.   */
/*                                                                            */
/* For example, the vector name _ADC2Interrupt becomes _AltADC2Interrupt in   */
/* the alternate vector table.                                                */
/*                                                                            */
/* Example Syntax:                                                            */
/*                                                                            */
/* void __attribute__((interrupt,auto_psv)) <Vector Name>(void)               */
/* {                                                                          */
/*     <Clear Interrupt Flag>                                                 */
/* }                                                                          */
/*                                                                            */
/* For more comprehensive interrupt examples refer to the C30 (MPLAB C        */
/* Compiler for PIC24 MCUs and dsPIC DSCs) User Guide in the                  */
/* <C30 compiler instal directory>/doc directory for the latest compiler      */
/* release.  For XC16, refer to the MPLAB XC16 C Compiler User's Guide in the */
/* <XC16 compiler instal directory>/doc folder.                               */
/*                                                                            */
/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* TODO Add interrupt routine code here. */


//control flags:
volatile unsigned int samplingFlag=0;





const char* DEVICE_INFO="Hello, this is PathMeT RevB\0";//do not change this. BBB uses this to test communication.

volatile unsigned int device_info_ptr=0;

volatile unsigned long time = 0;
volatile unsigned char laserBuff[4]={0xA0,0xA1,0xA2,0xA3};
volatile unsigned char IMUBuff[18]={0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC,
0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC, 0xAC};
volatile unsigned char measurements[2048];

volatile unsigned int spi_trans_ptr=0;//buffer block ptr
volatile unsigned int temp_ptr=0;//block * 128
volatile unsigned int bufferPtr=0;//ptr inside a block

//A0 lazer, B0C0 Enc, Ac IMU 256 bytes=2*(15*7+18)      lazer IMU ratio=7:1


void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void) {
    IFS0bits.T1IF = 0;
    time++;
   // if (time % 1000 == 0) {
       // LATBbits.LATB6 = ~LATBbits.LATB6;
  //  }
}

void __attribute__((__interrupt__, no_auto_psv)) _SPI1Interrupt(void) {
    volatile unsigned char buff;

    IFS0bits.SPI1IF = 0;
    SPI1STATbits.SPIROV = 0;
    buff = SPI1BUF; //read

    switch (buff) {
        case 0xAC://master is acquiring the main buffer
            SPI1BUF = measurements[bufferPtr+temp_ptr];
            bufferPtr = (bufferPtr + 1)&0x7F; //wrap around at index 127
            break;
        case 0xBC://master prepares reading the main buffer
            bufferPtr = 0; //reset bufferPtr
            temp_ptr=spi_trans_ptr<<7;         
            spi_trans_ptr=(spi_trans_ptr+1)&0xF;//wrap around
            SPI1BUF = measurements[bufferPtr+temp_ptr];
            bufferPtr++;
            break;
        case 0xAE:
            SPI1BUF=*(DEVICE_INFO+(device_info_ptr++));
            if(device_info_ptr==28)
            {
               device_info_ptr=0;
            }
            break;
        case 0xBE:
            device_info_ptr=0;
            SPI1BUF=*(DEVICE_INFO+(device_info_ptr++));
            break;

        case 0xAD:
            samplingFlag=1;
            SPI1BUF=0xAD;//respond back
            break;

        case 0xBD:
            samplingFlag=0;
            SPI1BUF=0xBD;//respond back
           
            break;
        default:
            SPI1BUF=0x01;
            break;
    }

}


/*void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
    unsigned char temp = U1RXREG; //read the byte
    U1TXREG = temp; // Transmit one character back
   // LATBbits.LATB15 = ~LATBbits.LATB15;
    IFS0bits.U1RXIF = 0; // Clear RX Interrupt flag
}
 */

