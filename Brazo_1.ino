#define R1 26 //Eje Y
#define L1 27
#define R2 22//Base
#define L2 23 
#define Up 30//Art. Garra
#define Down 31
#define Left 28//Eje Extrusor Rotacional
#define Right 29
#define O 24 //Eje Z
#define X 25 
#define Triangle 34  //Reinicio
#define Square 38  //Garra
//#define AxisStatus 7
#define Auto  32 //Muestra la posicion de todos los ejes
#define BaseStep 52
#define BaseDir 53
#define YaxisStep 48
#define YaxisDir 49
#define XaxisStep 44
#define XaxisDir 45
#define ZaxisStep 50 //Este eje debe tener 1 driver individual para ambos motores, pero con los pines de senal en paralelo uno del otro
#define ZaxisDir 51
#define EaxisStep 46 
#define EaxisDir 47
#define Open 7 //Poner mismo valor que Close para Servomotor, senal PWM
#define Close 7 //Poner mismo valor que Open para Servomotor, senal PWM
//#define EmergencyDisable 10//OUTPUT Senal de Enable a los drivers
//#define ManualEnable 42 //INPUT Boton que desactiva la senal de enable

#define Joystick_Left 40
#define Joystick_Right 41
#define Joystick_Up 42
#define Joystick_Down 43

                                /*----------------------------------------No Cambiar-------------------------------------------*/ 
                                /*----------------------------------------No Cambiar-------------------------------------------*/                                
uint16_t b = 0;
uint16_t y = 0;
uint16_t x = 0;
uint16_t z = 0;
uint16_t e = 0;
volatile int16_t r = 0;

float AccelDelayZ = 1;
float AccelDelayB = 1;
float AccelDelayY = 1;
float AccelDelayX = 1;
float AccelDelayE = 1;

volatile int16_t ZaxisPosition = 0;
volatile int16_t BasePosition = 0;
volatile int16_t XaxisPosition = 0;
volatile int16_t YaxisPosition = 0;
volatile int16_t EaxisPosition = 0;

bool BaseInit = false;
volatile bool BaseIO;
uint32_t TimerBase = 0;
bool PostAccelB = false;

bool YInit = false;
volatile bool YaxisIO;
uint32_t TimerY = 0;
bool PostAccelY = false;

bool XInit = false;
volatile bool XaxisIO;
uint32_t TimerX = 0;
bool PostAccelX = false;

bool ZInit = false;
volatile bool ZaxisIO;
uint32_t TimerZ = 0;
bool PostAccelZ = false;

bool EInit = false;
volatile bool EaxisIO;
uint32_t TimerE = 0;
bool PostAccelE = false;

bool Grip = false;
bool Loop = false;
bool Joystick = false;
                                /*----------------------------------------No Cambiar-------------------------------------------*/ 
                                /*----------------------------------------No Cambiar-------------------------------------------*/
//Intervalo de pulsaciones ON/OFF
uint8_t IntervalIO = 30;
//Intervalo de pulsaciones ON/OFF


//Velocidad de los ejes
volatile uint16_t HzBaseTimer = 700;
volatile uint16_t HzYaxisTimer = 700;
volatile uint16_t HzXaxisTimer = 800;
volatile uint16_t HzZaxisTimer = 750;
volatile uint16_t HzEaxisTimer = 700;
//Velocidad de los ejes


//Cantidad de pasos por fase de aceleracion
uint16_t AccelStepsZ = 100;
uint16_t AccelStepsY = 100;
uint16_t AccelStepsX = 100;
uint16_t AccelStepsB = 100;
uint16_t AccelStepsE = 100;
//Cantidad de pasos por fase de aceleracion


//Cantidad de pasos por fase de desaceleracion
uint16_t DecelStepsZ = 60;
uint16_t DecelStepsY = 80;
uint16_t DecelStepsX = 50;
uint16_t DecelStepsB = 40;
uint16_t DecelStepsE = 20;
//Cantidad de pasos por fase de desaceleracion



