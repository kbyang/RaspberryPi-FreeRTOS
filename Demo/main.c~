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

/*
 * Input commands and pin mounts
 */

#define FORWARD 'F'
#define LEFT    'L'
#define BACK    'B'
#define RIGHT   'R'
#define STOP    'S'

//Adjust to add intermediate speeds
#define IDLE    0
#define SPEED   1

//Adjust to reverse motor polarity
int LEFT_MOTOR  = 1;
int RIGHT_MOTOR = 0;

//These pins are for the RPI3 B, adjust if using a different board
#define EN1 20  //pwm pin, left motor
#define EN2 21  //pwm pin, right motor
#define A_1 6   //Y1, left motor positive
#define A_2 13  //Y2, left motor negative
#define A_3 19  //Y3, right motor positive
#define A_4 26  //Y4, right motor negative

#define RS 2
#define RW 3
#define E_ 4
#define D4 17
#define D5 27
#define D6 22
#define D7 10

#define TRIG 9
#define ECHO 11

#define T1_PIN 14
#define T2_PIN 15
#define T3_PIN 18

void moveRobot(char command);
void motorControl(int ifLeftMotor, char command);
void attach();
void setPins(int a, int b, int c, int d, int e, int f);
void moveTo(int line, int pos);
void displayOn();
void printChar(char cnum);
void clearLCD();
void printLCD(char str[7]);

int DISTANCE = 0;
char CURRENT_DIR = STOP;
int SWITCHTASK3 = 0;

void task1() {
	portTickType xLastWakeTime;
	const portTickType xFrequency = 60 / portTICK_RATE_MS;
	
	xLastWakeTime = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		
		//IN TASK
		SetGpio(T1_PIN, 1);
		//

		SetGpio(TRIG, 1);

		vTaskDelay(1);

		SetGpio(TRIG, 0);
		while(ReadGpio(ECHO) == 0);
		
		portTickType curr = xTaskGetTickCount();
		while(ReadGpio(ECHO) == 1);
		portTickType traveltime = xTaskGetTickCount() - curr;

		DISTANCE = (traveltime * portTICK_RATE_MS * 1000) / 58;
		
		//
		SetGpio(T1_PIN, 0);
		//END TASK
	}
}

void task2() {
	portTickType xLastWakeTime;
	const portTickType xFrequency = 100 / portTICK_RATE_MS;
	
	xLastWakeTime = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
		
		//IN TASK
		SetGpio(T2_PIN, 1);
		//

		if (DISTANCE < 20) {
			CURRENT_DIR = STOP;
			moveRobot(STOP);
		} else {
			CURRENT_DIR = FORWARD;
			moveRobot(FORWARD);
		}

		//
		SetGpio(T2_PIN, 0);
		//END TASK
	}
}

void task3() {
	char buf[5];

	while(1) {
		//IN TASK
		SetGpio(T3_PIN, 1);
		//

		if (SWITCHTASK3 == 0) {
			SWITCHTASK3 = 1;

			size_t i = 1u;

			int distance = DISTANCE;
			for (; i<=5; i++) {
				buf[5-i] = (char)((distance % 10u) + '0');
				distance /= 10u;
			}
			clearLCD();
			printLCD(buf);
		} else {
			SWITCHTASK3 = 0;
			clearLCD();
			switch(CURRENT_DIR) {
				case FORWARD:
				    printLCD("FORWARD");
				    break;
				case LEFT:
				    printLCD("LEFT   ");
				    break; 
				case BACK:
				    printLCD("BACK   ");
				    break; 
				case RIGHT:
				    printLCD("RIGHT  ");
				    break;
				case STOP:
				    printLCD("STOP   ");
				    break;
				default:
				    break;
			    }   
			
		}
		//
		SetGpio(T3_PIN, 0);
		//END TASK
	}
}

