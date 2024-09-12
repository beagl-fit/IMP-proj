/* Created by: David Novak (xnovak2r) */
/* Project based on IMP_projekt_had_tabule_test file */

/* Header file with all the essential definitions for a given type of MCU */
#include "MK60D10.h"
//#include "MK60DZ10.h"

/* Left and right buttons for changing messages */
#define BUTTON_RIGHT_MASK 	0x400
#define BUTTON_LEFT_MASK 	0x8000000

/* Macros for bit-level registers manipulation */
#define GPIO_PIN_MASK	0x1Fu
#define GPIO_PIN(x)		(((1)<<(x & GPIO_PIN_MASK)))

/* Constants specifying delay loop duration */
//#define	tdelay1			10000
#define	tdelay1			100
#define tdelay2 		20

/* Define message 0/1 */
unsigned msg = 0;

/* Configuration of the necessary MCU peripherals */
void SystemConfig() {
	/* Turn on all port clocks */
	SIM->SCGC5 = SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;


	/* Set corresponding PTA pins (column activators of 74HC154) for GPIO functionality */
	PORTA->PCR[8] = ( 0|PORT_PCR_MUX(0x01) );  // A0
	PORTA->PCR[10] = ( 0|PORT_PCR_MUX(0x01) ); // A1
	PORTA->PCR[6] = ( 0|PORT_PCR_MUX(0x01) );  // A2
	PORTA->PCR[11] = ( 0|PORT_PCR_MUX(0x01) ); // A3	MSB

	/* Set corresponding PTA pins (rows selectors of 74HC154) for GPIO functionality */
	PORTA->PCR[26] = ( 0|PORT_PCR_MUX(0x01) );  // R0
	PORTA->PCR[24] = ( 0|PORT_PCR_MUX(0x01) );  // R1
	PORTA->PCR[9] = ( 0|PORT_PCR_MUX(0x01) );   // R2
	PORTA->PCR[25] = ( 0|PORT_PCR_MUX(0x01) );  // R3
	PORTA->PCR[28] = ( 0|PORT_PCR_MUX(0x01) );  // R4
	PORTA->PCR[7] = ( 0|PORT_PCR_MUX(0x01) );   // R5
	PORTA->PCR[27] = ( 0|PORT_PCR_MUX(0x01) );  // R6
	PORTA->PCR[29] = ( 0|PORT_PCR_MUX(0x01) );  // R7

	/* Set corresponding PTE pins (output enable of 74HC154) for GPIO functionality */
	PORTE->PCR[28] = ( 0|PORT_PCR_MUX(0x01) ); // #EN


	/* Change corresponding PTA port pins as outputs */
	PTA->PDDR = GPIO_PDDR_PDD(0x3F000FC0);

	/* Change corresponding PTE port pins as outputs */
	PTE->PDDR = GPIO_PDDR_PDD( GPIO_PIN(28) );

	// button interrupt config for L and R buttons
	NVIC_ClearPendingIRQ(PORTE_IRQn);
	PORTE->PCR[10] = ( 	PORT_PCR_ISF(0X01)
						| PORT_PCR_IRQC(0X0A)
						| PORT_PCR_MUX(0X01)
						| PORT_PCR_PE(0X01)
						| PORT_PCR_PS(0X01));

	PORTE->PCR[27] = ( 	PORT_PCR_ISF(0X01)
						| PORT_PCR_IRQC(0X0A)
						| PORT_PCR_MUX(0X01)
						| PORT_PCR_PE(0X01)
						| PORT_PCR_PS(0X01));
	NVIC_EnableIRQ(PORTE_IRQn);

	// enable PIT
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
	PIT->MCR = 0x00;
	// timer 0 - every 0.1s
	PIT->CHANNEL[0].LDVAL = 0x004C4B3F;
	//	PIT->CHANNEL[0].LDVAL = 0x017D783F; 0.5s
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}


/* Variable delay loop */
void delay(int t1, int t2)
{
	int i, j;

	for(i=0; i<t1; i++) {
		for(j=0; j<t2; j++);
	}
}


