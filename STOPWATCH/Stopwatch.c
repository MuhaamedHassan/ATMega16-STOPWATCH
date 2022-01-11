#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define F_CPU 1000000

unsigned char second_count = 0;
unsigned char minute_count = 0;
unsigned char hour_count = 0;
unsigned char flag_count = 0;

void TIMER1_CTC_Mode(void) {
	TCNT1 = 0; /* Initial Value For Counting */
	OCR1A = 975; /* To Make Interrupt Every 1 second */
	TCCR1A = (1 << FOC1A) | (1 << FOC1B); /* Non Pwm Mode /CTC */
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12); /* CTC MODE / Prescaling 1024 */
	TIMSK |= (1 << OCIE1A); /* Enable Module Interrupt for CTC Mode Compare A */
}
void INT_Init(void) {
	/* Note */
	/* IF U Configured Pins Of Interrupt After Set Mode Of It
	 * TIMER Wont Start Count Cause INT1 Will Happen When U Start
	 * STOPWATCH And Will Close Clock So U Will Need TO Enable It Again
	 */
	/*Configure Pins Of Interrupts */
	/* INT0 */
	DDRD &= ~(1 << PD2); /* INPUT */
	PORTD |= (1 << PD2); /* PULL UP */
	/*INT1*/
	DDRD &= ~(1 << PD3); /* INPUT */
	/*INT2*/
	DDRB &= ~(1 << PB2); /* INPUT */
	PORTB |= (1 << PB2); /* PULL UP */
	/* INT0 Falling INT1 Rising  */
	MCUCR = (1 << ISC01) | (1 << ISC11) | (1 << ISC10);
	/////////////////////////////////////////////
	MCUCSR &= ~(1 << ISC2); /*  Falling Edge Mode For INT2 */
	GICR = (1 << INT0) | (1 << INT1) | (1 << INT2); /* Enable Module Interrupt For Ex Interrupt */

}

int main(void) {
	SREG |= (1 << 7); /* Enable I-BIT */
	/*Configure Pins For 7-Segment & Decoder */
	/*Decoder */
	DDRC |= 0x0F; /* First 4 Pins PortC OUTPUT */
	PORTC &= 0xF0; /* First 4 Pins PORTC LOW */
	/* 7 Segment Enable / Disable */
	DDRA |= 0x3F; /* First 6 Pins PORTA OUPUT */
	PORTA |= 0x3F; /* First 6 Pins PORTA HIGH */
	/////////////////////////////////////////////////
	TIMER1_CTC_Mode(); /* Call Timer Function */
	INT_Init(); /* Call Interrupt Function */

	while (1) {
		if (flag_count == 1) {
			second_count++;
			if (second_count == 60) /* Counted 60 Second */
			{
				minute_count++; /* Increase Minute Counter */
				second_count = 0; /* Clearing Second Counter */
			}
			if (minute_count == 60) /* Counted 60 Minute */
			{
				minute_count = 0; /*Clear Minute Counter */
				hour_count++; /* Increase Hour Counter */
			}
			if (hour_count == 24) /* Counted 24 Hour */
			{
				/* Reset STOPWATCH */
				second_count = 0;
				minute_count = 0;
				hour_count = 0;
			}
			flag_count = 0; /* Clearing Flag To Be Ready to receive another second */
		} else {
			PORTA = (PORTA & 0xC0) | (1 << PA0); /* Enable 1st 7 Segment */
			PORTC = (PORTC & 0xF0) | (second_count % 10); /* Insert Number Of Second In 1st 7 Segment */
			_delay_ms(2); /* Delay For 2 Second To Display Full Number */
			PORTA = (PORTA & 0xC0) | (1 << PA1); /* Enable 2nd 7 Segment */
			PORTC = (PORTC & 0xF0) | (second_count / 10); /* Insert Number Of Second In 2nd 7 Segment */
			_delay_ms(2); /* Delay For 2 Second To Display Full Number */
			PORTA = (PORTA & 0xC0) | (1 << PA2); /* Enable 3rd 7 Segment */
			PORTC = (PORTC & 0xF0) | (minute_count % 10); /* Insert Number Of Second In 3rd 7 Segment */
			_delay_ms(2); /* Delay For 2 Second To Display Full Number */
			PORTA = (PORTA & 0xC0) | (1 << PA3); /* Enable 4th 7 Segment */
			PORTC = (PORTC & 0xF0) | (minute_count / 10); /* Insert Number Of Second In 4th 7 Segment */
			_delay_ms(2); /* Delay For 2 Second To Display Full Number */
			PORTA = (PORTA & 0xC0) | (1 << PA4); /* Enable 5th 7 Segment */
			PORTC = (PORTC & 0xF0) | (hour_count % 10); /* Insert Number Of Second In 5th 7 Segment */
			_delay_ms(2); /* Delay For 2 Second To Display Full Number */
			PORTA = (PORTA & 0xC0) | (1 << PA5); /* Enable 6th 7 Segment */
			PORTC = (PORTC & 0xF0) | (hour_count / 10); /* Insert Number Of Second In 6th 7 Segment */
			_delay_ms(2); /* Delay For 2 Second To Display Full Number */
		}
	}

}
ISR(TIMER1_COMPA_vect) {
	flag_count = 1; /* The Timer Counter 1 Second */
}
ISR(INT0_vect) {
	/* Reset Variable Of STOPWATCH */
	second_count = 0;
	minute_count = 0;
	hour_count = 0;
}
ISR(INT1_vect) {
	/* Close Clock Of Timer To Stop Counting */
	TCCR1B &= 0xF8;
}
ISR(INT2_vect) {
	/* Resume The STOPWATCH By Enable Clock Again */
	TCCR1B |= (1 << CS10) | (1 << CS12);
}
