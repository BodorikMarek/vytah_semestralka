/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "board.h"
#include "fsl_lpsci.h"
#include "clock_config.h"
#include "pin_mux.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DEMO_LPSCI UART0
#define DEMO_LPSCI_CLKSRC kCLOCK_CoreSysClk
#define DEMO_LPSCI_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)
#define DEMO_LPSCI_IRQn UART0_IRQn
#define DEMO_LPSCI_IRQHandler UART0_IRQHandler
#define ON 0x01
#define OFF 0x00

/*! @brief Ring buffer size (Unit: Byte). */
#define DEMO_RING_BUFFER_SIZE 16

/*! @brief Ring buffer to save received data. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint8_t message[265], index = 0, led1, led2, limitSwitch = 0xe0, lastKnownLimitSwitch, currentFloor, direction = 0x02;
volatile bool messageIsComplete = false;


uint8_t g_tipString[] =
    "LPSCI functional API interrupt example\r\nBoard receives characters then sends them out\r\nNow please input:\r\n";

/*
  Ring buffer for data input and output, in this example, input data are saved
  to ring buffer in IRQ handler. The main function polls the ring buffer status,
  if there are new data, then send them out.
  Ring buffer full: (((rxIndex + 1) % DEMO_RING_BUFFER_SIZE) == txIndex)
  Ring buffer empty: (rxIndex == txIndex)
*/
uint8_t demoRingBuffer[DEMO_RING_BUFFER_SIZE];
volatile uint16_t txIndex; /* Index of the data to send out. */
volatile uint16_t rxIndex; /* Index of the memory to save new arrived data. */

/*******************************************************************************
 * Code
 ******************************************************************************/

void DEMO_LPSCI_IRQHandler(void)
{
	if ((kLPSCI_RxDataRegFullFlag) & LPSCI_GetStatusFlags(DEMO_LPSCI)) {
	                message[index] = LPSCI_ReadByte(DEMO_LPSCI);

	                if(index == (message[3] + 4)){
	                	messageIsComplete = true;
	                }
	                index++;
	    }
}

unsigned char crc8(const unsigned char * data, const unsigned int size)
{
    unsigned char crc = 0;
    for ( unsigned int i = 0; i < size; ++i )
    {
        unsigned char inbyte = data[i];
        for ( unsigned char j = 0; j < 8; ++j )
        {
            unsigned char mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if ( mix ) crc ^= 0x8C;
            inbyte >>= 1;
        }
    }
    return crc;
}

void delay(int millis) {
	for(long i = 0; i < millis * 10000; i++) __asm("nop");
}


void sendAck() {
	uint8_t crcAck[] = {message[2], 0x00};
	uint8_t ackPacket[] = {0xA1, message[2], 0x00, 0x00, crc8(crcAck, sizeof(crcAck))};
	LPSCI_WriteBlocking(DEMO_LPSCI, ackPacket, sizeof(ackPacket));
}

void closeDoor(void) {
	uint8_t msg[] = {0xA0, 0xf0, 0x00, 0x01, 0x01, 0xd3};
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void openDoor() {
	uint8_t msg[] = {0xA0, 0xF0, 0x00, 0x01, 0x00, 0x8D};
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void motorUp(void) {
	uint8_t move_up[] = {0xA0, 0xf1 , 0x00,0x05, 0x02, 0x64, 0x00, 0x00, 0x00, 0x64};
	LPSCI_WriteBlocking(DEMO_LPSCI, move_up, sizeof(move_up));
}


void motorDown() {
	uint8_t move_down[] = {0xA0, 0xf1 , 0x00, 0x05, 0x02, 0x9C, 0xFF, 0xFF, 0xFF, 0x6F};
	LPSCI_WriteBlocking(DEMO_LPSCI, move_down, sizeof(move_down));
}

void stopMotor() {
	uint8_t msg[] = {0xa0, 0xf1, 0x00, 0x01, 0x01, 0x78};
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void ledOnOff(uint8_t led,uint8_t status){
	uint8_t crc[] = {led, 0x00, status};
	uint8_t msg[] = {0xA0, led, 0x00, 0x01, status, crc8(crc, sizeof(crc))};
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void sendFloorToDisplay() {
	uint8_t crc[] = {0x30, 0x00, direction, currentFloor};
	uint8_t msg[] = {0xA0, 0x30, 0x00, 0x02, direction, currentFloor, crc8(crc, sizeof(crc))};
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void processMessage() {
	if ((message[2] >= 0xc0 && message[2] <= 0xc4) || (message[2] >= 0xb0 && message[2] <= 0xb4)) {
		getInfoFromButtons();
		ledOnOff(led1, ON);
		delay(5);
		ledOnOff(led2, ON);
		delay(5);
		closeDoor();
		delay(340);
		if (lastKnownLimitSwitch == limitSwitch) return;
		if(lastKnownLimitSwitch > limitSwitch){
			motorDown();
			direction = 0x02;
			delay(5);
			sendFloorToDisplay();
		} else {
			motorUp();
			direction = 0x01;
			delay(5);
			sendFloorToDisplay();

		}

	}

	if (message[2] >= 0xe0 && message[2] <= 0xe4) {
			lastKnownLimitSwitch = message[2];
			currentFloor = message[2] - 0xB0;
			sendFloorToDisplay();
	}

	if(message[2] == limitSwitch){
		stopMotor();
		direction = 0x00;
		delay(500);
		openDoor();
		ledOnOff(led1, OFF);
		delay(5);
		ledOnOff(led2, OFF);
		delay(5);
		sendFloorToDisplay();
	}
}

void getInfoFromButtons() {
	if(message[2] == 0xc4 || message[2] == 0xb4){
		led1 = 0x14;
		led2 = 0x24;
		limitSwitch = 0xe4;
	}else if(message[2] == 0xc3 || message[2] == 0xb3){
		led1 = 0x13;
		led2 = 0x23;
		limitSwitch = 0xe3;
	}else if(message[2] == 0xc2 || message[2] == 0xb2){
		led1 = 0x12;
		led2 = 0x22;
		limitSwitch = 0xe2;
	}else if(message[2] == 0xc1 || message[2] == 0xb1){
		led1 = 0x11;
		led2 = 0x21;
		limitSwitch = 0xe1;
	}else if(message[2] == 0xc0 || message[2] == 0xb0){
		led1 = 0x10;
		led2 = 0x20;
		limitSwitch = 0xe0;
	}


}



int main(void)
{

    lpsci_config_t config;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    CLOCK_SetLpsci0Clock(0x1U);

    LPSCI_GetDefaultConfig(&config);
    config.baudRate_Bps = BOARD_DEBUG_UART_BAUDRATE;
    config.enableTx = true;
    config.enableRx = true;

    LPSCI_Init(DEMO_LPSCI, &config, DEMO_LPSCI_CLK_FREQ);


    /* Enable RX interrupt. */
    LPSCI_EnableInterrupts(DEMO_LPSCI, kLPSCI_RxDataRegFullInterruptEnable);
    EnableIRQ(DEMO_LPSCI_IRQn);
    closeDoor();
    delay(120);
    motorDown();
    while(true) {
    	if(messageIsComplete == true){
    		if(message[0] != 0xA1) {
    				sendAck();
    				delay(25);
    				processMessage();
    				index = 0;
    				messageIsComplete = false;

    		}
    		index = 0;
    		messageIsComplete = false;

    	}

    }


}
