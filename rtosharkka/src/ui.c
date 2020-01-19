
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
#include "stdlib.h"

/* User interface that combines buttons, LEDs and UART communication with Xilinx
console. */
/*
The
first button selects mode between 1. configuration, 2. idling and 3. modulating modes, the second
button in configuration mode selects configurable parameter Kp or Ki, the third button decreases
value: reference output voltage in modulating mode or parameter value in configuration mode. The
fourth button increases values correspondently. In idling mode, the system does not modulate
(converter is off) and in modulating mode controller controls the output voltage of model. The mode
changes are indicated by LEDs and by writing to console. The console can request mode changes and
set parameter values or reference voltage depending on current mode.
*/

/* harkka 4: UART
 * harkka 5: program structures, keskeytykset in general luentomatsku sivu 30
 * harkka 6: Interrupts
 * freertos ref manual pdf kappale 4: semaforit
 */

/* next steps:
 * Jussi: interrupts, näppäimet
 * Tuomo & Juha: semaforien käsittely
 */

/* MODES */

#define INIT 0
#define CONFIG 1
#define CONFIG_KP 2
#define CONFIG_KI 3
#define IDLING 4
#define MODULATING 5
#define SERIALCOMM 6

#define BUTTON1 0
#define BUTTON2 1
#define BUTTON3 2
#define BUTTON4 4

#define STEPSIZE 0.05


/*
LED USAGE:
led zero indicates "init" mode, when nothing is happening. in modulating mode, led zero brightness indicates Uact value.
led one: config mode
led two: idling mode
led three: modulating mode
*/

// Set LED outputs based on character value '1', '2', '3', '4'
void set_leds(uint8_t input) {
	// If input is something outside the range of intended numbers.
	if (input < '0' || '4' < input) return;
	uint8_t mask = 0;
	// In a character table, '0' is the first out of the numbers.
	// Its integer value can be something like 48 (ASCII).
	// By subtracting it from input, we arrive at the actual
	// number it's representing.
	char c = input - '0';
	for (uint8_t i = 0; i < c; i++) {
		mask <<= 1; // Bitwise left shift assignment
		mask |= 0b1; // Set first bit true / led on
	}
	AXI_LED_DATA = mask; // LEDS LD3..0 - AXI LED DATA GPIO register bits [3:0]
}

// Check if UART receive FIFO (first in, first out) is not empty and return the new data
char uart_receive() {
	if ((UART_STATUS & XUARTPS_SR_RXEMPTY) == XUARTPS_SR_RXEMPTY) return 0;
	return UART_FIFO;
}
static char str[] = "\tHello World\r";

// Send one character through UART interface
void uart_send(char c) {
	while (UART_STATUS & XUARTPS_SR_TNFUL);
	UART_FIFO = c;
	while (UART_STATUS & XUARTPS_SR_TACTIVE);
}

// Send string (character array) through UART interface
void uart_send_string(char str[BUFFER_SIZE]) {
	char *ptr = str;
	// While the string still continues.
	while (*ptr != '\0') {
		uart_send(*ptr);
		ptr++;
	}
}

