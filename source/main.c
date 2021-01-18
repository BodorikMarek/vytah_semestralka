/**
 * @file    main.c
 * @brief   Hlavný spúšťací súbor
 *
 * Hlavná trieda v ktorej sa nachádza handler na prerušenia, spracovanie správy,
 * crc súčet, funkcia pre delay, odosielanie ack správy
 *
 */
#include "board.h"
#include <motor.h>
#include <door.h>
#include <leds.h>
#include <definitions.h>
#include <variables.h>

/*!
 * Funkcia pre správu prerušení, každým vyvolaným prerušením sa príjme 1B správy, v prípade,
 * že správa je kompletná, globálna premenná messageIsComplete sa nastavý na true a ďalej sa
 * spracuváva.
 */
void DEMO_LPSCI_IRQHandler(void) {
	if ((kLPSCI_RxDataRegFullFlag) & LPSCI_GetStatusFlags(DEMO_LPSCI)) {
		message[index] = LPSCI_ReadByte(DEMO_LPSCI);

		if (index == (message[3] + 4)) {
			messageIsComplete = true;
		}
		index++;
	}
}

/*!
 * Funkcia pre výpočet crc hodnot.
 *
 * Zdroj: https://stackoverflow.com/questions/51752284/how-to-calculate-crc8-in-c
 *
 * Vstupný parameter *data predstavuje dáta z ktorých sa crc bude počítať a size
 * ich veľkosť.
 */
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


/*!
 * Funkcia vyvarajúca oneskorenie.
 */
void delay(int millis) {
	for (long i = 0; i < millis * 10000; i++)
		__asm("nop");
}

/*!
 * Funkcia, ktorá posiela ack správy prijaté od výťahu.
 */
void sendAck(uint8_t reciverAddress) {
	uint8_t crcAck[] = { reciverAddress, myAddress };
	uint8_t ackPacket[] = { 0xA1, reciverAddress, 0x00, 0x00, crc8(crcAck,
			sizeof(crcAck)) };
	LPSCI_WriteBlocking(DEMO_LPSCI, ackPacket, sizeof(ackPacket));
}

/*!
 * Funkcia, ktora spracuváva správu od výťahu, je to hlavná funkcia obsluhujúca výťah.
 */
void processMessage() {
	if (isMoving == true) {
		if (message[2] == destinationSwitch) {
			stopCabineProcedure();
		}

		if (message[2] >= 0xe0 && message[2] <= 0xe4) {
			lastKnownLimitSwitch = message[2];
			currentFloor = message[2] - 0xB0;
			sendFloorToDisplay();
			checkForStop(message[2]);
		}

		if ((message[2] >= 0xc0 && message[2] <= 0xc4)
				|| (message[2] >= 0xb0 && message[2] <= 0xb4)) {
			getInfoFromButtons(message[2]);
			ledOnOff(led, ON);
			minFloorFind();
		}
	} else {
		if ((message[2] >= 0xc0 && message[2] <= 0xc4)
				|| (message[2] >= 0xb0 && message[2] <= 0xb4)) {
			getInfoFromButtons(message[2]);
			minFloorFind();
			if (lastKnownLimitSwitch == destinationSwitch)
				return;

			ledOnOff(led, ON);
			delay(5);

			if (isDoorClose == false) {
				closeDoor();
				delay(340);
			}
			if (lastKnownLimitSwitch > destinationSwitch) {
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


	closeDoor();
	delay(120);
	motorDown();
	isMoving = true;
	isDoorClose = true;

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
