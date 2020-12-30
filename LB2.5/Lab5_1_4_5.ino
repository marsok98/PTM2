
volatile byte state2 = LOW;
volatile byte state3 = LOW;
int i =0;
int incomingByte = 0;
char convert=NULL;
String Rok_urodzenia;
int RokInt;
int wiek=0;
void setup() 
{
  pinMode(2,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2),blink2,CHANGE);
  pinMode(3,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3),blink3,CHANGE);
  pinMode(0,OUTPUT);
  pinMode(1,OUTPUT);
  /////////////////////////////////////////////
  Serial.begin(9600);
  Serial.println("Licze twoj wiek - 248922");
  Serial.println("Podaj swoj wiek");
}

void loop() 
{
  digitalWrite(0,state2);
  digitalWrite(1,state3);
  delay(1000);
  if(Serial.available()>0)
  {
    Rok_urodzenia = Serial.readStringUntil("\n");
    RokInt = Rok_urodzenia.toInt();
    wiek = 2020-RokInt;
    if(wiek < 18){Serial.print("Masz ");Serial.print(wiek);Serial.println(" lat jestes niepelnoletni");}
    if( 18 <= wiek && wiek < 25){Serial.print("Masz ");Serial.print(wiek);Serial.println(" lat jestes pelnoletni, ale nic nie robisz");}
    if( 25 <= wiek && wiek < 65){Serial.print("Masz ");Serial.print(wiek);Serial.println(" lat jestes pelnoletni, ale nie wejdziesz do sklepu od 10-12");}
    if( 65 <= wiek){Serial.print("Masz ");Serial.print(wiek);Serial.println(" lat jestes pelnoletni,  wejdziesz do sklepu od 10-12");}
    if(wiek)
    {
      Serial.println("Podaj swoj wiek");
      wiek=0;
    }
  }
  
  
  
}

void blink2()
{
  state2 = !state2;
  if(state2)
  {
    Serial.println("Dioda2 ON");
  }
  else
  {
    Serial.println("Dioda2 OFF");
  }
}

void blink3()
{
  state3 = !state3;
  if(state3)
  {
    Serial.println("Dioda3 ON");
  }
  else
  {
    Serial.println("Dioda3 OFF");
  }
}
