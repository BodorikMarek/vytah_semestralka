/*
 * motor.c
 *
 *  Created on: 16. 1. 2021
 *      Author: bodor
 */
/**
 * @file    motor.c
 * @brief   Obsluha motorov
 *
 *	Trieda motor.c je zodpovedná za správu motoru jeho zastavenie a následne znova zapnutie.
 *
 */


#include <motor.h>

/*!
 * Funkcia, ktorá v prípade, že je výťah zavolaný na viacero poschodí, tak zastaví a pohne sa v rovnakom smere
 * v akom výťah prišiel na dané poschodie.
 */
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


/*!
 * Funkcia, ktorá zastaví výťah na poslednom poschodí.
 */
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

/*!
 * Funkcia, kontrolujúca či výťah má zastaviť na danom poschodí.
 */
void checkForStop(uint8_t floorForStop) {
	switch (floorForStop) {
	case 0xe3:
		if (floors[3] == true) {
			stopAndGoCabineProcedure();
		}
		break;

	case 0xe2:
		if (floors[2] == true) {
			stopAndGoCabineProcedure();
		}
		break;

	case 0xe1:
		if (floors[1] == true) {
			stopAndGoCabineProcedure();
		}

		break;
	}
}

/*!
 * Funkcia, ktorá pošle výťah s rýchlosťou 100 smerom hore.
 */
void motorUp(void) {
	uint8_t crc[] = { motorAddress, myAddress, 0x02, 0x64, 0x00, 0x00, 0x00 };
	uint8_t move_up[] = { dataMessage, motorAddress, myAddress, 0x05, 0x02,
			0x64, 0x00, 0x00, 0x00, crc8(crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, move_up, sizeof(move_up));
}

/*!
 * Funkcia, ktorá pošle výťah s rýchlosťou 100 smerom dole.
 */
void motorDown() {
	uint8_t crc[] = { motorAddress, myAddress, 0x02, 0x9C, 0xFF, 0xFF, 0xFF };
	uint8_t move_down[] = { dataMessage, motorAddress, myAddress, 0x05, 0x02,
			0x9C, 0xFF, 0xFF, 0xFF, crc8(crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, move_down, sizeof(move_down));
}

/*!
 * Funkcia, ktorá zastaví výťah.
 */
void stopMotor() {
	uint8_t crc[] = { motorAddress, myAddress, 0x01 };
	uint8_t msg[] = { dataMessage, motorAddress, myAddress, 0x01, 0x01, crc8(
			crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}
