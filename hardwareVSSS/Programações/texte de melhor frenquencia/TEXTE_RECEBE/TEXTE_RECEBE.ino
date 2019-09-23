#include <SPI.h>      
#include <nRF24L01.h>        
#include <RF24.h>
#include <Wire.h>

#define CE A1//define o pino 7 como CE
#define CS A0 //define o pino 8 como CS

RF24 radio(CE,CS);
const uint64_t pipe[6]={0xF1F1F1F1E1LL,0xF0F0F0F0E2LL,0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL};// Endereços do radio
uint8_t data[2];
int i=0,q=0,id=1;

void setup(){
    Serial.begin(9600);

    radio.begin();
    radio.openReadingPipe(1,pipe[1]);
    radio.setPALevel(RF24_PA_MIN);
    radio.setChannel(0);
    radio.startListening();

    Wire.begin(8);
    Wire.onReceive(receiveEvent);
}

void loop(){
  if(radio.available()>0){
    radio.read(&data,sizeof(data));
    if(data[0]==id){
      q++;
    }
  }
  if(q>97){
    Serial.println(i);
  }
}

void receiveEvent(int howMany){
    q=0;
    int y=Wire.read();
    i=y;
    radio.setChannel(i);
    radio.openReadingPipe(1,pipe[1]);
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
}
