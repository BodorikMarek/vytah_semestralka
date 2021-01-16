/*
 * leds.h
 *
 *  Created on: 16. 1. 2021
 *      Author: bodor
 */


#ifndef LEDS_H_
#define LEDS_H_

#include <definitions.h>

extern uint8_t direction, destinationSwitch, destinationSwitchTmp, led, currentFloor;
extern bool floors[];

void sendFloorToDisplay();
void ledOnOff(uint8_t led, uint8_t status);
void minFloorFind();
void floorsAndLedsFalse(uint8_t floor);
void getInfoFromButtons(uint8_t button);
#endif /* LEDS_H_ */
