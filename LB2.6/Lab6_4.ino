#include <Servo.h>

Servo serwomechanizm;
int pozycja = 0;
int zmiana = 6;
String pozycja_wpisana;
int pozycja_stopien  =0;
int pozycja_procent =0;
bool wyswietlono_komunikat = false;
void setup() 
{
  Serial.begin(9600);
  Serial.println("Servo 248922");
  serwomechanizm.attach(9);
}

void loop() 
{
  
  if(Serial.available()>0)
  {
    pozycja_wpisana = Serial.readStringUntil("\n");
    pozycja_procent = pozycja_wpisana.toInt();
    pozycja_stopien = map(pozycja_procent,0,100,0,180);
    wyswietlono_komunikat = false;
  }
  
  if(pozycja_stopien <= 180 && pozycja_stopien >=0)
  {
    serwomechanizm.write(pozycja_stopien);
    
    if(!wyswietlono_komunikat) {Serial.println("Krece \n");Serial.println(pozycja_stopien);}
    wyswietlono_komunikat = true;
  }
  else
  {
    if(!wyswietlono_komunikat) {Serial.println("Wpisano zla wartosc, sprobuj ponownie");}
    wyswietlono_komunikat = true;
  }
  
  
  
  
}
