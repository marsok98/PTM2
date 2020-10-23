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

uint8_t SwitchIsPushed(uint8_t PINx,uint8_t Key)//Handling of switch with debouncing
{
	uint8_t flag = 0;
	if(bit_is_set(PINx,Key))
	{
		_delay_ms(80);
		if(bit_is_set(PINx, Key))
		{
			flag = 1;
		}
	}
	return flag;
}


int getButton ()
{
	int TabelaPortow[] = {PC4,PC5,PC6,PC7}; //zawiera porty do sprawdzenia
	int i=0;				//zmienna sluzaca do wpisania liczby z klawiatury
	int k=0;				//iterator za pomoca ktorego bedziemy przemieszczac sie po TabeliPortow
	int LiczbaWpisana = 0;  //zmienna ktora bedzie zwracana jako ta ktora zostala wpisana przez uzytkownika
	int Wyjatek =0;			//zmienna ktora bedzie zapewniala zgodnosc z wytycznymi dla znakow * 0 i #
	for(k=0;k<4;k++)
	{
		sbi(PORTC,TabelaPortow[k]);	//odpytujemy kolejne porty
		switch (k)					//w tej strukturze w zaleznosci od portu wpisujemy wartosci na ktorych bedziemy bazowac
		{

			case 3:	i=1;			break;
			case 2:	i=2;			break;
			case 1:	i=3;			break;
			case 0:	Wyjatek = 13;	break;
			default:i=-1;			break;
		}
		if(SwitchIsPushed(PINC,PC0))	//teraz nastapi odpytanie wszystkich pinow od PD0 do PD4
		{
			i+=6;
			if(!k){i = Wyjatek+3;}    //Obsluga tej czwartej kolumny
			LiczbaWpisana=i;		//wpisujemy wartosc poczatkowa
			cbi(PORTC,TabelaPortow[k]);//koniec odpytania
			return LiczbaWpisana;		//mamy nasza wartosc wpisana wiec konczymy funkcje
		}
		else if(SwitchIsPushed(PINC,PC1))
		{
			i+=3;					//jest to PD1 wiec dodajemy do i 3, np 1+3 = 4  2+3=5
			if(!k){i = Wyjatek+2;}
			LiczbaWpisana=i;
			cbi(PORTC,TabelaPortow[k]);
			return LiczbaWpisana;
		}
		else if(SwitchIsPushed(PINC,PC2))
		{
			if(!k){i = Wyjatek+1;}
			LiczbaWpisana=i;
			cbi(PORTC,TabelaPortow[k]);
			return LiczbaWpisana;
		}
		else if(SwitchIsPushed(PINC,PC3))
		{
			i+=9;
			if(!k){i = Wyjatek;}
			LiczbaWpisana=i;
			cbi(PORTC,TabelaPortow[k]);
			return LiczbaWpisana;
		}
		else
		{
			LiczbaWpisana=-1;			//jesli nic nie wpiszemy zostanie zwrocone -1, ale nie w tym miejscu
										//poniewaz to zablokowaloby mozliwosc odpytania pozostalych portow
		}
		cbi(PORTC,TabelaPortow[k]);		//zamykamy port
	}
	return LiczbaWpisana;				// w praktyce wykona sie to tylko wtedy gdy nic nie wpiszemy i petla przejdzie wszystkie porty
}

void setLED(uint8_t led, uint8_t stan)
{
	switch(led)
	{
		case 1: if(stan)	{sbi(PORTD,PD0);}	else	{cbi(PORTD,PD0);} 	break;
		case 2: if(stan)	{sbi(PORTD,PD1);}	else	{cbi(PORTD,PD1);} 	break;
		case 3: if(stan)	{sbi(PORTD,PD2);}	else	{cbi(PORTD,PD2);} 	break;
		case 4: if(stan)	{sbi(PORTD,PD3);}	else	{cbi(PORTD,PD3);} 	break;
		case 5: if(stan)	{sbi(PORTD,PD4);}	else	{cbi(PORTD,PD4);} 	break;
		case 6: if(stan)	{sbi(PORTD,PD5);}	else	{cbi(PORTD,PD5);} 	break;
		case 7: if(stan)	{sbi(PORTD,PD6);}	else	{cbi(PORTD,PD6);} 	break;
		case 8: if(stan)	{sbi(PORTD,PD7);}	else	{cbi(PORTD,PD7);} 	break;
		default: break;
	}
}

void setLEDmy(uint8_t led)
{
	switch(led)
	{
		case 1: tbi(PORTD,PD0); break;
		case 2: tbi(PORTD,PD1); break;
		case 3: tbi(PORTD,PD2); break;
		case 4: tbi(PORTD,PD3); break;
		case 5: tbi(PORTD,PD4); break;
		case 6: tbi(PORTD,PD5); break;
		case 7: tbi(PORTD,PD6); break;
		case 8: tbi(PORTD,PD7); break;
		case 10:  PORTD=0b11111111; break; //zapalenie wszystkich
		case 12: PORTD=0b00000000;break; 	//zgaszenie wszystkich
		default: break;
	}
}



int main()
{
	DDRD=0b11111111;
	DDRC=0b00000000;
	while(1)
	{
		setLEDmy(getButton());
		_delay_ms(100);
	}

}
