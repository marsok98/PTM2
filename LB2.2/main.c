#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/sfr_defs.h>
#include <math.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "HD44780.h"


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


int ZrobOperacje(int s1, int s2, uint8_t dzialanie)  //funkcja ktora umozliwia nam obliczenie operacji dodawania lub odejmowania
													//dzialanie = 0 dodawanie dzialanie = 1 odejmowanie 2 mnozenie
{
	int wynik = 0;
	switch (dzialanie)
	{
		case 0: wynik = s1+s2; break;
		case 1: wynik = s1-s2; break;
		case 2: wynik = s1*s2; break;
		default: wynik =0; break;
	}
	return wynik;

}
int main() {
		LCD_Initalize();	//inicjalizacja wyswietlacza
		LCD_Home();
		int x=0;			//zmienna do ktorej wpiszemy wynik pobrany z klawiatury
		int ilosc_liczb1=0;	//ilosc liczb pierwszego skladnika, liczymy aby ograniczyc do 4 liczb
		int ilosc_liczb2=0;
		int skladnik1 = 0;	//liczba zapisana w postaci juz uporzadkowanej potrzebna do wykonanej operacji
		int skladnik2 = 0;
		uint8_t kolejnosc = 0; //zmienna opisujaca czy jest w tej chwili wprowadzany pierwszy czy drugi skladnik,
		char znak[3];			//tablica tak aby wpisac konkretna liczbe na wyswietlacz musi byc 3 bo dwucyfrowe liczby wpisujemy
		uint8_t dzialanie=0;	//0 dodawanie 1 odejmowanie 2 mnozenie
		int wynik=0;						//wynik w postaci liczby
		char wynikDoPokazania[5];			//wynik w postaci tablicy 5s elementowej do wyswietlenia
		uint8_t normalna_praca=0;			//normalna praca kalkulatora, czyli cos jest wpisane ale nie znak rowna sie ani C
		uint8_t brak_dzialania=0;			//ani + i - *
		while (1)
		{
			x = getButton();	//pobieramy dana z klawiatury
			normalna_praca= (x!=-1 && x!=13 &&x!=14);
			brak_dzialania = (x!=15 && x!=16 && x!=10);

			if (x == 14) //jesli juz byl Clear
			{
				LCD_Clear();
				ilosc_liczb1=0;
				ilosc_liczb2=0;
				skladnik1 = 0;
				skladnik2 = 0;
				kolejnosc = 0;
				dzialanie=0;
				wynik=0;
			}

			if(normalna_praca)       //jesli jest normalna praca kalkulatora, czyli cos jest wpisane ale nie znak rowna sie ani C
			{
				if(brak_dzialania && ilosc_liczb1<4 && kolejnosc==0) //jesli nie jest to operacja dodawania lub odejmowania
				{													//i jest to pierwsza liczba do wpisana
					if(x==11)										//jesli bylo 0 bo taki ma kod 11
					{
						skladnik1*=10;								//obsluga 0
						sprintf(znak,"%d",0);						//odrazu wpisujemy zero do zmiennej
					}
					else
					{
						skladnik1 =  10* skladnik1 + x;
						sprintf(znak,"%d",x);
					}
					LCD_GoTo(ilosc_liczb1,1);
					LCD_WriteText(znak);
					ilosc_liczb1++;									//zwiekszamy ilosc liczb wpisanych
				}
				else if (x==15)										//jesli wpisano dodawanie
				{
					dzialanie = 0;
					LCD_GoTo(ilosc_liczb1,1);
					LCD_WriteText("+");
					kolejnosc = 1;
				}
				else if (x == 16)									//jesli wpisano odejmowanie
				{
					dzialanie = 1;
					LCD_GoTo(ilosc_liczb1,1);
					LCD_WriteText("-");
					kolejnosc = 1;
				}
				else if (x == 10)									//jesli wpisano mnozenie
				{
					dzialanie = 2;
					LCD_GoTo(ilosc_liczb1,1);
					LCD_WriteText("*");
					kolejnosc = 1;
				}

				else if (brak_dzialania &&ilosc_liczb2<4 && kolejnosc == 1) //drugi skladnik dzialania
				{
					if(x==11)
					{
						skladnik2*=10;								//obsluga 0
						sprintf(znak,"%d",0);						//odrazu wpisujemy zero do zmiennej
					}
					else
					{
						skladnik2 =  10* skladnik2 + x;
						sprintf(znak,"%d",x);
					}
					LCD_GoTo(ilosc_liczb1+1+ilosc_liczb2,1);
					LCD_WriteText(znak);
					ilosc_liczb2++;
				}
			}
			else if (x == 13)							//jesli bylo rowna sie
			{
				LCD_GoTo(ilosc_liczb1+1+ilosc_liczb2,1);			//odpowiednie formatowanie
				LCD_WriteText("=");
				wynik = ZrobOperacje(skladnik1, skladnik2, dzialanie); //zrob konkretna operacje
				LCD_GoTo(ilosc_liczb1+1+ilosc_liczb2+1,1);
				sprintf(wynikDoPokazania,"%d",wynik);
				LCD_WriteText(wynikDoPokazania);					//pokaz wynik
			}

			_delay_ms(250);
		}

}








