
volatile byte state2 = LOW;
volatile byte state3 = LOW;
int i =0;
int incomingByte = 0;
char convert=NULL;
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
  Serial.println("Hello world - 248922");
}

void loop() 
{
  digitalWrite(0,state2);
  digitalWrite(1,state3);
  delay(1000);
  i++;
  if(Serial.available()>0)
  {
    incomingByte = Serial.read();
    convert = incomingByte;
    Serial.print(convert);
    Serial.print(" - ");
    Serial.println(incomingByte,DEC);
    i = 0;
  }
  if(i==10)
  {
     Serial.println("Czekam na znak, pospiesz sie…");
     i=0;
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