int main(void) {
	SetGpioFunction(T1_PIN, 1);
	SetGpioFunction(T2_PIN, 1);
	SetGpioFunction(T3_PIN, 1);
	SetGpio(T1_PIN, 0);
	SetGpio(T2_PIN, 0);
	SetGpio(T3_PIN, 0);

	SetGpioFunction(EN1, 1);
	SetGpioFunction(EN2, 1);
	SetGpioFunction(A_1, 1);
	SetGpioFunction(A_2, 1);
	SetGpioFunction(A_3, 1);
	SetGpioFunction(A_4, 1);

	SetGpioFunction(TRIG, 1);
	SetGpioFunction(ECHO, 0);
	SetGpio(TRIG, 0);
	
	attach();
	displayOn();
	moveTo(0,0);
	printLCD("Hi!    ");

	initFB();

	DisableInterrupts();
	InitInterruptController();

	xTaskCreate(task1, "t1", 128, NULL, 2, NULL);
	xTaskCreate(task2, "t2", 128, NULL, 1, NULL);
	xTaskCreate(task3, "t3", 128, NULL, 0, NULL);

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

void moveRobot(char command) {
    switch(command) {
        case FORWARD:
            motorControl(LEFT_MOTOR, FORWARD);
            motorControl(RIGHT_MOTOR, FORWARD);
            break;
        case LEFT:
            motorControl(LEFT_MOTOR, STOP);
            motorControl(RIGHT_MOTOR, FORWARD);
            break; 
        case BACK:
            motorControl(LEFT_MOTOR, BACK);
            motorControl(RIGHT_MOTOR, BACK);
            break; 
        case RIGHT:
            motorControl(LEFT_MOTOR, FORWARD);
            motorControl(RIGHT_MOTOR, STOP);
            break;
        case STOP:
            motorControl(LEFT_MOTOR, STOP);
            motorControl(RIGHT_MOTOR, STOP);
            break;
        default:
            break;
    }   
}

void motorControl(int ifLeftMotor, char command) {
    int enable      = ifLeftMotor ? EN1 : EN2;
    int motorPos    = ifLeftMotor ? A_1 : A_3;
    int motorNeg    = ifLeftMotor ? A_2 : A_4;

    switch (command) {
        case FORWARD:
            SetGpio(enable, SPEED);
            SetGpio(motorPos, 1);
            SetGpio(motorNeg, 0);
            break;
        case BACK:
            SetGpio(enable, SPEED);
            SetGpio(motorPos, 0);
            SetGpio(motorNeg, 1);
            break;    
        case STOP:
            SetGpio(motorPos, 0);
            SetGpio(motorNeg, 0);
            break;
        default:
            break;           
    }
}

void attach() {
	SetGpioFunction(RS, 1);
	SetGpioFunction(RW, 1);
	SetGpioFunction(E_, 1);
	SetGpioFunction(D4, 1);
	SetGpioFunction(D5, 1);
	SetGpioFunction(D6, 1);
	SetGpioFunction(D7, 1);
	setPins(0,0,0,0,1,0);
	setPins(0,0,0,0,1,0);
	setPins(0,0,1,0,0,0);
}
void setPins(int a, int b, int c, int d, int e, int f) {
	SetGpio(RS, a);
	SetGpio(RW, b);
	SetGpio(D7, c);
	SetGpio(D6, d);
	SetGpio(D5, e);
	SetGpio(D4, f);
	SetGpio(E_, 1);
	volatile int i = 0;
	for(i = 0; i < 7000; ++i);
	SetGpio(E_, 0);
	for(i = 0; i < 7000; ++i);
}

void moveTo(int line, int pos) {
	int b3 = (pos >> 3) & 0x1;
	int b2 = (pos >> 2) & 0x1;
	int b1 = (pos >> 1) & 0x1;
	int b0 = (pos >> 0) & 0x1;

	setPins(0,0,1,line,0,0);
	setPins(0,0,b3,b2,b1,b0);
}

void displayOn() {
	setPins(0,0,0,0,0,0);
	setPins(0,0,1,1,0,0);
	setPins(0,0,0,0,0,0);
	setPins(0,0,0,1,1,0);
}

void printChar(char cnum) {
	int b7 = (cnum >> 7) & 0x1;
	int b6 = (cnum >> 6) & 0x1;
	int b5 = (cnum >> 5) & 0x1;
	int b4 = (cnum >> 4) & 0x1;
	int b3 = (cnum >> 3) & 0x1;
	int b2 = (cnum >> 2) & 0x1;
	int b1 = (cnum >> 1) & 0x1;
	int b0 = (cnum >> 0) & 0x1;

	setPins(1,0,b7,b6,b5,b4);
	setPins(1,0,b3,b2,b1,b0);
}

void clearLCD(){
	moveTo(0, 0);
	printLCD("       ");
	moveTo(0, 0);
}

void printLCD(char str[7]) {
	for (int i = 0; i < 7; ++i) {
		printChar(str[i]);
	}
}