/*int16_t analogReadWithDisable(uint8_t pin){
 ACSR |= (1<<ACD);
ADCSRA |= (1 << ADEN);

uint16_t value = analogRead(pin);

ADCSRA &=~ (1<<ADEN);
ACSR &=~ (1<<ACD);
}*/
void setup() {  cli();
/*ADCSRA &=~ (1<<ADEN);
ADCSRB &=~ (1<<ACME); 
ACSR &=~ (1<<ACD);
ACSR |= (1<<ACIE);
ACSR |= ((1<<ACIS0) | (1<<ACIS1));
*/
Serial.begin(9600);

pinMode(R2,INPUT_PULLUP);
pinMode(L2,INPUT_PULLUP);
pinMode(R1,INPUT_PULLUP);
pinMode(L1,INPUT_PULLUP);
pinMode(Up,INPUT_PULLUP);
pinMode(Down,INPUT_PULLUP);
pinMode(Left,INPUT_PULLUP);
pinMode(Right,INPUT_PULLUP);
pinMode(O,INPUT_PULLUP);
pinMode(X,INPUT_PULLUP);
pinMode(Square,INPUT_PULLUP);
pinMode(Triangle,INPUT_PULLUP);
pinMode(Auto,INPUT_PULLUP);
pinMode(Joystick_Left,INPUT_PULLUP);
pinMode(Joystick_Right,INPUT_PULLUP);
pinMode(Joystick_Down,INPUT_PULLUP);
pinMode(Joystick_Up,INPUT_PULLUP);

//pinMode(AxisStatus,INPUT_PULLUP);

//pinMode(ManualEnable,INPUT);


pinMode(BaseStep,OUTPUT);
pinMode(BaseDir,OUTPUT);
pinMode(YaxisStep,OUTPUT);
pinMode(YaxisDir,OUTPUT); 
pinMode(XaxisStep,OUTPUT);
pinMode(XaxisDir,OUTPUT); 
pinMode(ZaxisStep,OUTPUT);
pinMode(ZaxisDir,OUTPUT); 
pinMode(EaxisStep,OUTPUT);
pinMode(EaxisDir,OUTPUT); 
pinMode(Open,OUTPUT); 
pinMode(Close,OUTPUT);

//pinMode(EmergencyDisable,OUTPUT);
sei();
}

