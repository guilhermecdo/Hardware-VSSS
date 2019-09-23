#include <PID_v1.h>
#include <TimerOne.h>

#define timer 10000 //intervalo de tempo da interrupção interna (em micro segundos)
#define encoderAdir 3 //pino de interrupção 1
#define pwm1 9
#define in1 8
#define in2 7

double input,output,setpoint;
double kp=0.091039;
double ki=0.338855;
//double ki=0.9698*(255/6.0);
//double kp=5.85;
//double ki=0.342;
double kd=0.0;
volatile unsigned long dir=0;
double velocitym1=0;
float ratio=3.0*51.45;
int pwm_out;

PID motor1(&input,&output,&setpoint,kp,ki,kd,DIRECT);

void setup(){
    pinMode(pwm1,OUTPUT);
    pinMode(in1,OUTPUT);
    pinMode(in2,OUTPUT);
    pinMode(encoderAdir,INPUT);

    Serial.begin(9600);
    Serial.setTimeout(200000);

    Timer1.initialize(timer);
    Timer1.attachInterrupt(vel);
    attachInterrupt(1,dirconta,RISING);

    motor1.SetMode(AUTOMATIC);
    motor1.SetOutputLimits(0,7.2);
    setpoint=55.0;
}

void loop(){
}

void vel(){
    velocitym1=((2*PI*dir)/(ratio*0.01));
    Serial.println(velocitym1);
    dir=0;
    controle();
}

void controle(){
    input=velocitym1;
    motor1.Compute();
    pwm_out=(255*output)/7.2;
    motor();
}

void motor(){
    analogWrite(pwm1,pwm_out);
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
}

void dirconta (){
    dir++;
}
