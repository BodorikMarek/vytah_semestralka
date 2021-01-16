/*
 * motor.c
 *
 *  Created on: 16. 1. 2021
 *      Author: bodor
 */

/**
 * @file    motor.c
 * @brief   Trieda spracuvava pohyb kabiny vytahu
 * @details Subor obsahuje
 * 			prijatych sprav bez ohladu na ich typ a dalsie funkcie
 * 			potrebne pre spravny chod aplikacie
 */

#include <motor.h>

void stopAndGoCabineProcedure() {
	delay(5);
	stopMotor();
	uint8_t directionTmp = direction;
	direction = 0x00;
	delay(500);
	openDoor();
	floorsAndLedsFalse(lastKnownLimitSwitch);
	isDoorClose = false;
	delay(5);
	sendFloorToDisplay();
	delay(1000);
	closeDoor();
	delay(50);
	direction = directionTmp;

	if (direction == 0x02) {
		motorDown();
		delay(5);
		sendFloorToDisplay();
	} else {
		motorUp();
		delay(5);
		sendFloorToDisplay();
	}
}

void stopCabineProcedure() {
	stopMotor();
	direction = 0x00;
	delay(500);
	floorsAndLedsFalse(message[2]);
	openDoor();
	isDoorClose = false;
	delay(5);
	sendFloorToDisplay();
	isMoving = false;
}

void motorUp(void) {
	uint8_t crc[] = { motorAddress, myAddress, 0x02, 0x64, 0x00, 0x00, 0x00 };
	uint8_t move_up[] = { dataMessage, motorAddress, myAddress, 0x05, 0x02,
			0x64, 0x00, 0x00, 0x00, crc8(crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, move_up, sizeof(move_up));
}

void motorDown() {
	uint8_t crc[] = { motorAddress, myAddress, 0x02, 0x9C, 0xFF, 0xFF, 0xFF };
	uint8_t move_down[] = { dataMessage, motorAddress, myAddress, 0x05, 0x02,
			0x9C, 0xFF, 0xFF, 0xFF, crc8(crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, move_down, sizeof(move_down));
}

void stopMotor() {
	uint8_t crc[] = { motorAddress, myAddress, 0x01 };
	uint8_t msg[] = { dataMessage, motorAddress, myAddress, 0x01, 0x01, crc8(
			crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}