extern  void ui() {
    int mode = INIT;
    int buttonpressed = 0;
    int whichbuttonpressed = 0;
    int ledmask = 0;
	// ms -> tics, only if tick interval is more than ms, give directly as ticks
	const TickType_t delay = pdMS_TO_TICKS( 100 );
	// Necessary forever loop. A thread should never be able to exit!
	for( ;; ) { // Same as while(1) or while(true)
		// AXI_LED_DATA =AXI_LED_DATA ^ 0x01;
		
		whichbuttonpressed = BUTTON1;
		whichbuttonpressed = rand() % 5;;

		
		switch(mode) {
		case INIT: {
		  if (BUTTON1 == whichbuttonpressed) {mode = CONFIG; uart_send_string("Mode: INIT->CONFIG\r\n"); ledmask = 1;}; // OK
		  if (BUTTON2 == whichbuttonpressed) {};
		  if (BUTTON3 == whichbuttonpressed) {};
		  whichbuttonpressed = -1;
		} break;
		case CONFIG: {		  
		  if (BUTTON1 == whichbuttonpressed) {mode = IDLING; uart_send_string("Mode: CONFIG->IDLING\r\n"); ledmask = 2;};  // OK
		  if (BUTTON2 == whichbuttonpressed) {mode = CONFIG_KP; uart_send_string("Mode: CONFIG->CONFIG_KP\r\n"); };
		  if (BUTTON3 == whichbuttonpressed) {};
		  whichbuttonpressed = -1;
		} break;
		case CONFIG_KP: {		  
		  if (BUTTON1 == whichbuttonpressed) {};  // OK
		  if (BUTTON2 == whichbuttonpressed) {mode = CONFIG_KI; uart_send_string("Mode: CONFIG_KP->CONFIG_KI\r\n"); };
		  if (BUTTON3 == whichbuttonpressed) {set_Kp(get_Kp() - STEPSIZE); }; // make Kp smaller
		  if (BUTTON4 == whichbuttonpressed) {set_Kp(get_Kp() + STEPSIZE); }; // make Kp bigger
		  whichbuttonpressed = -1;
		} break;
		case CONFIG_KI: {		  
		  if (BUTTON1 == whichbuttonpressed) {};  // OK
		  if (BUTTON2 == whichbuttonpressed) {mode = CONFIG;  uart_send_string("Mode: CONFIG_KI->CONFIG\r\n"); };
		  if (BUTTON3 == whichbuttonpressed) {set_Ki(get_Ki() - STEPSIZE); }; // make Ki smaller
		  if (BUTTON4 == whichbuttonpressed) {set_Ki(get_Ki() + STEPSIZE); }; // make Ki bigger
		  whichbuttonpressed = -1;
		} break;
		case IDLING: {
		  if (BUTTON1 == whichbuttonpressed) {mode = MODULATING;  uart_send_string("Mode: IDLING->MODULATING\r\n");  ledmask = 3;};  // OK
		  if (BUTTON2 == whichbuttonpressed) {};
		  if (BUTTON3 == whichbuttonpressed) {};
		  whichbuttonpressed = -1;		
		} break;
		case MODULATING: {
		  if (BUTTON1 == whichbuttonpressed) {mode = CONFIG;  xil_printf("Mode: MODULATING->CONFIG\r\n");  ledmask = 1; }; // OK
		  if (BUTTON2 == whichbuttonpressed) {};
		  if (BUTTON3 == whichbuttonpressed) {set_Uref(get_Uref() - 2*STEPSIZE);}; // make Uref smaller
		  if (BUTTON4 == whichbuttonpressed) {set_Uref(get_Uref() + 2*STEPSIZE);}; // make Uref bigger		  
		  whichbuttonpressed = -1;		
		} break;
		case SERIALCOMM: {
		  if (BUTTON1 == whichbuttonpressed) {mode = CONFIG;};
		  if (BUTTON2 == whichbuttonpressed) {};
		  if (BUTTON3 == whichbuttonpressed) {};	
		  whichbuttonpressed = -1;	
		} break;
		}
		
		AXI_LED_DATA = (ledmask & 0xF);

		
		// RTOS specific delay. Normal delay would halt the system for the duration => Is not multitasking!
		// https://www.freertos.org/a00127.html
		vTaskDelay( delay );  // not synchronous , do not use in control or in real time models

	}
}
#if 0
extern  void ui() {
	// ms -> tics, only if tick interval is more than ms, give directly as ticks
	const TickType_t delay = pdMS_TO_TICKS( 1000 );
	// Necessary forever loop. A thread should never be able to exit!
	for( ;; ) { // Same as while(1) or while(true)
		AXI_LED_DATA =AXI_LED_DATA ^ 0x01;
		// RTOS specific delay. Normal delay would halt the system for the duration => Is not multitasking!
		// https://www.freertos.org/a00127.html
		vTaskDelay( delay );  // not synchronous , do not use in control or in real time models
		set_Kp(0.1);
	}
}

if 0 /* Original */
extern  void ui() {
	// ms -> tics, only if tick interval is more than ms, give directly as ticks
	const TickType_t delay = pdMS_TO_TICKS( 1000 );
	// Necessary forever loop. A thread should never be able to exit!
	for( ;; ) { // Same as while(1) or while(true)
		AXI_LED_DATA =AXI_LED_DATA ^ 0x01;
		// RTOS specific delay. Normal delay would halt the system for the duration => Is not multitasking!
		// https://www.freertos.org/a00127.html
		vTaskDelay( delay );  // not synchronous , do not use in control or in real time models
	}
}
#endif
