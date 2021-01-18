/*
 * motor.h
 *
 *  Created on: 16. 1. 2021
 *      Author: bodor
 */

/**
 * @file    motor.h
 * @brief   Hlavičkový súbor s funkciami pre motor.c
 *
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <definitions.h>

extern volatile uint8_t message[];
extern uint8_t direction, lastKnownLimitSwitch;
extern bool isDoorClose, isMoving;

void stopAndGoCabineProcedure();
void stopCabineProcedure();
void checkForStop(uint8_t floorForStop);
void motorUp();
void motorDown();
void stopMotor();

#endif /* MOTOR_H_ */