void loop() {
  // put your main code here, to run repeatedly:
uint32_t currentMicrosZ = micros();
uint32_t currentMicrosX = micros();
uint32_t currentMicrosB = micros();
uint32_t currentMicrosY = micros();
uint32_t currentMicrosE = micros();

/*if(digitalRead(ManualEnable)){
  digitalWrite(EmergencyDisable,LOW);
}*/
if(!digitalRead(Square)){
 
  if(!(Grip)){
     
analogWrite(Close, 500);
delay(500);
    Grip = true;
    return;
  }
   if(Grip){

analogWrite(Open,100);
delay(500);
    Grip = false;
  }
}


if(!digitalRead(Joystick_Up) || !digitalRead(Joystick_Down)){
Joystick = true;
HzXaxisTimer = 1000;
if(!digitalRead(Joystick_Up)){
  ZaxisIO = true;
  XaxisIO = true;
  YaxisIO = true;
}

if(!digitalRead(Joystick_Down)){
  ZaxisIO = false;
  XaxisIO = false;
  YaxisIO = false;
} 
}
else{
HzXaxisTimer = 800;  
Joystick = false;
}


                                /*----------------------------------------Eje Base-------------------------------------------*/ 
                                /*----------------------------------------Eje Base-------------------------------------------*/  



if(!digitalRead(R2) || !digitalRead(L2) || !digitalRead(Joystick_Left) || !digitalRead(Joystick_Right)){
  if(!digitalRead(L2) || !digitalRead(Joystick_Left)){
    BaseIO = true;

  }
  else if(!digitalRead(R2) || !digitalRead(Joystick_Right)){
    BaseIO = false;
   
    
  }
//Aceleracion
if(!(BaseInit)){
if(currentMicrosB - TimerBase >= (HzBaseTimer * AccelDelayB)){
TimerBase = currentMicrosB;  
if(BaseIO){
 BasePosition--;} //incremento de la posicion unsigned
 else{
  BasePosition++;
   }
if(b < AccelStepsB){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
b++; //conteo de pasos tipo aceleracion
AccelDelayB = 3;
 }

else if(b >= AccelStepsB && b < (AccelStepsB * 2)){
   digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
b++; //conteo de pasos tipo aceleracion
AccelDelayB = 2.5;
}

else if(b >= (AccelStepsB * 2) && b <= (AccelStepsB * 3)){
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
AccelDelayB = 2;
b++; //conteo de pasos tipo aceleracion
  }

else if(b >= (AccelStepsB * 3) && b <= (AccelStepsB * 4)){
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
AccelDelayB = 1.5;
b++; //conteo de pasos tipo aceleracion
  }
else if(b >= (AccelStepsB * 4) && b <= (AccelStepsB * 5)){
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
AccelDelayB = 1.2;
b++; //conteo de pasos tipo aceleracion
  }
 } 
}

//Velocidad Lineal

if(b >= ((AccelStepsB * 5) + 1)){
if(currentMicrosB - TimerBase >= HzBaseTimer){
TimerBase = currentMicrosB;  
BaseInit = true;

digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off

if(BaseIO){
 BasePosition--;} //incremento de la posicion unsigned
 else{
  BasePosition++;
   }
  }
 }
}
//Desaceleracion
if(digitalRead(R2) && digitalRead(L2) && b >= ((AccelStepsB * 5) + 1) || PostAccelB){
  PostAccelB = true;
  if(currentMicrosB - TimerBase >= (HzBaseTimer * AccelDelayB)){
     if(b >= ((AccelStepsB * 5) + 1)){
     b = 0;
     }
     if(BaseIO){
 BasePosition--;} //incremento de la posicion unsigned
 else{
  BasePosition++;
   }
     TimerBase = currentMicrosB;  
if(b < DecelStepsB){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
b++; //conteo de pasos tipo aceleracion
AccelDelayB = 1.2;

 }

else if(b >= DecelStepsB && b < (DecelStepsB * 2)){
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
b++; //conteo de pasos tipo aceleracion
AccelDelayB = 1.5;
}

else if(b >= (DecelStepsB * 2) && b <= (DecelStepsB * 3)){
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
AccelDelayB = 2;
b++; //conteo de pasos tipo aceleracion
  }

else if(b >= (DecelStepsB * 3) && b <= (DecelStepsB * 4)){
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
AccelDelayB = 2.5;
b++; //conteo de pasos tipo aceleracion
  }
else if(b >= (DecelStepsB * 4) && b <= (DecelStepsB * 5)){
  digitalWrite(BaseDir,BaseIO); //Giro horario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(BaseStep,LOW); //Step Off
AccelDelayB = 3;
b++; //conteo de pasos tipo aceleracion
   }
     if(b >= (DecelStepsB * 5)){
    PostAccelB = false;
  }
  }
 }

if((digitalRead(R2)) && (digitalRead(L2)) && !PostAccelB || digitalRead(Joystick_Left) && digitalRead(Joystick_Right) && !PostAccelB){
  BaseInit = false;
  b = 0;
}


                                /*----------------------------------------Eje Base-------------------------------------------*/ 
                                /*----------------------------------------Eje Base-------------------------------------------*/  


                                /*----------------------------------------Eje Y-------------------------------------------*/ 
                                /*----------------------------------------Eje Y-------------------------------------------*/  






if(!digitalRead(R1) || !digitalRead(L1)){
  if(!digitalRead(L1)){
    YaxisIO = true;

  }
  else if(!digitalRead(R1)){
    YaxisIO = false;

  }
//Aceleracion
if(!(YInit)){
if(currentMicrosY - TimerY >= (HzYaxisTimer * AccelDelayY)){
  if(YaxisIO){
 YaxisPosition--;} //incremento de la posicion unsigned
 else{
  YaxisPosition++;
   }
TimerY = currentMicrosY;  
if(y < AccelStepsY){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
y++; //conteo de pasos tipo aceleracion
AccelDelayY = 3;
 }

else if(y >= AccelStepsY && y < (AccelStepsY * 2)){
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
y++; //conteo de pasos tipo aceleracion
AccelDelayY = 2.5;
}

else if(y >= (AccelStepsY * 2) && y <= (AccelStepsY * 3)){
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
AccelDelayY = 2;
y++; //conteo de pasos tipo aceleracion
  }

else if(y >= (AccelStepsY * 3) && y <= (AccelStepsY * 4)){
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
AccelDelayY = 1.5;
y++; //conteo de pasos tipo aceleracion
  }
else if(y >= (AccelStepsY * 4) && y <= (AccelStepsY * 5)){
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
AccelDelayY = 1.2;
y++; //conteo de pasos tipo aceleracion
  }
 } 
}

//Velocidad Lineal

if(y >= ((AccelStepsY * 5) + 1)){
if(currentMicrosY - TimerY >= HzYaxisTimer){
TimerY = currentMicrosY;  
YInit = true;

digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off

if(YaxisIO){
 YaxisPosition--;} //incremento de la posicion unsigned
 else{
  YaxisPosition++;
   }
  }
 }
}

if(Joystick){
if(currentMicrosY - TimerY >= HzYaxisTimer){
TimerY = currentMicrosY;  
digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off

if(YaxisIO){
 YaxisPosition--;} //incremento de la posicion unsigned
 else{
  YaxisPosition++;
   }
  }
 }
 
//Desaceleracion
if(digitalRead(R1) && digitalRead(L1) && y >= ((AccelStepsY * 5) + 1) || PostAccelY){
  PostAccelY = true;
  if(currentMicrosY - TimerY >= (HzYaxisTimer * AccelDelayY)){
     if(y >= ((AccelStepsY * 5) + 1)){
     y = 0;
     }
     if(YaxisIO){
 YaxisPosition--;} //incremento de la posicion unsigned
 else{
  YaxisPosition++;
   }
     TimerY = currentMicrosY;  
if(y < DecelStepsY){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
y++; //conteo de pasos tipo aceleracion
AccelDelayY = 1.2;

 }

else if(y >= DecelStepsY && y < (DecelStepsY * 2)){
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
y++; //conteo de pasos tipo aceleracion
AccelDelayY = 1.5;
}

else if(y >= (DecelStepsY * 2) && y <= (DecelStepsY * 3)){
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
AccelDelayY = 2;
y++; //conteo de pasos tipo aceleracion
  }

else if(y >= (DecelStepsY * 3) && y <= (DecelStepsY * 4)){
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
AccelDelayY = 2.5;
y++; //conteo de pasos tipo aceleracion
  }
else if(y >= (DecelStepsY * 4) && y <= (DecelStepsY * 5)){
  digitalWrite(YaxisDir,YaxisIO); //Giro horario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(YaxisStep,LOW); //Step Off
AccelDelayY = 3;
y++; //conteo de pasos tipo aceleracion
   }
     if(y >= (DecelStepsY * 5)){
    PostAccelY = false;
  }
  }
 }

if((digitalRead(R1)) && (digitalRead(L1)) && !PostAccelY && !Joystick){
  YInit = false;
  y = 0;
}


                                /*----------------------------------------Eje Y-------------------------------------------*/ 
                                /*----------------------------------------Eje Y-------------------------------------------*/  


                                /*----------------------------------------Eje X-------------------------------------------*/ 
                                /*----------------------------------------Eje X-------------------------------------------*/                                 

if(!digitalRead(Up) || !digitalRead(Down) || Joystick){
  if(!digitalRead(Down)){
    XaxisIO = true;
  }
  else if(!digitalRead(Up)){
    XaxisIO = false;
  }
//Aceleracion
if(!(XInit)){
if(currentMicrosX - TimerX >= (HzXaxisTimer * AccelDelayX)){
  if(XaxisIO){
 XaxisPosition--;} //incremento de la posicion unsigned
 else{
  XaxisPosition++;
   }
TimerX = currentMicrosX;  
if(x < AccelStepsX){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
x++; //conteo de pasos tipo aceleracion
AccelDelayX = 3;
 }

else if(x >= AccelStepsX && x < (AccelStepsX * 2)){
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
x++; //conteo de pasos tipo aceleracion
AccelDelayX = 2.5;
}

else if(x >= (AccelStepsX * 2) && x <= (AccelStepsX * 3)){
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
AccelDelayX = 2;
x++; //conteo de pasos tipo aceleracion
  }

else if(x >= (AccelStepsX * 3) && x <= (AccelStepsX * 4)){
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
AccelDelayX = 1.5;
x++; //conteo de pasos tipo aceleracion
  }
else if(x >= (AccelStepsX * 4) && x <= (AccelStepsX * 5)){
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
AccelDelayX = 1.2;
x++; //conteo de pasos tipo aceleracion
  }
 } 
}

//Velocidad Lineal

if(x >= ((AccelStepsX * 5) + 1)){
if(currentMicrosX - TimerX >= HzXaxisTimer){
TimerX = currentMicrosX;  
XInit = true;

digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off

if(XaxisIO){
 XaxisPosition--;} //incremento de la posicion unsigned
 else{
  XaxisPosition++;
   }
  }
 }
}
//Desaceleracion
if(digitalRead(Up) && digitalRead(Down) && x >= ((AccelStepsX * 5) + 1) || PostAccelX){
  PostAccelX = true;
  if(currentMicrosX - TimerX >= (HzXaxisTimer * AccelDelayX)){
     if(x >= ((AccelStepsX * 5) + 1)){
     x = 0;
     }
     if(XaxisIO){
 XaxisPosition--;} //incremento de la posicion unsigned
 else{
  XaxisPosition++;
   }
     TimerX = currentMicrosX;  
if(x < DecelStepsX){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
x++; //conteo de pasos tipo aceleracion
AccelDelayX = 1.2;

 }

else if(x >= DecelStepsX && x < (DecelStepsX * 2)){
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
x++; //conteo de pasos tipo aceleracion
AccelDelayX = 1.5;
}

else if(x >= (DecelStepsX * 2) && x <= (DecelStepsX * 3)){
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
AccelDelayX = 2;
x++; //conteo de pasos tipo aceleracion
  }

else if(x >= (DecelStepsX * 3) && x <= (DecelStepsX * 4)){
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
AccelDelayX = 2.5;
x++; //conteo de pasos tipo aceleracion
  }
else if(x >= (DecelStepsX * 4) && x <= (DecelStepsX * 5)){
  digitalWrite(XaxisDir,XaxisIO); //Giro horario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(XaxisStep,LOW); //Step Off
AccelDelayX = 3;
x++; //conteo de pasos tipo aceleracion
   }
     if(x >= (DecelStepsX * 5)){
    PostAccelX = false;
  }
  }
 }

if((digitalRead(Up)) && (digitalRead(Down)) && !PostAccelX && !Joystick){
  XInit = false;
  x = 0;
}


                                /*----------------------------------------Eje X-------------------------------------------*/ 
                                /*----------------------------------------Eje X-------------------------------------------*/ 



                                /*----------------------------------------Eje Z-------------------------------------------*/ 
                                /*----------------------------------------Eje z-------------------------------------------*/ 



if(!digitalRead(O) || !digitalRead(X) || Joystick){
  if(!digitalRead(X)){
    ZaxisIO = true;
  }
 if(!digitalRead(O)){
    ZaxisIO = false;
  }
//Aceleracion
if(!(ZInit)){
if(currentMicrosZ - TimerZ >= (HzZaxisTimer * AccelDelayZ)){
  if(ZaxisIO){
 ZaxisPosition--;} //incremento de la posicion unsigned
 else{
  ZaxisPosition++;
   }
TimerZ = currentMicrosZ;  
if(z < AccelStepsZ){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
z++; //conteo de pasos tipo aceleracion
AccelDelayZ = 3;
 }

else if(z >= AccelStepsZ && z < (AccelStepsZ * 2)){
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
z++; //conteo de pasos tipo aceleracion
AccelDelayZ = 2.5;
}

else if(z >= (AccelStepsZ * 2) && z <= (AccelStepsZ * 3)){
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
AccelDelayZ = 2;
z++; //conteo de pasos tipo aceleracion
  }

else if(z >= (AccelStepsZ * 3) && z <= (AccelStepsZ * 4)){
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
AccelDelayZ = 1.5;
z++; //conteo de pasos tipo aceleracion
  }
else if(z >= (AccelStepsZ * 4) && z <= (AccelStepsZ * 5)){
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
AccelDelayZ = 1.2;
z++; //conteo de pasos tipo aceleracion
  }
 } 
}

//Velocidad Lineal

if(z >= ((AccelStepsZ * 5) + 1)){
if(currentMicrosZ - TimerZ >= HzZaxisTimer){
TimerZ = currentMicrosZ;  
ZInit = true;
  if(!digitalRead(X)){
    ZaxisIO = true;
  }
 if(!digitalRead(O)){
      ZaxisIO = false;
  }
digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off

if(ZaxisIO){
 ZaxisPosition--;} //incremento de la posicion unsigned
 else{
  ZaxisPosition++;
   }
  }
 }
}
//Desaceleracion
if(digitalRead(O) && digitalRead(X) && z >= ((AccelStepsZ * 5) + 1) || PostAccelZ){
  PostAccelZ = true;
  if(currentMicrosZ - TimerZ >= (HzZaxisTimer * AccelDelayZ)){
     if(z >= ((AccelStepsZ * 5) + 1)){
     z = 0;
     }
     if(ZaxisIO){
 ZaxisPosition--;} //incremento de la posicion unsigned
 else{
  ZaxisPosition++;
   }
     TimerZ = currentMicrosZ;  
if(z < DecelStepsZ){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
z++; //conteo de pasos tipo aceleracion
AccelDelayZ = 1.2;

 }

else if(z >= DecelStepsZ && z < (DecelStepsZ * 2)){
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
z++; //conteo de pasos tipo aceleracion
AccelDelayZ = 1.5;
}

else if(z >= (DecelStepsZ * 2) && z <= (DecelStepsZ * 3)){
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
AccelDelayZ = 2;
z++; //conteo de pasos tipo aceleracion
  }

else if(z >= (DecelStepsZ * 3) && z <= (DecelStepsZ * 4)){
  digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
AccelDelayZ = 2.5;
z++; //conteo de pasos tipo aceleracion
  }
else if(z >= (DecelStepsZ * 4) && z <= (DecelStepsZ * 5)){
digitalWrite(ZaxisDir,ZaxisIO); //Giro horario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(ZaxisStep,LOW); //Step Off
AccelDelayZ = 3;
z++; //conteo de pasos tipo aceleracion
   }
     if(z >= (DecelStepsZ * 5)){
    PostAccelZ = false;
  }
  }
 }

if((digitalRead(O)) && (digitalRead(X)) && !PostAccelZ && !Joystick){
  ZInit = false;
  z = 0;
}


                                /*----------------------------------------Eje Z-------------------------------------------*/ 
                                /*----------------------------------------Eje z-------------------------------------------*/ 


                                /*----------------------------------------Extrusor-------------------------------------------*/ 
                                /*----------------------------------------Extrusor-------------------------------------------*/ 



if(!digitalRead(Left) || !digitalRead(Right)){
  if(!digitalRead(Right)){
    EaxisIO = true;
  }
  else if(!digitalRead(Left)){
    EaxisIO = false;
  }
//Aceleracion
if(!(EInit)){
if(currentMicrosE - TimerE >= (HzEaxisTimer * AccelDelayE)){
  if(EaxisIO){
 EaxisPosition--;} //incremento de la posicion unsigned
 else{
  EaxisPosition++;
   }
TimerE = currentMicrosE;  
if(e < AccelStepsE){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
e++; //conteo de pasos tipo aceleracion
AccelDelayE = 3;
 }

else if(e >= AccelStepsE && e < (AccelStepsE * 2)){
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
e++; //conteo de pasos tipo aceleracion
AccelDelayE = 2.5;
}

else if(e >= (AccelStepsE * 2) && e <= (AccelStepsE * 3)){
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
AccelDelayE = 2;
e++; //conteo de pasos tipo aceleracion
  }

else if(e >= (AccelStepsE * 3) && e <= (AccelStepsE * 4)){
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
AccelDelayE = 1.5;
e++; //conteo de pasos tipo aceleracion
  }
else if(e >= (AccelStepsE * 4) && e <= (AccelStepsE * 5)){
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
AccelDelayE = 1.2;
e++; //conteo de pasos tipo aceleracion
  }
 } 
}

//Velocidad Lineal

if(e >= ((AccelStepsE * 5) + 1)){
if(currentMicrosE - TimerE >= HzEaxisTimer){
TimerE = currentMicrosE;  
EInit = true;

digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off

if(EaxisIO){
 EaxisPosition--;} //incremento de la posicion unsigned
 else{
  EaxisPosition++;
   }
  }
 }
}
//Desaceleracion
if(digitalRead(Left) && digitalRead(Right) && e >= ((AccelStepsE * 5) + 1) || PostAccelE){
  PostAccelE = true;
  if(currentMicrosE - TimerE >= (HzEaxisTimer * AccelDelayE)){
     if(e >= ((AccelStepsE * 5) + 1)){
     e = 0;
     }
     if(EaxisIO){
 EaxisPosition--;} //incremento de la posicion unsigned
 else{
  EaxisPosition++;
   }
     TimerE = currentMicrosE;  
if(e < DecelStepsE){ //cambiar este valor mejora la fluides, duracion  y seguridad de la aceleracion, aumentarlo no afecta los delays
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
e++; //conteo de pasos tipo aceleracion
AccelDelayE = 1.2;

 }

else if(e >= DecelStepsE && e < (DecelStepsE * 2)){
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
e++; //conteo de pasos tipo aceleracion
AccelDelayE = 1.5;
}

else if(e >= (DecelStepsE * 2) && e <= (DecelStepsE * 3)){
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
AccelDelayE = 2;
e++; //conteo de pasos tipo aceleracion
  }

else if(e >= (DecelStepsE * 3) && e <= (DecelStepsE * 4)){
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
AccelDelayE = 2.5;
e++; //conteo de pasos tipo aceleracion
  }
  
else if(e >= (DecelStepsE * 4) && e <= (DecelStepsE * 5)){
  digitalWrite(EaxisDir,EaxisIO); //Giro horario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(IntervalIO);
digitalWrite(EaxisStep,LOW); //Step Off
AccelDelayE = 3;
e++; //conteo de pasos tipo aceleracion
   }
     if(e >= (DecelStepsE * 5)){
    PostAccelE = false;
  }
  }
 }


if((digitalRead(Left)) && (digitalRead(Right)) && !PostAccelE){
  EInit = false;
  e = 0;
}


                                /*----------------------------------------Extrusor-------------------------------------------*/ 
                                /*----------------------------------------Extrusor-------------------------------------------*/ 


/*if(!digitalRead(AxisStatus)){
  Serial.println(BasePosition);
  Serial.println(ZaxisPosition);
  Serial.println(YaxisPosition);
  Serial.println(EaxisPosition);
  Serial.println(XaxisPosition);
  delay(500);
}*/

if(!digitalRead(Auto) || Loop){
//Mov 1
for(int n = 0; n < 4; n++){
Serial.println("Error");
Loop = true;
  for(r = 0; r < 131; r++){
    digitalWrite(XaxisDir, 0); //Giro antihorario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(HzXaxisTimer); //Frecuencia de pulsos Base
digitalWrite(XaxisStep,LOW); //Step Off
  }
  for(r = 0; r > -1091; r--){
    digitalWrite(ZaxisDir, 1); //Giro antihorario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(HzYaxisTimer); //Frecuencia de pulsos Base
digitalWrite(ZaxisStep,LOW); //Step Off
  }
  for(r = 0; r < 3150; r++){
    digitalWrite(YaxisDir,0); //Giro antihorario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(HzZaxisTimer); //Frecuencia de pulsos Base
digitalWrite(YaxisStep,LOW); //Step Off
  }
  analogWrite(Close,210);
//Mov 1
delay(1000);
//Mov 2
 for(r = 0; r > -131; r--){
    digitalWrite(XaxisDir, 1); //Giro antihorario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(HzXaxisTimer); //Frecuencia de pulsos Base
digitalWrite(XaxisStep,LOW); //Step Off
  }
  for(r = 0; r < 1091; r++){
    digitalWrite(ZaxisDir, 0); //Giro antihorario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(HzYaxisTimer); //Frecuencia de pulsos Base
digitalWrite(ZaxisStep,LOW); //Step Off
  }
  for(r = 0; r > -3150; r--){
    digitalWrite(YaxisDir,1); //Giro antihorario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(HzZaxisTimer); //Frecuencia de pulsos Base
digitalWrite(YaxisStep,LOW); //Step Off
  }
 
  for(r = 0; r > -4019; r--){
digitalWrite(BaseDir,1); //Giro antihorario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(HzBaseTimer); //Frecuencia de pulsos Base
digitalWrite(BaseStep,LOW); //Step Off
  }
//Mov 2
//Move 3
 for(r = 0; r < 131; r++){
    digitalWrite(XaxisDir, 0); //Giro antihorario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(HzXaxisTimer); //Frecuencia de pulsos Base
digitalWrite(XaxisStep,LOW); //Step Off
  }
  for(r = 0; r > -1091; r--){
    digitalWrite(ZaxisDir, 1); //Giro antihorario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(HzYaxisTimer); //Frecuencia de pulsos Base
digitalWrite(ZaxisStep,LOW); //Step Off
  }
  for(r = 0; r < 3218; r++){
    digitalWrite(YaxisDir,0); //Giro antihorario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(HzZaxisTimer); //Frecuencia de pulsos Base
digitalWrite(YaxisStep,LOW); //Step Off
  }
  delay(1000);
  analogWrite(Open,100);
  delay(500);
//Mov 3
//Mov 4
 for(r = 0; r > (-131 * 2); r--){
    digitalWrite(XaxisDir, 1); //Giro antihorario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(HzXaxisTimer); //Frecuencia de pulsos Base
digitalWrite(XaxisStep,LOW); //Step Off
  }
  for(r = 0; r < (1091 * 2); r++){
    digitalWrite(ZaxisDir, 0); //Giro antihorario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(HzYaxisTimer); //Frecuencia de pulsos Base
digitalWrite(ZaxisStep,LOW); //Step Off
  }
  for(r = 0; r > (-3150 * 2); r--){
    digitalWrite(YaxisDir,1); //Giro antihorario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(HzZaxisTimer); //Frecuencia de pulsos Base
digitalWrite(YaxisStep,LOW); //Step Off
  }

  analogWrite(Close,210);
  delay(1000);
//Mov 4
//Mov 5
for(r = 0; r < 131; r++){
    digitalWrite(XaxisDir, 0); //Giro antihorario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(HzXaxisTimer); //Frecuencia de pulsos Base
digitalWrite(XaxisStep,LOW); //Step Off
  }
  for(r = 0; r > -1091; r--){
    digitalWrite(ZaxisDir, 1); //Giro antihorario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(HzYaxisTimer); //Frecuencia de pulsos Base
digitalWrite(ZaxisStep,LOW); //Step Off
  }
  for(r = 0; r < 3218; r++){
    digitalWrite(YaxisDir,0); //Giro antihorario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(HzZaxisTimer); //Frecuencia de pulsos Base
digitalWrite(YaxisStep,LOW); //Step Off
  }

    for(r = 0; r < 4019; r++){
digitalWrite(BaseDir,0); //Giro antihorario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(HzBaseTimer); //Frecuencia de pulsos Base
digitalWrite(BaseStep,LOW); //Step Off
  }
//Mov 5
 delay(1000);
  analogWrite(Open,100);
  delay(500);
}
}


 if(!digitalRead(Triangle)){
  if(BasePosition > 0){
BaseIO = true;
  }
  else{
BaseIO = false;
BasePosition = BasePosition * -1;
  }
  if(ZaxisPosition > 0){
ZaxisIO = true;
  }
  else{
ZaxisIO = false;
ZaxisPosition = ZaxisPosition * -1;
  }
  if(YaxisPosition > 0){
YaxisIO = true;
  }
  else{
YaxisIO = false;
YaxisPosition = YaxisPosition * -1;
  }
  if(XaxisPosition > 0){
XaxisIO = true;
  }
  else{
XaxisIO = false;
XaxisPosition = XaxisPosition * -1;
  }
  if(EaxisPosition > 0){
EaxisIO = true;
  }
  else{
EaxisIO = false;
EaxisPosition = EaxisPosition * -1;
  }

  for(r = 0; r < BasePosition; r++){
digitalWrite(BaseDir,BaseIO); //Giro antihorario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(HzBaseTimer); //Frecuencia de pulsos Base
digitalWrite(BaseStep,LOW); //Step Off
  }
  for(r = 0; r < XaxisPosition; r++){
    digitalWrite(XaxisDir,XaxisIO); //Giro antihorario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(HzXaxisTimer); //Frecuencia de pulsos Base
digitalWrite(XaxisStep,LOW); //Step Off
  }
  for(r = 0; r < ZaxisPosition; r++){
    digitalWrite(ZaxisDir,ZaxisIO); //Giro antihorario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(HzYaxisTimer); //Frecuencia de pulsos Base
digitalWrite(ZaxisStep,LOW); //Step Off
  }
  for(r = 0; r < YaxisPosition; r++){
    digitalWrite(YaxisDir,YaxisIO); //Giro antihorario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(HzZaxisTimer); //Frecuencia de pulsos Base
digitalWrite(YaxisStep,LOW); //Step Off
  }
  for(r = 0; r < EaxisPosition; r++){
    digitalWrite(EaxisDir,EaxisIO); //Giro antihorario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(HzEaxisTimer); //Frecuencia de pulsos Base
digitalWrite(EaxisStep,LOW); //Step Off
  }
  BasePosition = 0;
  XaxisPosition = 0;
  ZaxisPosition = 0;
  YaxisPosition = 0;
  EaxisPosition = 0;
}
} //final loop

