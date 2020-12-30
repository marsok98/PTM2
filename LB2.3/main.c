#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "HD44780.h"


#ifndef _BV
#define _BV(bit)				(1<<(bit))
#endif

#ifndef inb
#define	inb(addr)			(addr)
#endif

#ifndef outb
#define	outb(addr, data)	addr = (data)
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

/*
 *  Gotowe zaimplementowane:
 #define 	bit_is_set(sfr, bit)   (_SFR_BYTE(sfr) & _BV(bit))
 #define 	bit_is_clear(sfr, bit)   (!(_SFR_BYTE(sfr) & _BV(bit)))
 #define 	loop_until_bit_is_set(sfr, bit)   do { } while (bit_is_clear(sfr, bit))
 #define 	loop_until_bit_is_clear(sfr, bit)   do { } while (bit_is_set(sfr, bit))

 */

// MIN/MAX/ABS macros
#define MIN(a,b)			((a<b)?(a):(b))
#define MAX(a,b)			((a>b)?(a):(b))
#define ABS(x)				((x>0)?(x):(-x))

void ADC_init()
{
	sbi(ADMUX, REFS0); //tryb AVCC
	sbi(ADCSRA,ADPS2);
	sbi(ADCSRA,ADPS1);
	sbi(ADCSRA,ADPS0); //ustawienie preskalera na 128
	sbi(ADCSRA,ADEN); //wlaczenie przetwornika
	cbi(ADMUX,MUX0);
}

int ADC_10bit()
{
	sbi(ADCSRA,ADSC); //uruchomienie pojedynczego pomiaru
	loop_until_bit_is_set(ADCSRA, ADSC);
	return ADC;
}

int ConvertAnalogToVolt(uint16_t odczyt)
{
	int x;
	x = (odczyt * 5.0)/1023.00 * 100;
	return x;
}

int ConvertAnalogToPercent(uint16_t odczyt)
{
		int x;
		x = (odczyt * 100)/1023.00;
		return x;
}

void dwupolozeniowy(uint8_t wartoscZadana, uint8_t wartoscRzeczywista)
{
	if (wartoscRzeczywista < wartoscZadana)
	{
		sbi(PORTD,PD0);
	}
	else if (wartoscRzeczywista > wartoscZadana)
	{
		cbi(PORTD,PD0);
	}
}

void dwupolozeniowy_histereza(uint8_t wartoscZadana, uint8_t wartoscRzeczywista, uint8_t histereza)
{
	if (wartoscRzeczywista + histereza < wartoscZadana)
	{
		sbi(PORTD,PD1);
	}
	else if(wartoscRzeczywista - histereza < wartoscZadana)
	{
		cbi(PORTD,PD1);
	}
}
void trzypolozeniowy(uint8_t wartoscZadana, uint8_t wartoscRzeczywista, uint8_t bufor)
{
	if (wartoscRzeczywista + bufor < wartoscZadana)
	{
		sbi(PORTD,PD2);
		cbi(PORTD,PD3);
	}
	else if(wartoscRzeczywista - bufor > wartoscZadana)
	{
		cbi(PORTD,PD2);
		sbi(PORTD,PD3);
	}
	else
	{
		cbi(PORTD,PD2);
		cbi(PORTD,PD3);
	}
}

int main() {
	LCD_Initalize();
	LCD_Home();
	ADC_init();
	DDRC=0xFF; //ustawia na portach C jedynki, bo sterujemy zerem chyba, dzieki temu lepiej swiecia
	DDRD=0xFF;
	char znaki1[20];
	char znaki2[20];
	LCD_Clear();
	LCD_GoTo(0,0);
	int napiecie1 = 0;
	int percent1 = 0;
	int napiecie2 = 0;
	int percent2 = 0;
	while(1)
	{
		cbi(ADMUX,MUX0);
		LCD_GoTo(0,0);
		napiecie1 = ConvertAnalogToVolt(ADC_10bit());
		percent1 = ConvertAnalogToPercent(ADC_10bit());
		sprintf(znaki1,"Nap,Pro: %d, %d",napiecie1,percent1);
		LCD_WriteText(znaki1);

		sbi(ADMUX,MUX0);
		LCD_GoTo(0,1);
		napiecie2 = ConvertAnalogToVolt(ADC_10bit());
		percent2 = ConvertAnalogToPercent(ADC_10bit());
		sprintf(znaki2,"Nap,Pro: %d, %d",napiecie2,percent2);
		LCD_WriteText(znaki2);

		dwupolozeniowy(napiecie1,napiecie2);
		dwupolozeniowy_histereza(napiecie1,napiecie2,5);
		trzypolozeniowy(napiecie1,napiecie2,100);

		_delay_ms(50);
	}
}


