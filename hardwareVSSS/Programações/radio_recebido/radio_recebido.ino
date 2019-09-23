#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE A1 //define o pino como CE
#define CS A0 //define o pino como CS
#define pwm1 9
#define pwm2 6
#define in1 8
#define in2 7
#define in3 4
#define in4 5

RF24 radio(CE, CS);//Define quais serão os pinos do arduino usados para conectar o CE e o CS no rádio
int id;
bool S1;
bool S2;
int data[5];
int data2[5];
const uint64_t pipe[6]={0xF1F1F1F1E1LL,0xF0F0F0F0E2LL,0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL};//Endereços do radio

void setup(){
  pinMode(A4,INPUT_PULLUP);
  pinMode(A5,INPUT_PULLUP);
  pinMode(pwm1,OUTPUT);
  pinMode(pwm2,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  radio.begin();//Inicia o rádio
  radio.openReadingPipe(1,pipe[0]);
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(111);//Canal selecionado para ocorrer a transmição de dados de 0-125
  radio.startListening();//Começa a escutar
  //identifica();
  id=1;
}

void loop(){
  recebido();
  motor();
}

void identifica(){
  S1=digitalRead(A4);
  S2=digitalRead(A5);
  if(S1==0 && S2==0){
    id=1;
  }
  if(S1==1 && S2==0){
    id=2;
  }
  if(S1==0 && S2==1){
    id=3;
  }
    if(S1==1 && S2==1){
    id=1;
  }
}

void recebido(){
  if(radio.available()){
    radio.read(&data,sizeof(data));
      if(data[0]==id){
        data2[1]=data[1];
        data2[2]=data[2];
        data2[3]=data[3];
        data2[4]=data[4];
      }
  }
}

void motor (){
    switch (data2[3]){
      case 1:
        analogWrite(pwm1,0);
        digitalWrite(in1,LOW);
        digitalWrite(in2,LOW);
        break;
     case 2:
        analogWrite(pwm1,data2[1]);
        digitalWrite(in1,HIGH);
        digitalWrite(in2,LOW);
        break;
      case 3:
        analogWrite(pwm1,data2[1]);
        digitalWrite(in1,LOW);
        digitalWrite(in2,HIGH);
        break;  
      }
    switch (data2[4]){
      case 1:
       analogWrite(pwm2,0);
        digitalWrite(in3,LOW);
       digitalWrite(in4,LOW);
        break;
      case 2:
        analogWrite(pwm2,data2[2]);
        digitalWrite(in3,LOW);
        digitalWrite(in4,HIGH);
        break;
      case 3:
        analogWrite(pwm2,data2[2]);
        digitalWrite(in3,HIGH);
        digitalWrite(in4,LOW);
        break;
    }
}
