/*
 * leds.c
 *
 *  Created on: 16. 1. 2021
 *      Author: bodor
 */

#include <leds.h>


void ledOnOff(uint8_t led, uint8_t status) {
	uint8_t crc[] = { led, 0x00, status };
	uint8_t msg[] = { dataMessage, led, myAddress, 0x01, status, crc8(crc,
			sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

void minFloorFind() {
	if (direction == 0x02) {
		if (destinationSwitchTmp > destinationSwitch) {
			return;
		} else {
			destinationSwitch = destinationSwitchTmp;
		}
	}

	if (direction == 0x01) {
		if (destinationSwitchTmp < destinationSwitch) {
			return;
		} else {
			destinationSwitch = destinationSwitchTmp;
		}
	}
	if (direction == 0x00)
		destinationSwitch = destinationSwitchTmp;
}

void floorsAndLedsFalse(uint8_t floor) {
	if (floor == 0xe4) {
		floors[4] = false;
		delay(5);
		ledOnOff(led4, OFF);
		delay(15);
		ledOnOff(ledCab4, OFF);
	}

	if (floor == 0xe3) {
		floors[3] = false;
		delay(5);
		ledOnOff(led3, OFF);
		delay(15);
		ledOnOff(ledCab3, OFF);
	}

	if (floor == 0xe2) {
		floors[2] = false;
		delay(5);
		ledOnOff(led2, OFF);
		delay(15);
		ledOnOff(ledCab2, OFF);
	}

	if (floor == 0xe1) {
		floors[1] = false;
		delay(5);
		ledOnOff(led1, OFF);
		delay(15);
		ledOnOff(ledCab1, OFF);
	}

	if (floor == 0xe0) {
		floors[0] = false;
		delay(5);
		ledOnOff(led0, OFF);
		delay(15);
		ledOnOff(ledCab0, OFF);
	}

}

void getInfoFromButtons(uint8_t button) {
	switch (button) {
	case 0xc4:
		led = 0x14;
		floors[4] = true;
		destinationSwitchTmp = 0xe4;
		break;
	case 0xb4:
		led = 0x24;
		floors[4] = true;
		destinationSwitchTmp = 0xe4;
		break;
	case 0xc3:
		led = 0x13;
		floors[3] = true;
		destinationSwitchTmp = 0xe3;
		break;
	case 0xb3:
		led = 0x23;
		floors[3] = true;
		destinationSwitchTmp = 0xe3;
		break;
	case 0xc2:
		led = 0x12;
		floors[2] = true;
		destinationSwitchTmp = 0xe2;
		break;
	case 0xb2:
		led = 0x22;
		floors[2] = true;
		destinationSwitchTmp = 0xe2;
		break;
	case 0xc1:
		led = 0x11;
		floors[1] = true;
		destinationSwitchTmp = 0xe1;
		break;
	case 0xb1:
		led = 0x21;
		floors[1] = true;
		destinationSwitchTmp = 0xe1;
		break;
	case 0xc0:
		led = 0x10;
		floors[0] = true;
		destinationSwitchTmp = 0xe0;
		break;
	case 0xb0:
		led = 0x20;
		floors[0] = true;
		destinationSwitchTmp = 0xe0;
		break;
	}

}

void sendFloorToDisplay() {
	uint8_t crc[] = { displayAddress, myAddress, direction, currentFloor };
	uint8_t msg[] = { dataMessage, displayAddress, myAddress, 0x02, direction,
			currentFloor, crc8(crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}
