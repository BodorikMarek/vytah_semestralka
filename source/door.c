/*
 * door.c
 *
 *  Created on: 16. 1. 2021
 *      Author: bodor
 */

/**
 * @file    door.c
 * @brief   Dvere
 *
 *	Funkcie na otváranie a zatváranie dverí na výťahu.
 *
 */
#include <door.h>
/*!
 * Funkcia, ktorá zatvorí dvere výťahu.
 */
void closeDoor() {
	uint8_t crc[] = { doorAddress, myAddress, 0x01 };
	uint8_t msg[] = { dataMessage, doorAddress, myAddress, 0x01, 0x01, crc8(
			crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}
/*!
 * Funkcia, ktorá otvorí dvere výťahu.
 */
void openDoor() {
	uint8_t crc[] = { doorAddress, myAddress, 0x00 };
	uint8_t msg[] = { dataMessage, doorAddress, myAddress, 0x01, 0x00, crc8(
			crc, sizeof(crc)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, msg, sizeof(msg));
}

