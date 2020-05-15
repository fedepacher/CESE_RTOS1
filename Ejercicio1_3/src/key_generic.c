/*
 * key_generic.c
 *
 *  Created on: Apr 1, 2020
 *      Author: fedepacher
 */

/*=====[Inclusion de su propia cabecera]=====================================*/

#include "key_generic.h"

/*=====[Inclusiones de dependencias de funciones privadas]===================*/

/*=====[Macros de definicion de constantes privadas]=========================*/

/*=====[Macros estilo funcion privadas]======================================*/

/*=====[Definiciones de tipos de datos privados]=============================*/

// Tipo de datos que renombra un tipo basico

// Tipo de datos de puntero a funcion

// Tipo de datos enumerado

// Tipo de datos estructua, union o campo de bits

/*=====[Definiciones de Variables globales publicas externas]================*/
fsmButtonState_t fsmButtonState;

TickType_t time_down;
TickType_t time_up;
TickType_t time_diff;
/*=====[Definiciones de Variables globales publicas]=========================*/

/*=====[Definiciones de Variables globales privadas]=========================*/

/*=====[Prototipos de funciones privadas]====================================*/

/*=====[Implementaciones de funciones publicas]==============================*/

TickType_t get_diff(void){
	return time_diff;
}

void clear_diff(void){
	time_diff = 0;
}

void buttonPressed(void){
	time_down = xTaskGetTickCount();
}

void buttonReleased(void){
	time_up = xTaskGetTickCount();
	time_diff = time_up - time_down;
}

void fsmButtonInit(void){
	fsmButtonState = STATE_BUTTON_UP;
}

void fsmButtonError(void){
	fsmButtonInit();
}

void fsmButtonUpdate(gpioMap_t button) {

	static uint8_t countFalling = 0;
	static uint8_t countRising = 0;

	switch (fsmButtonState) {
	case STATE_BUTTON_UP:

		if (!gpioRead(button)) {
			fsmButtonState = STATE_BUTTON_FALLING;
		}

		break;
	case STATE_BUTTON_FALLING:
		if (countFalling >= DEBOUNCE_TIME) {
			if (!gpioRead(button)) {
				fsmButtonState = STATE_BUTTON_DOWN;
				buttonPressed();

			} else{
				fsmButtonState = STATE_BUTTON_UP;
			}
			countFalling = 0;
		}
		countFalling++;	//1 cycle = 1ms
		break;
	case STATE_BUTTON_DOWN:
		if (gpioRead(button)) {
			fsmButtonState = STATE_BUTTON_RISING;
		}
		break;
	case STATE_BUTTON_RISING:
		if (countRising >= DEBOUNCE_TIME) {
			if (gpioRead(button)) {
				fsmButtonState = STATE_BUTTON_UP;
				buttonReleased();
				} else{
					fsmButtonState = STATE_BUTTON_DOWN;
				}
			countRising = 0;
		}
		countRising++;
		break;
	default:
		fsmButtonError();
		break;
	}

}


/*=====[Implementaciones de funciones de interrupcion publicas]==============*/

