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
	if(time_up >= time_down)
		time_diff = time_up - time_down;
	else
		time_diff = time_down - time_up ;

	BaseType_t res =
			xTaskCreate(ledTask,                 // Funcion de la tarea a ejecutar
				(const char *) "ledTask", // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
				&time_diff,                          // Parametros de tarea
				tskIDLE_PRIORITY + 1,         // Prioridad de la tarea
				0                         // Puntero a la tarea creada en el sistema
				);

		if (res == pdFAIL) {

		}


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

