/* 
 * File:   interrupt.h
 * Author: Tim
 *
 * Created on December 21, 2014, 1:23 PM
 */

#ifndef INTERRUPT_H
#define	INTERRUPT_H

#ifdef	__cplusplus
extern "C" {
#endif

//controls
extern volatile unsigned int samplingFlag;


//buffers and ptrs
extern volatile unsigned long time;//time counter
extern volatile unsigned char laserBuff[4];
extern volatile unsigned char IMUBuff[18];
extern volatile unsigned char measurements[2048];
extern volatile unsigned int bufferPtr;
extern volatile unsigned char spi_trans_ptr;
extern volatile unsigned int temp_ptr;//block * 128
extern volatile unsigned int bufferPtr;//ptr inside a block


#ifdef	__cplusplus
}
#endif

#endif	/* INTERRUPT_H */

