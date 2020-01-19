/*
 * task3.c
 *
 *  Created on: 7 Dec 2019
 *      Author: Jussi M Savola
 */



/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_types.h"

/* LUT includes. */
#include "rtosharkka.h"

/* Converter model. This model is a simple state space model given to you in eq. 3. */

extern  void converter() {
	float test = 11111;
	float ERR = -1;
	const TickType_t freq = pdMS_TO_TICKS( 100 );
	TickType_t wakeTime;



	wakeTime = xTaskGetTickCount();
	for( ;; ) {

		xSemaphoreTake(LEDsem,portMAX_DELAY);
		// AXI_LED_DATA =AXI_LED_DATA ^ 0x04;
		xSemaphoreGive(LEDsem);
		while (test == ERR) {
		    test = get_Kp();
		}
		//xil_printf( "testi variaabeli %d\r\n", test );

		vTaskDelayUntil( &wakeTime, freq );
	}
}
