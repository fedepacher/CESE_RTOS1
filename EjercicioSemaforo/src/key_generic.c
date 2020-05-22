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


/*=====[Definiciones de Variables globales publicas]=========================*/

/*=====[Definiciones de Variables globales privadas]=========================*/

/*=====[Prototipos de funciones privadas]====================================*/

/*=====[Implementaciones de funciones publicas]==============================*/


void buttonPressed(button_led_t* config){
	config->time_down = xTaskGetTickCount();
	config->pressed = TRUE;
}

void buttonReleased(button_led_t* config){
	config->time_up = xTaskGetTickCount();
	if(config->time_up >= config->time_down)
		config->messure_time = config->time_up - config->time_down;
	else
		config->messure_time = config->time_down - config->time_up ;
	config->released = TRUE;
}

void fsmButtonInit(button_led_t* config){
	config->fsmButtonState = STATE_BUTTON_UP;
	config->countFalling = 0;
	config->countRising = 0;
	config->pressed = FALSE;
	config->released = FALSE;
}

void fsmButtonError(button_led_t* config){
	config->fsmButtonState = STATE_BUTTON_UP;
}

void fsmButtonUpdate(button_led_t* config) {

	switch (config->fsmButtonState) {
	case STATE_BUTTON_UP:
		config->pressed = FALSE;
		config->released = FALSE;
		if (!gpioRead(config->button)) {
			config->fsmButtonState = STATE_BUTTON_FALLING;
		}

		break;
	case STATE_BUTTON_FALLING:
		if (config->countFalling >= DEBOUNCE_TIME) {
			if (!gpioRead(config->button)) {
				config->fsmButtonState = STATE_BUTTON_DOWN;
				buttonPressed(config);

			} else{
				config->fsmButtonState = STATE_BUTTON_UP;
			}
			config->countFalling = 0;
		}
		config->countFalling++;	//1 cycle = 1ms
		break;
	case STATE_BUTTON_DOWN:
		if (gpioRead(config->button)) {
			config->fsmButtonState = STATE_BUTTON_RISING;
		}
		break;
	case STATE_BUTTON_RISING:
		if (config->countRising >= DEBOUNCE_TIME) {
			if (gpioRead(config->button)) {
				config->fsmButtonState = STATE_BUTTON_UP;
				buttonReleased(config);
				} else{
					config->fsmButtonState = STATE_BUTTON_DOWN;
				}
			config->countRising = 0;
		}
		config->countRising++;
		break;
	default:
		fsmButtonError(config);
		break;
	}

}


/*=====[Implementaciones de funciones de interrupcion publicas]==============*/

