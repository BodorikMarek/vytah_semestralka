/*
 * definitions.h
 *
 *  Created on: 16. 1. 2021
 *      Author: bodor
 */

#include <fsl_lpsci.h>
#include <pin_mux.h>
#include <stdbool.h>

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#define DEMO_LPSCI UART0
#define DEMO_LPSCI_CLKSRC kCLOCK_CoreSysClk
#define DEMO_LPSCI_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define DEMO_LPSCI_IRQn UART0_IRQn
#define DEMO_LPSCI_IRQHandler UART0_IRQHandler

#define ON 0x01
#define OFF 0x00
#define myAddress 0x00
#define motorAddress 0xf1
#define doorAddress 0xf0
#define displayAddress 0x30
#define dataMessage 0xa0
#define ackMessage 0xa1
#define led0 0x10
#define led1 0x11
#define led2 0x12
#define led3 0x13
#define led4 0x14
#define ledCab0 0x20
#define ledCab1 0x21
#define ledCab2 0x22
#define ledCab3 0x23
#define ledCab4 0x24

#endif /* DEFINITIONS_H_ */
