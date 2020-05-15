/*
 * key_generic.h
 *
 *  Created on: Apr 1, 2020
 *      Author: fedepacher
 */

#ifndef FEDE_CLASE5PM_INC_KEY_GENERIC_H_
#define FEDE_CLASE5PM_INC_KEY_GENERIC_H_

#include "sapi.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"

/*=====[C++ comienzo]========================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*=====[Macros de definicion de constantes publicas]=========================*/

//#define PI 3.14

/*=====[Macros estilo funcion publicas]======================================*/

#define DEBOUNCE_TIME	40		/*tiempo del antirebote*/

/*=====[Definiciones de tipos de datos publicos]=============================*/

// Tipo de datos que renombra un tipo basico


// Tipo de datos de puntero a funcion


// Tipo de datos enumerado
typedef enum {
	STATE_BUTTON_UP, STATE_BUTTON_FALLING, STATE_BUTTON_DOWN, STATE_BUTTON_RISING,		/*estados de la MEF*/
} fsmButtonState_t;



// Tipo de datos estructua, union o campo de bits

/*=====[Prototipos de funciones publicas]====================================*/


void fsmButtonError(void);
void fsmButtonInit(void);
void fsmButtonUpdate(gpioMap_t button);
void buttonPressed(void);
void buttonReleased(void);
TickType_t get_diff(void);
void clear_diff(void);


/*=====[Prototipos de funciones publicas de interrupcion]====================*/



/*=====[C++ fin]=============================================================*/

#ifdef __cplusplus
}
#endif



#endif /* FEDE_CLASE5PM_INC_KEY_GENERIC_H_ */