/* Conversion of requested column number into the 4-to-16 decoder control.  */
void column_select(unsigned int col_num)
{
	unsigned i, result, col_sel[4];

	for (i =0; i<4; i++) {
		result = col_num / 2;	  // Whole-number division of the input number
		col_sel[i] = col_num % 2;
		col_num = result;

		switch(i) {

			// Selection signal A0
		    case 0:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(8))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(8)));
				break;

			// Selection signal A1
			case 1:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(10))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(10)));
				break;

			// Selection signal A2
			case 2:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(6))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(6)));
				break;

			// Selection signal A3
			case 3:
				((col_sel[i]) == 0) ? (PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(11))) : (PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(11)));
				break;

			// Otherwise nothing to do...
			default:
				break;
		}
	}
}

/******* letter functions *******/
/*******************************/
/************ 	D	 ***********/
void letter_D0(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	delay(tdelay1, tdelay2);
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_D1(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_D2(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

/*******************************/
/************ 	A	 ***********/
void letter_A0(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}


void letter_A1(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_A2(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

/*******************************/
/************ 	V	 ***********/
void letter_V0(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_V1(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_V2(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

/*******************************/
/************ 	I	 ***********/
void letter_I0(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_I1(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	delay(tdelay1, tdelay2);
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_I2(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

/*******************************/
/************ 	N	 ***********/
void letter_N0(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	delay(tdelay1, tdelay2);
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}
void letter_N1(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}
void letter_N2(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_N3(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	delay(tdelay1, tdelay2);
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}
/*******************************/
/************ 	O	 ***********/
void letter_O0(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}
void letter_O1(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}
void letter_O2(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}


/*******************************/
/************ 	K	 ***********/
void letter_K0(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	delay(tdelay1, tdelay2);
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}
void letter_K1(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}
void letter_K2(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

void letter_K3(unsigned int start){
	column_select(start);
	PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
	PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
	delay(tdelay1, tdelay2);
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
	PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
	PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
}

/*******************************/
/*******************************/
/*

		PTE->PDDR &= ~GPIO_PDDR_PDD( GPIO_PIN(28));
		PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(26));
		PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(24));
		PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(9));
		PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(25));
		PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(28));
		PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(7));
		PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(27));
		PTA->PDOR &= ~GPIO_PDOR_PDO( GPIO_PIN(29));
		delay(tdelay1, tdelay2);
		PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(26));
		PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(24));
		PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(9));
		PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(25));
		PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(28));
		PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(7));
		PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(27));
		PTA->PDOR |= GPIO_PDOR_PDO( GPIO_PIN(29));
		PTE->PDDR |= GPIO_PDDR_PDD( GPIO_PIN(28));
 */

/* Button interrupt handler */
void PORTE_IRQHandler(void){
	if(PORTE->ISFR & BUTTON_RIGHT_MASK)
		msg = 1;
	else if(PORTE->ISFR & BUTTON_LEFT_MASK)
		msg = 0;

	PORTE->ISFR |= BUTTON_LEFT_MASK | BUTTON_RIGHT_MASK;
}


int main(void)
{
	SystemConfig();

	delay(tdelay1, tdelay2);
	int len;
	for (;;) {
		if(msg == 0){
			// len = 5 letters; letter = 3 cols + 1 space; + 1 because N is 4 cols
			len = 5*4 + 1;
			for(int i = 16; i >= -len; i--) {
				for(;;) {
					int col = i;
					col += len;
					col--;
					if(col >= 0 && col < 16)
						letter_K2(col);
					col--;
					if(col >= 0 && col < 16)
						letter_K1(col);
					col--;
					if(col >= 0 && col < 16)
						letter_K0(col);
					col--;

					col--;
					if(col >= 0 && col < 16)
						letter_A2(col);
					col--;
					if(col >= 0 && col < 16)
						letter_A1(col);
					col--;
					if(col >= 0 && col < 16)
						letter_A0(col);
					col--;

					col--;
					if(col >= 0 && col < 16)
						letter_V2(col);
					col--;
					if(col >= 0 && col < 16)
						letter_V1(col);
					col--;
					if(col >= 0 && col < 16)
						letter_V0(col);
					col--;

					col--;
					if(col >= 0 && col < 16)
						letter_O2(col);
					col--;
					if(col >= 0 && col < 16)
						letter_O1(col);
					col--;
					if(col >= 0 && col < 16)
						letter_O0(col);
					col--;

					col--;
					if(col >= 0 && col < 16)
						letter_N3(col);
					col--;
					if(col >= 0 && col < 16)
						letter_N2(col);
					col--;
					if(col >= 0 && col < 16)
						letter_N1(col);
					col--;
					if(col >= 0 && col < 16)
						letter_N0(col);

					if(PIT->CHANNEL[0].TFLG){
						PIT->CHANNEL[0].TFLG = 0x1;
						break;
					}
				}
			if(msg == 1)
				break;
			}
		}

		// 34 below is number of cases and there is 1 case for every possible state of DAVID on led display
		if(msg == 1){
			for(unsigned state = 0; state < 34; state++) {
				for(;;) {
					switch(state) {
						case 0:
							letter_D2(0);
							break;
						case 1:
							letter_D1(0);
							letter_D2(1);
							break;
						case 2:
							letter_D0(0);
							letter_D1(1);
							letter_D2(2);
							break;
						case 3:
							letter_D0(1);
							letter_D1(2);
							letter_D2(3);
							break;
						case 4:
							letter_I2(0);
							letter_D0(2);
							letter_D1(3);
							letter_D2(4);
							break;
						case 5:
							letter_I1(0);
							letter_I2(1);
							letter_D0(3);
							letter_D1(4);
							letter_D2(5);
							break;
						case 6:
							letter_I0(0);
							letter_I1(1);
							letter_I2(2);
							letter_D0(4);
							letter_D1(5);
							letter_D2(6);
							break;
						case 7:
							letter_I0(1);
							letter_I1(2);
							letter_I2(3);
							letter_D0(5);
							letter_D1(6);
							letter_D2(7);
							break;
						case 8:
							letter_V2(0);
							letter_I0(2);
							letter_I1(3);
							letter_I2(4);
							letter_D0(6);
							letter_D1(7);
							letter_D2(8);
							break;
						case 9:
							letter_V1(0);
							letter_V2(1);
							letter_I0(3);
							letter_I1(4);
							letter_I2(5);
							letter_D0(7);
							letter_D1(8);
							letter_D2(9);
							break;
						case 10:
							letter_V0(0);
							letter_V1(1);
							letter_V2(2);
							letter_I0(4);
							letter_I1(5);
							letter_I2(6);
							letter_D0(8);
							letter_D1(9);
							letter_D2(10);
							break;
						case 11:
							letter_V0(1);
							letter_V1(2);
							letter_V2(3);
							letter_I0(5);
							letter_I1(6);
							letter_I2(7);
							letter_D0(9);
							letter_D1(10);
							letter_D2(11);
							break;
						case 12:
							letter_A2(0);
							letter_V0(2);
							letter_V1(3);
							letter_V2(4);
							letter_I0(6);
							letter_I1(7);
							letter_I2(8);
							letter_D0(10);
							letter_D1(11);
							letter_D2(12);
							break;
						case 13:
							letter_A1(0);
							letter_A2(1);
							letter_V0(3);
							letter_V1(4);
							letter_V2(5);
							letter_I0(7);
							letter_I1(8);
							letter_I2(9);
							letter_D0(11);
							letter_D1(12);
							letter_D2(13);
							break;
						case 14:
							letter_A0(0);
							letter_A1(1);
							letter_A2(2);
							letter_V0(4);
							letter_V1(5);
							letter_V2(6);
							letter_I0(8);
							letter_I1(9);
							letter_I2(10);
							letter_D0(12);
							letter_D1(13);
							letter_D2(14);
							break;
						case 15:
							letter_A0(1);
							letter_A1(2);
							letter_A2(3);
							letter_V0(5);
							letter_V1(6);
							letter_V2(7);
							letter_I0(9);
							letter_I1(10);
							letter_I2(11);
							letter_D0(13);
							letter_D1(14);
							letter_D2(15);
							break;
						case 16:
							letter_D2(0);
							letter_A0(2);
							letter_A1(3);
							letter_A2(4);
							letter_V0(6);
							letter_V1(7);
							letter_V2(8);
							letter_I0(10);
							letter_I1(11);
							letter_I2(12);
							letter_D0(14);
							letter_D1(15);
							break;
						case 17:
							letter_D1(0);
							letter_D2(1);
							letter_A0(3);
							letter_A1(4);
							letter_A2(5);
							letter_V0(7);
							letter_V1(8);
							letter_V2(9);
							letter_I0(11);
							letter_I1(12);
							letter_I2(13);
							letter_D0(15);
							break;
						case 18:
							letter_D0(0);
							letter_D1(1);
							letter_D2(2);
							letter_A0(4);
							letter_A1(5);
							letter_A2(6);
							letter_V0(8);
							letter_V1(9);
							letter_V2(10);
							letter_I0(12);
							letter_I1(13);
							letter_I2(14);
							break;
						case 19:
							letter_D0(1);
							letter_D1(2);
							letter_D2(3);
							letter_A0(5);
							letter_A1(6);
							letter_A2(7);
							letter_V0(9);
							letter_V1(10);
							letter_V2(11);
							letter_I0(13);
							letter_I1(14);
							letter_I2(15);
							break;
						case 20:
							letter_D0(2);
							letter_D1(3);
							letter_D2(4);
							letter_A0(6);
							letter_A1(7);
							letter_A2(8);
							letter_V0(10);
							letter_V1(11);
							letter_V2(12);
							letter_I0(14);
							letter_I1(15);
							break;
						case 21:
							letter_D0(3);
							letter_D1(4);
							letter_D2(5);
							letter_A0(7);
							letter_A1(8);
							letter_A2(9);
							letter_V0(11);
							letter_V1(12);
							letter_V2(13);
							letter_I0(15);
							break;
						case 22:
							letter_D0(4);
							letter_D1(5);
							letter_D2(6);
							letter_A0(8);
							letter_A1(9);
							letter_A2(10);
							letter_V0(12);
							letter_V1(13);
							letter_V2(14);
							break;
						case 23:
							letter_D0(5);
							letter_D1(8);
							letter_D2(7);
							letter_A0(9);
							letter_A1(10);
							letter_A2(11);
							letter_V0(13);
							letter_V1(14);
							letter_V2(15);
							break;
						case 24:
							letter_D0(6);
							letter_D1(7);
							letter_D2(8);
							letter_A0(10);
							letter_A1(11);
							letter_A2(12);
							letter_V0(14);
							letter_V1(15);
							break;
						case 25:
							letter_D0(7);
							letter_D1(8);
							letter_D2(9);
							letter_A0(11);
							letter_A1(12);
							letter_A2(13);
							letter_V0(15);
							break;
						case 26:
							letter_D0(8);
							letter_D1(9);
							letter_D2(10);
							letter_A0(12);
							letter_A1(13);
							letter_A2(14);
							break;
						case 27:
							letter_D0(9);
							letter_D1(10);
							letter_D2(11);
							letter_A0(13);
							letter_A1(14);
							letter_A2(15);
							break;
						case 28:
							letter_D0(10);
							letter_D1(11);
							letter_D2(12);
							letter_A0(14);
							letter_A1(15);
							break;
						case 29:
							letter_D0(11);
							letter_D1(12);
							letter_D2(13);
							letter_A0(15);
							break;
						case 30:
							letter_D0(12);
							letter_D1(13);
							letter_D2(14);
							break;
						case 31:
							letter_D0(13);
							letter_D1(14);
							letter_D2(15);
							break;
						case 32:
							letter_D0(14);
							letter_D1(15);
							break;
						case 33:
							letter_D0(15);
							break;
						default:
							break;
					}

					if(PIT->CHANNEL[0].TFLG){
						PIT->CHANNEL[0].TFLG = 0x1;
						break;
					}
				}
			if(msg == 0)
				break;
			}
		}
	}

    /* Never leave main */
    return 0;
}
