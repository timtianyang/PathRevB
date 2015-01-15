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

//used for __delay_ms function
#define FOSC    (140000000ULL)
#define FCY     (FOSC/2)
#include <libpic30.h>

#include <stdint.h>          /* For uint16_t definition                       */
#include <stdbool.h>         /* For true/false definition                     */
#include <string.h>
#include "user.h"            /* variables/params used by user.c               */
#include "interrupt.h"

/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/* <Initialize variables in user.h and insert code for user algorithms.> */

void InitApp(void) {
    InitIO();
    MapPins();
    InitTimer();
    InitSPI1();
    //  InitINT1();
}

void InitIO(void) {
    ANSELA = 0x00; // Convert all I/O pins to digital
    ANSELB = 0x00;

    TRISBbits.TRISB6 = 0; //led
    TRISBbits.TRISB15 = 0; //CTS BBB data is ready
    TRISBbits.TRISB1 = 1; //RTS from BBB
    LATBbits.LATB6 = 0;
    LATBbits.LATB15 = 0;


    // TRISBbits.TRISB0 = 0; //for UART1
    // TRISBbits.TRISB1 = 1;

    TRISBbits.TRISB7 = 1; //SPI1CLK
    TRISBbits.TRISB9 = 1; //SPI1SDI
    TRISBbits.TRISB8 = 0; //SPI1SDO
    TRISBbits.TRISB0 = 1; //SPI1SS

}

void MapPins(void) {
    //mapping
    __builtin_write_OSCCONL(OSCCON & ~(1 << 6));
    //    RPINR18bits.U1RXR = 1; // Assign U1Rx To Pin RP1
    //    RPOR0bits.RP0R = 3; // Assign U1Tx To Pin RP0
    // RPINR0bits.INT1R=0b010 0001; //map IN1 to B.1
    // Lock Registers
    //*************************************************************
    __builtin_write_OSCCONL(OSCCON | (1 << 6));
}

void InitTimer(void) {//1 ms
    //Timer1
    //Prescaler 1:8; PR1 Preload = 8750; Actual Interrupt Time = 1 ms
    T1CONbits.TON = 0; // Disable Timer

    T1CON = 0x8010;
    IPC0 = IPC0 | 0x1000;
    PR1 = 8750;

    IFS0bits.T1IF = 0; // Reset Timer1 interrupt flag
    IPC0bits.T1IP = 5; // Timer1 Interrupt priority level=4
    IEC0bits.T1IE = 1; // Enable Timer1 interrupt

    time = 0;
    T1CONbits.TON = 1; // Enable Timer1 and start the counter
}

void InitSPI1(void) {
    SPI1STAT = 0x0; // disable the SPI module (just in case)
    SPI1CON1bits.CKE = 1;
    SPI1CON1bits.CKP = 0;
    SPI1CON1bits.SMP = 0; //slave has to be 0
    SPI1CON1bits.MSTEN = 0; //enter slave mode
    SPI1CON1bits.SSEN = 1;
    SPI1CON1bits.MODE16 = 0; //8bit mode
    SPI1STATbits.SPIROV = 0; //Make sure no errors

    IEC0bits.SPI1IE = 1; //enable interrupt
    IPC2bits.SPI1IP = 6; //priority
    SPI1STATbits.SPIEN = 1; //enable the module
    SPI1BUF = 0x45;
}

volatile unsigned char shadowBuff[128];
volatile unsigned char spi_cpy_prt = 0;
volatile unsigned long tempT;
volatile unsigned long encRight;
volatile unsigned long encLeft;
volatile unsigned int shadowBuffPtr = 0;
volatile unsigned char measurementCount = 0;
unsigned char a = 0;

void SampleOnce(void) {
    __delay_us(500);

    encRight++;
    encLeft++;
    loadShadowBuff();


}

void loadShadowBuff() {
    volatile long encR = encRight;
    volatile long encL = encLeft;
    volatile long temptr;
    memcpy((void *) (shadowBuff + shadowBuffPtr), (const void*) laserBuff, 4);
    shadowBuffPtr += 4;
    shadowBuff[shadowBuffPtr++] = (encR >> 24)&0xff;
    shadowBuff[shadowBuffPtr++] = (encR >> 16)&0xff;
    shadowBuff[shadowBuffPtr++] = (encR >> 8)&0xff;
    shadowBuff[shadowBuffPtr++] = encR & 0xff;
    shadowBuff[shadowBuffPtr++] = (encL >> 24)&0xff;
    shadowBuff[shadowBuffPtr++] = (encL >> 16)&0xff;
    shadowBuff[shadowBuffPtr++] = (encL >> 8)&0xff;
    shadowBuff[shadowBuffPtr++] = encL & 0xff;

    tempT = time; //get interrupt time
    shadowBuff[shadowBuffPtr++] = tempT >> 16;
    shadowBuff[shadowBuffPtr++] = tempT >> 8;
    shadowBuff[shadowBuffPtr++] = tempT;

    measurementCount++;

    if (measurementCount == 7) {

        memcpy((void *) (shadowBuff + shadowBuffPtr), (const void*) IMUBuff, 18);
        shadowBuffPtr += 18;
        measurementCount = 0;

    }
    if (shadowBuffPtr == 123) {//shadow buffer is full, transfer to primary buffer
        // LATBbits.LATB6=1;
        //

        // while(bufferPtr!=0);
        shadowBuff[125] = a++;
        shadowBuff[126] = spi_cpy_prt;
        shadowBuff[127] = spi_trans_ptr;

        temptr = spi_cpy_prt << 7;
        memcpy((void *) (measurements + temptr), (const void*) shadowBuff, 128);
        spi_cpy_prt++;
        spi_cpy_prt = spi_cpy_prt & 0xF;
        LATBbits.LATB15 = 1; //Tell BBB that the data is ready
        shadowBuffPtr = 0; //wrap around


        //while(bufferPtr<=126);
        //LATBbits.LATB6=0;
        LATBbits.LATB15 = 0;
    }

}

void enter_sampling_state() {//start timer and do reduncent reset
    encRight = 0;
    encLeft = 0;
    spi_cpy_prt = 0;
    shadowBuffPtr = 0;
    temp_ptr = 0; //block * 128
    spi_trans_ptr = 0;
    bufferPtr = 0; //ptr inside a block
    measurementCount = 0;
    InitTimer(); //reinit timer clears timer
}

void clear_state() {
    spi_cpy_prt = 0;
    shadowBuffPtr = 0;
    measurementCount = 0;
    temp_ptr = 0; //block * 128
    bufferPtr = 0; //ptr inside a block
    spi_trans_ptr = 0;
}