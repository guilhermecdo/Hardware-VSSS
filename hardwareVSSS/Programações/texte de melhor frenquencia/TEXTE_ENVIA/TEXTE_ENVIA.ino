#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

#define CE A1//define o pino 7 como CE
#define CS A0//define o pino 8 como CS

RF24 radio(CE,CS);
const uint64_t pipe[6]={0xF1F1F1F1E1LL,0xF0F0F0F0E2LL,0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL};// Endereços do radio
uint8_t data[2];
int i=0,q=0;

void setup(){
    radio.begin();
    radio.openWritingPipe(pipe[1]);
    radio.setPALevel(RF24_PA_MIN);
    radio.setChannel(0);
    radio.stopListening();

    Wire.begin();
}

void loop(){
    for(i=0;i<126;i++){
            Wire.beginTransmission(8);
            Wire.write(i);
            Wire.endTransmission();
            for(q=0;q<100;q++){
                radio.setChannel(i);
                radio.openWritingPipe(pipe[1]);
                radio.setPALevel(RF24_PA_MIN);
                radio.stopListening();
                data[0]=1;
                data[1]=q;
                radio.write(&data,sizeof(data));
                delay(33);
            }
    }
}
