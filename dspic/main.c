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


#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"        /* User funct/params, such as InitApp              */
#include "interrupt.h"
#include <libpic30.h>
/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

/* i.e. uint16_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

int16_t main(void) {

    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize IO ports and peripherals */
    InitApp();

    /* TODO <INSERT USER APPLICATION CODE HERE> */
    //  LATBbits.LATB6=1;
    // while(!PORTBbits.RB1);//wait for CTS from BBB
    LATBbits.LATB6 = 0;
    while (1) {
        while (!samplingFlag);
         __delay_ms(100);

        enter_sampling_state();
        while (samplingFlag) {
            LATBbits.LATB6 = 1;            
            SampleOnce();
            LATBbits.LATB6 = 0;
        }

        //check flag to see if it's changed. Then clear state
        __delay_ms(100);
        clear_state();
      

    }
}
