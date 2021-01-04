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
#define myAddress 0x00
#define motorAddress 0xf1
#define doorAddress 0xf0
#define displayAddress 0x30
#define dataMessage 0xa0
#define ackMessage 0xa1

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint8_t message[20], index = 0, led = 0x10, stopLimitSwitch = 0xe0,
		lastKnownLimitSwitch, currentFloor, direction = 0x02;
volatile bool messageIsComplete = false, isMoving = false;

/*******************************************************************************
 * Code
 ******************************************************************************/

void DEMO_LPSCI_IRQHandler(void) {
	if ((kLPSCI_RxDataRegFullFlag) & LPSCI_GetStatusFlags(DEMO_LPSCI)) {
		message[index] = LPSCI_ReadByte(DEMO_LPSCI);

		if (index == (message[3] + 4)) {
			messageIsComplete = true;
		}
		index++;
	}
}

unsigned char crc8(const unsigned char *data, const unsigned int size) {
	unsigned char crc = 0;
	for (unsigned int i = 0; i < size; ++i) {
		unsigned char inbyte = data[i];
		for (unsigned char j = 0; j < 8; ++j) {
			unsigned char mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix)
				crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}

void delay(int millis) {
	for (long i = 0; i < millis * 10000; i++) __asm("nop");
}

void sendAck(uint8_t reciverAddress) {
	uint8_t crcAck[] = { reciverAddress, myAddress };
	uint8_t ackPacket[] = { 0xA1, reciverAddress, 0x00, 0x00, crc8(crcAck,
			sizeof(crcAck)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, ackPacket, sizeof(ackPacket));
}

void closeDoor(void) {
	uint8_t crcAck[] = { doorAddress, myAddress, 0x01 };
	uint8_t msg[] = { dataMessage, doorAddress, myAddress, 0x01, 0x01, crc8(
			crcAck, sizeof(crcAck)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void openDoor() {
	uint8_t crcAck[] = { doorAddress, myAddress, 0x00 };
	uint8_t msg[] = { dataMessage, doorAddress, myAddress, 0x01, 0x00, crc8(
			crcAck, sizeof(crcAck)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void motorUp(void) {
	uint8_t crcAck[] = { motorAddress, myAddress, 0x02, 0x64, 0x00, 0x00, 0x00 };
	uint8_t move_up[] = { dataMessage, motorAddress, myAddress, 0x05, 0x02,
			0x64, 0x00, 0x00, 0x00, crc8(crcAck, sizeof(crcAck)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, move_up, sizeof(move_up));
}

void motorDown() {
	uint8_t crcAck[] = { motorAddress, myAddress, 0x02, 0x9C, 0xFF, 0xFF, 0xFF };
	uint8_t move_down[] = { dataMessage, motorAddress, myAddress, 0x05, 0x02,
			0x9C, 0xFF, 0xFF, 0xFF, crc8(crcAck, sizeof(crcAck)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, move_down, sizeof(move_down));
}

void stopMotor() {
	uint8_t crcAck[] = { motorAddress, myAddress, 0x01 };
	uint8_t msg[] = { dataMessage, motorAddress, myAddress, 0x01, 0x01, crc8(
			crcAck, sizeof(crcAck)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void ledOnOff(uint8_t led, uint8_t status) {
	uint8_t crcAck[] = { led, 0x00, status };
	uint8_t msg[] = { dataMessage, led, myAddress, 0x01, status, crc8(crcAck,
			sizeof(crcAck)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void sendFloorToDisplay() {
	uint8_t crc[] = { displayAddress, myAddress, direction, currentFloor };
	uint8_t msg[] = { dataMessage, displayAddress, myAddress, 0x02, direction,
			currentFloor, crc8(crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void processMessage() {
	if (isMoving == true) {
		if (message[2] == stopLimitSwitch) {
			stopMotor();
			direction = 0x00;
			delay(500);
			openDoor();
			ledOnOff(led, OFF);
			delay(5);
			sendFloorToDisplay();
			isMoving = false;
		}

		if (message[2] >= 0xe0 && message[2] <= 0xe4) {
			lastKnownLimitSwitch = message[2];
			currentFloor = message[2] - 0xB0;
			sendFloorToDisplay();
		}

		if ((message[2] >= 0xc0 && message[2] <= 0xc4)
				|| (message[2] >= 0xb0 && message[2] <= 0xb4)) {
			getInfoFromButtons(message[2]);
			ledOnOff(led, ON);
			//spracovanie poradia//
		}
	} else {
		if ((message[2] >= 0xc0 && message[2] <= 0xc4)
				|| (message[2] >= 0xb0 && message[2] <= 0xb4)) {
			getInfoFromButtons(message[2]);
			ledOnOff(led, ON);
			delay(5);
			closeDoor();
			delay(340);
			if (lastKnownLimitSwitch == stopLimitSwitch)
				return;
			if (lastKnownLimitSwitch > stopLimitSwitch) {
				motorDown();
				direction = 0x02;
				delay(5);
				sendFloorToDisplay();
				isMoving = true;
			} else {
				motorUp();
				direction = 0x01;
				delay(5);
				sendFloorToDisplay();
				isMoving = true;
			}

		}
	}

}

void getInfoFromButtons(uint8_t button) {
	switch (button) {
	case 0xc4:
		led = 0x14;
		stopLimitSwitch = 0xe4;
		break;
	case 0xb4:
		led = 0x24;
		stopLimitSwitch = 0xe4;
		break;
	case 0xc3:
		led = 0x13;
		stopLimitSwitch = 0xe3;
		break;
	case 0xb3:
		led = 0x23;
		stopLimitSwitch = 0xe3;
		break;
	case 0xc2:
		led = 0x12;
		stopLimitSwitch = 0xe2;
		break;
	case 0xb2:
		led = 0x22;
		stopLimitSwitch = 0xe2;
		break;
	case 0xc1:
		led = 0x11;
		stopLimitSwitch = 0xe1;
		break;
	case 0xb1:
		led = 0x21;
		stopLimitSwitch = 0xe1;
		break;
	case 0xc0:
		led = 0x10;
		stopLimitSwitch = 0xe0;
		break;
	case 0xb0:
		led = 0x20;
		stopLimitSwitch = 0xe0;
		break;
	}
}

int main(void) {

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

	// Zavola vytah na spodne poschodie 0xe0 - limit switch
	closeDoor();
	delay(120);
	motorDown();
	isMoving = true;

	// Nekonecna slucka na posielanie a prijimanie sprav
	while (true) {
		if (messageIsComplete == true) {
			if (message[0] != ackMessage) {
				sendAck(message[2]);
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