/*ISR(ANALOG_COMP_vect){

  if(BasePosition > 0){
BaseIO = false;
  }
  else{
BaseIO = true;
BasePosition = BasePosition * -1;
  }
  if(ZaxisPosition > 0){
ZaxisIO = false;
  }
  else{
ZaxisIO = true;
ZaxisPosition = ZaxisPosition * -1;
  }
  if(YaxisPosition > 0){
YaxisIO = false;
  }
  else{
YaxisIO = true;
YaxisPosition = YaxisPosition * -1;
  }
  if(XaxisPosition > 0){
XaxisIO = false;
  }
  else{
XaxisIO = true;
XaxisPosition = XaxisPosition * -1;
  }
  if(EaxisPosition > 0){
EaxisIO = false;
  }
  else{
EaxisIO = true;
EaxisPosition = EaxisPosition * -1;
  }

  for(r = 0; r < BasePosition; r++){
digitalWrite(BaseDir,BaseIO); //Giro antihorario
digitalWrite(BaseStep,HIGH); //Step On
delayMicroseconds(HzBaseTimer); //Frecuencia de pulsos Base
digitalWrite(BaseStep,LOW); //Step Off
  }
  for(r = 0; r < XaxisPosition; r++){
    digitalWrite(XaxisDir,XaxisIO); //Giro antihorario
digitalWrite(XaxisStep,HIGH); //Step On
delayMicroseconds(HzXaxisTimer); //Frecuencia de pulsos Base
digitalWrite(XaxisStep,LOW); //Step Off
  }
  for(r = 0; r < ZaxisPosition; r++){
    digitalWrite(ZaxisDir,ZaxisIO); //Giro antihorario
digitalWrite(ZaxisStep,HIGH); //Step On
delayMicroseconds(HzYaxisTimer); //Frecuencia de pulsos Base
digitalWrite(ZaxisStep,LOW); //Step Off
  }
  for(r = 0; r < YaxisPosition; r++){
    digitalWrite(YaxisDir,YaxisIO); //Giro antihorario
digitalWrite(YaxisStep,HIGH); //Step On
delayMicroseconds(HzZaxisTimer); //Frecuencia de pulsos Base
digitalWrite(YaxisStep,LOW); //Step Off
  }
  for(r = 0; r < EaxisPosition; r++){
    digitalWrite(EaxisDir,EaxisIO); //Giro antihorario
digitalWrite(EaxisStep,HIGH); //Step On
delayMicroseconds(HzEaxisTimer); //Frecuencia de pulsos Base
digitalWrite(EaxisStep,LOW); //Step Off
  }
  BasePosition = 0;
  XaxisPosition = 0;
  ZaxisPosition = 0;
  YaxisPosition = 0;
  EaxisPosition = 0;

  digitalWrite(EmergencyDisable, HIGH);
}*/