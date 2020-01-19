

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

extern  void ledita_4() {
	TickType_t wakeTime;



	wakeTime = xTaskGetTickCount();
	for( ;; ) {

		// Semaphore is just as an example here

		xSemaphoreTake(LEDsem,portMAX_DELAY);
		AXI_LED_DATA =AXI_LED_DATA ^ 0x08;
		xSemaphoreGive(LEDsem);

		uint8_t input = AXI_LED_DATA & 0xF;  // Range: 0..15
		uint16_t delay = 1000 - 60*input; // Range: 100-1000ms

		TickType_t freq = pdMS_TO_TICKS( delay );

		vTaskDelayUntil( &wakeTime, freq );
		//vTaskDelayUntil( &wakeTime, 200 );
	}
}
