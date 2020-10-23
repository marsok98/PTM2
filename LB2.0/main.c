#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>


#ifndef _BV
#define _BV(bit)				(1<<(bit))
#endif
#ifndef sbi
#define sbi(reg,bit)		reg |= (_BV(bit))
#endif

#ifndef cbi
#define cbi(reg,bit)		reg &= ~(_BV(bit))
#endif

#ifndef tbi
#define tbi(reg,bit)		reg ^= (_BV(bit))
#endif

#define KEY1 (1<<PD5) //niepotrzebne teraz
uint8_t flaga = 0;

uint8_t SwitchIsPushed(uint8_t PINx,uint8_t Key)//Handling of switch with debouncing
{
	uint8_t flag = 0;
	if(bit_is_clear(PINx,Key))
	{
		_delay_ms(80);
		if(bit_is_clear(PINx, Key))
		{
			flag = 1;
		}
	}
	return flag;
}
uint8_t counter = 0; // press counter
uint8_t Menu()       // Handling number of press
{
	uint8_t zmienna = SwitchIsPushed(PIND,PD5);
	if(zmienna)
	{
		counter++;
		if(counter >= 3){counter = 0;};
		return 0;
	}
	else{return 1;}
}

void InitInterrupt()		//inicjalizacja przerwania
{
	sbi(TCCR1B,WGM12);	//Wybranie trybu pracy CTC z TOP OCR1A
	sbi(TCCR1B,CS10);
	sbi(TCCR1B,CS12);		//Wybranie dzielnika czestotliwosci 1024
	OCR1A = 15625;           //Zapisanie do OCR1A wartosci odpowiadajacej 2s
					//8000000/1024/15625 = 0.5Hz, po odwroceniu 2s
	sbi(TIMSK,OCIE1A);	//Uruchomienie przerwania OCIE1A
}

int main()
{
	DDRD=0b1000000; 		//PD6 as output rest as input
	sbi(PORTD,PD5);
	sei();
	InitInterrupt();
	while(1)
	{
		do
		{
			if(counter == 1)
			{
				sbi(PORTD,PD6);
			}
			if(counter == 2)
			{
				cbi(PORTD,PD6);
			}

		}while(Menu());
		_delay_ms(50);
	}

}
ISR(TIMER1_COMPA_vect) {
	if(counter == 0)		//z dokladnoscia do 2s
	{
		tbi(PORTD,PD6);
	}
}
