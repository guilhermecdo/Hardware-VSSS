#include <SPI.h>           
#include <nRF24L01.h>             
#include <RF24.h>
#include <TimerOne.h>

#define timer 33333
#define CE 7 //define o pino 7 como CE
#define CS 8 //define o pino 8 como CS

RF24 radio(CE,CS);                              
const uint64_t pipe[6]={0xF1F1F1F1E1LL,0xF0F0F0F0E2LL,0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL};// Endereços do radio
uint8_t data[6];
int i=0;
void setup(){
  Serial.begin(9600);
  Serial.setTimeout(200000);
  radio.begin();
  radio.openWritingPipe(pipe[0]);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(111);
  radio.stopListening();

  Timer1.initialize(timer);
  Timer1.attachInterrupt(envia);
}

void loop(){
}

void envia(){
  i++;
  data[0]=1;
  Serial.print("id:");
  Serial.println(data[0]);
  data[1]=random(0,127);
  Serial.print("vx:");
  Serial.println(data[1]);
  data[2]=random(0,127);
  Serial.print("v0:");
  Serial.println(data[2]);
  data[3]=3;
  Serial.print("dx:");
  Serial.println(data[3]);
  data[4]=3;
  Serial.print("d0:");
  Serial.println(data[4]);
  data[5]=i;
  Serial.print("Nº");
  Serial.println(data[5]);
  radio.write(&data,sizeof(data));
}
