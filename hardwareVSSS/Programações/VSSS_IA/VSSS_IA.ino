#include <nRF24L01.h>
#include <RF24.h>
#include <TimerOne.h>

#define timer 10000//intervalo de tempo da interrupção interna (em micro segundos)
#define attempts 20//numero de tentaivas de leitura do radio antes do robô parar 

#define encoder_a_right 3//pino de interrupção 1 
#define encoder_b_right 1//
#define encoder_a_left 2//pino de interrupção 0
#define encoder_b_left 0//

#define CE A1//define o pino como CE
#define CS A0//define o pino como CS
#define channel 100//define o canal o radio 0 ate 125

#define enable_1 9
#define enable_2 6
#define in1 8
#define in2 7
#define in3 4
#define in4 5

#define s1 A4
#define s2 A5
s
int id;

int counter=0;
bool a=1; 

uint8_t data[5];
const uint64_t pipe[6]={0xF1F1F1F1E1LL,0xF0F0F0F0E2LL,0xF0F0F0F0E3LL,0xF0F0F0F0E4LL,0xF0F0F0F0E5LL,0xF0F0F0F0E6LL};//Endereços dos pipes do radio
double vx,v0,dx,d0;

double r=0.03;
double R=0.04;

double input_motor_1,output_motor_1,setpoint_motor_1;
double input_motor_2,output_motor_2,setpoint_motor_2;
double last_output_motor_1,last_output_motor_2;
double ax1,ax2;
double kp=0.03;//0.091039   
double ki=0.9;//0.338855

double current_error_m1=0,last_error_m1=0;
double current_error_m2=0,last_error_m2=0;

volatile long long int right=0;
volatile long long int last_right=0;
volatile long long int left=0;
volatile long long int last_left=0;

double velocitym1=0,velocitym2=0;
float ratio=6*51.45;

int pwm_out1,pwm_out2;

int state_enc_m1;
int last_state_enc_m1;

int state_enc_m2;
int last_state_enc_m2;

RF24 radio(CE,CS);//Define quais serão os pinos do arduino usados para conectar o CE e o CS no rádio

void setup(){
  pinMode(s1,INPUT_PULLUP);
  pinMode(s2,INPUT_PULLUP);
  pinMode(enable_1,OUTPUT);
  pinMode(enable_2,OUTPUT);
  pinMode(in1,OUTPUT);
  pinMode(in2,OUTPUT);
  pinMode(in3,OUTPUT);
  pinMode(in4,OUTPUT);
  pinMode(encoder_a_right,INPUT);
  pinMode(encoder_b_right,INPUT);
  pinMode(encoder_a_left,INPUT);
  pinMode(encoder_b_left,INPUT);
  
  Timer1.initialize(timer);//cria a interrupção interna e define o intervalo o qual ela ocorre
  Timer1.attachInterrupt(velocity);//cria a função para interrupção de tempo
  attachInterrupt(1,dirconta,CHANGE);//cria a interrupção externa do encoder direito
  attachInterrupt(0,esqconta,CHANGE);//cria a interrupção externa do encoder esquerdo

  radio.begin();//Inicia o rádio
  radio.openReadingPipe(1,pipe[0]);//define o pipe de comunicação
  radio.setPALevel(RF24_PA_MIN);//define o PA para minimo 
  radio.setChannel(channel);//Canal selecionado para ocorrer a transmição de dados de 0-125
  radio.startListening();//Começa a escutar

  identify();//função para definir o ID do robô
}

void loop(){
}

void velocity(){
  if(counter>attempts && a==1){
    setpoint_motor_1=0;
    setpoint_motor_2=0;
    analogWrite(enable_1,0);
    analogWrite(enable_2,0);
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
    a=0;
  }
  counter++;

  int pulses_right = right - last_right;
  int pulses_left = left - last_left;

  velocitym1=((2*PI*pulses_right)/(ratio*0.01));
  velocitym2=((2*PI*pulses_left)/(ratio*0.01));
  last_right=right;
  last_left=left;

  recebido();
}

void recebido(){
  if(radio.available()){
    radio.read(&data,sizeof(data));
    if(data[0]==id){
      counter=0;
      a=1;
      vx=data[1];
      v0=data[2];
      dx=data[3];
      d0=data[4];
      controle();
    }
  }
}

void controle (){
  vx=((1.5*vx)/180.0)*(dx-2);
  v0=((37.5*v0)/180.0)*(d0-2);
  ax1=((vx/r)+((v0*R)/r));
  ax2=((vx/r)-((v0*R)/r));

  if(ax1<0.0) setpoint_motor_1=-1*ax1;
  else setpoint_motor_1=ax1;
  if(ax2<0.0) setpoint_motor_2=-1*ax2;
  else setpoint_motor_2=ax2;

  if(velocitym1<0)input_motor_1=velocitym1*-1;
  else input_motor_1=velocitym1;

  if(velocitym2<0)input_motor_2=velocitym2*-1;
  else input_motor_2=velocitym2;

  if(setpoint_motor_1-input_motor_1<0) current_error_m1=(setpoint_motor_1-input_motor_1)*-1;
  else current_error_m1=setpoint_motor_1-input_motor_1;
  
  if(setpoint_motor_2-input_motor_2<0) current_error_m2=(setpoint_motor_2-input_motor_2)*-1;
  else current_error_m2=setpoint_motor_2-input_motor_2;
  
  output_motor_1=last_output_motor_1 + kp*(current_error_m1) + (ki*(last_error_m1));

  output_motor_2=last_output_motor_2 + kp*(current_error_m2) + (ki*(last_error_m2));

  pwm_out1=(output_motor_1*255)/55;
  pwm_out2=(output_motor_2*255)/55;

  last_output_motor_1=output_motor_1;
  last_output_motor_2=output_motor_2;

  last_error_m1=current_error_m1;
  last_error_m2=current_error_m2;

  motor();
}

void motor(){
  if(ax1>=0.0){
   analogWrite(enable_1,pwm_out1);
   digitalWrite(in1,HIGH);
   digitalWrite(in2,LOW);
  }
  else{
   analogWrite(enable_1,pwm_out1);
   digitalWrite(in1,LOW);
   digitalWrite(in2,HIGH);
  }
  if(ax2>=0.0){
   analogWrite(enable_2,pwm_out2);
   digitalWrite(in3,HIGH);
   digitalWrite(in4,LOW);
  }
  else{
   analogWrite(enable_2,pwm_out2);
   digitalWrite(in3,LOW);
   digitalWrite(in4,HIGH);
  }
}

void dirconta (){
  state_enc_m1=(digitalRead(encoder_a_right)<<1) | digitalRead(encoder_b_right);
  if(state_enc_m1==0 || state_enc_m1==3) right++;
  else if (state_enc_m1==1 || state_enc_m1==2) right--;
  last_state_enc_m1 = state_enc_m1;
}

void esqconta (){
  state_enc_m2=(digitalRead(encoder_a_left)<<1) | digitalRead(encoder_b_left);
  if(state_enc_m2==0 || state_enc_m2==3) left++;
  else if (state_enc_m2==1 || state_enc_m2==2) left--;
  last_state_enc_m2=state_enc_m2;
}

void identify(){
  if(digitalRead(s1)==0 && digitalRead(s2)==0) id=1;
  else if(digitalRead(s1)==1 && digitalRead(s2)==0) id=2;
  else if(digitalRead(s1)==0 && digitalRead(s2)==1) id=3;
  else if(digitalRead(s1)==1 && digitalRead(s2)==1) id=1;
}
