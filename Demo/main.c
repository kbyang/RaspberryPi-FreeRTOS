//main.c
//authored by Jared Hull
//
//tasks 1 and 2 blink the ACT LED
//main initialises the devices and IP tasks

#include <FreeRTOS.h>
#include <task.h>

#include "interrupts.h"
#include "gpio.h"
#include "video.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

void task1() {
	while(1) {
		SetGpio(2, 1);
		vTaskDelay(2);
		SetGpio(2, 0);
		vTaskDelay(2);
	}
}

int main(void) {
	SetGpioFunction(2, 1);			// RDY led

	initFB();

	SetGpio(2, 1);
	

	DisableInterrupts();
	InitInterruptController();

	xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);

	//set to 0 for no debug, 1 for debug, or 2 for GCC instrumentation (if enabled in config)
	loaded = 1;

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
