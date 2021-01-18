/*
 * variables.h
 *
 *  Created on: 16. 1. 2021
 *      Author: bodor
 */

/**
 * @file    variables.h
 * @brief   Hlavičkový súbor s premennými.
 *
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

volatile uint8_t message[20], index = 0;
uint8_t led = 0x10, destinationSwitch = 0xe0,
		lastKnownLimitSwitch, currentFloor, direction = 0x02, destinationSwitchTmp;

volatile bool messageIsComplete = false;
bool isMoving = false, isDoorClose = false;
bool floors[5] = { true, false, false, false, false };

#endif /* VARIABLES_H_ */
