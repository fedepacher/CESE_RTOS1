/* Copyright 2017-2018, Eric Pernia
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*==================[inlcusiones]============================================*/

// Includes de FreeRTOS
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

// sAPI header
#include "sapi.h"
#include "key_generic.h"

/*==================[definiciones y macros]==================================*/
#define T_1ms	1
/*==================[definiciones de datos internos]=========================*/
gpioMap_t buttons_array[] = { TEC1, TEC2, TEC3, TEC4 }; //arreglo de teclas
gpioMap_t leds_array[] = { LEDB, LED1, LED2, LED3 };	//arreglo de leds
button_led_t button_config[sizeof(buttons_array)];
xSemaphoreHandle xSemaphore1;
/*==================[definiciones de datos externos]=========================*/

DEBUG_PRINT_ENABLE
;

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

// Prototipo de funcion de la tarea
void keyTask(void* taskParmPtr);
void otraTask(void* taskParmPtr);

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main(void) {
	// ---------- CONFIGURACIONES ------------------------------
	// Inicializar y configurar la plataforma
	boardConfig();

	// UART for debug messages
	debugPrintConfigUart( UART_USB, 115200 );
	//debugPrintlnString("Ejercicio 1_1");


	//inicializacion de arreglo de pulsadores
	int i = 0;
	for (i = 0; i < sizeof(buttons_array); i++) {
		button_config[i].button = buttons_array[i];
	}

	//creo semaforo
	xSemaphore1 = xSemaphoreCreateBinary();
	if (xSemaphore1 != NULL) {

		// Crear tarea en freeRTOS
		BaseType_t res = xTaskCreate(keyTask,  // Funcion de la tarea a ejecutar
				(const char *) "buttons", // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
				button_config,                        // Parametros de tarea
				tskIDLE_PRIORITY + 1,         // Prioridad de la tarea
				0                     // Puntero a la tarea creada en el sistema
				);

		if (res == pdFAIL) {

		}

		//esta tarea tiene una prioridad mayor de manera tal que la tarea keyTask hacer un give del semaphore y ponga en ready la
		//tarea otraTarea el scheduler le ceda el micro al instante, de lo contrario pude no cederle el micro lo que haria cambiar el
		//estado de los flags pressed y released de los pulsadores

		res = xTaskCreate(otraTask,      // Funcion de la tarea a ejecutar
				(const char *) "otraTarea", // Nombre de la tarea como String amigable para el usuario
				configMINIMAL_STACK_SIZE * 2, // Cantidad de stack de la tarea
				0,                          // Parametros de tarea
				tskIDLE_PRIORITY + 2,         // Prioridad de la tarea
				0                     // Puntero a la tarea creada en el sistema
				);

		if (res == pdFAIL) {

		}
	}

	// Iniciar scheduler
	vTaskStartScheduler();

	// ---------- REPETIR POR SIEMPRE --------------------------
	while ( TRUE) {
		// Si cae en este while 1 significa que no pudo iniciar el scheduler
	}

	// NO DEBE LLEGAR NUNCA AQUI, debido a que a este programa se ejecuta
	// directamenteno sobre un microcontroladore y no es llamado por ningun
	// Sistema Operativo, como en el caso de un programa para PC.
	return 0;
}

/*==================[definiciones de funciones internas]=====================*/

/*==================[definiciones de funciones externas]=====================*/

// Implementacion de funcion de la tarea
void keyTask(void* taskParmPtr) {
	// ---------- CONFIGURACIONES ------------------------------

	button_led_t* config = (button_led_t*) taskParmPtr;
	int i = 0;
//inicializacion de los pulsadores
	for (i = 0; i < sizeof(buttons_array); i++)
		fsmButtonInit(&config[i]);

	// ---------- REPETIR POR SIEMPRE --------------------------
	while (TRUE) {
		for (i = 0; i < sizeof(buttons_array); i++)
			fsmButtonUpdate(&config[i]);
		vTaskDelay( T_1ms / portTICK_RATE_MS);//esto tiene que estar si o si para que el debounce funcione porque llama a la funcion debounce cara 1 ms


		if(config[0].released == TRUE)
			gpioToggle(leds_array[0]);	//accion TEC1
		if(config[1].released == TRUE)
			gpioToggle(leds_array[1]);	//accion TEC2
		if(config[2].released == TRUE)
			xSemaphoreGive(xSemaphore1);	//accion TEC3
	}
	vTaskDelete(NULL);
}

void otraTask(void* taskParmPtr) {
	// ---------- CONFIGURACIONES ------------------------------
	// ---------- REPETIR POR SIEMPRE --------------------------
//al tener prioridad mas alta esta tarea se ejecuta primero y como hace un take del semaforo por tiempo indefinido (portMAX_DELAY)
	// pasa inmediatamente a esta block hasta qeu se haga un give del semaforo

	while (TRUE) {
		xSemaphoreTake(xSemaphore1, portMAX_DELAY);
		gpioToggle(leds_array[2]);
		vTaskDelay(T_1ms / portTICK_RATE_MS);
	}

	vTaskDelete(NULL);
}

/*==================[fin del archivo]========================================*/
