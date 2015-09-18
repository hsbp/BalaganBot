#iclude <Stepper.h>

#define PIN_MOT_LEFT_FOR 5	//Motor pin
#define PIN_MOT_LEFT_BACK 6     //...
#define PIN_MOT_RIGHT_FOR 9     //...
#define PIN_MOT_RIGHT_BACK 10	//Motor pin

#define PIN_MOT_LEFT_DIRECTION 5
#define PIN_MOT_LEFT_PWM 6
#define PIN_MOT_RIGHT_DIRECTION 9
#define PIN_MOT_RIGHT_PWM 10

#define TOP_SPEED 254	//Max pwm kitoltes
#define ACTION_DURATION 50      //Meddig tart egy előre 
#define TURN_STEP 15      //PWM freki lépés, ha gazt kap ennyivel emeli a pwm frekvenciat

//----------------------
//---COMMAND CONSTANT---
//----------------------
#define COM_MASK B11100000       //Hatso 5 bitet ezzel csapjuk le
#define COM_MOT_LEFT_FOR 224     //B11100000
#define COM_MOT_LEFT_BACK 192    //B11000000
#define COM_MOT_RIGHT_FOR 160    //B10100000
#define COM_MOT_RIGHT_BACK 128   //B10000000
#define COM_OLD_II 96            //B01100000
#define COM_OLD_I 32             //B00100000




//gazpedal allasa [0, TOP_SPEED]
int speed_mot_left_for = 0;	
int speed_mot_left_back = 0;
int speed_mot_right_for = 0;
int speed_mot_right_back = 0;

int speed_mot_left = 0;
int speed_mot_right = 0;

//-------------------------
//-----Motor mozgatas------
void brumBrum(byte pin, byte speed);
void old(byte data);

void avanti();      //ELORE!!!!
void indietro();    //Hatramenet
void lento();        //LASSÍTS!!!!!
void turnRight();    //Jobbrafordul
void turnLeft();      //Ballra fordul
void jajj();        //Panik leallitas

//----------------
//-----FEJ--------
const int stepsPerRevolution = 200;	//Ennyi lepes alatt fordul korbe a fej
Stepper myStepper(stepsPerRevolution, 2,4,7,8);		//2, 4, 7, 8 pineken kapcsolodik a fejmozgato stepper
#define HEAD_SPEED 30      //fejforgas sebesseg
#define HEAD_rSTEPS 30      //hany lepest tegyen meg egy fordulas utasitasra

int stepper_Stand = 0;		//Hol all most a fej kerdest megvaloszolo valtozo

int head_Rotate(int state, int steps);	

//---------------
//--COMMUNICATION
byte incomingByte;


//---------------
//----SHOW-------
//Szorakoztato eszkozok
typedef struct ShowTime{
  byte onOff[6];// = {0,0,0,0,0,0};
  int pin[6];
}ShowTime;
ShowTime Show{{0,0,0,0,0,0}, {A0, A1, 3, 11, 12, 13}};

void show(int i);

void setup(){
  Serial.begin(9600);
  
  //Stepper motor sebesseg
  myStepper.setSpeed(HEAD_SPEED);

  for(int i = 3; i < 14; i++){
    pinMode(i, OUTPUT);
  }  
  
}

void loop(){
  if (Serial.available() > 0){
    // read the oldest byte in the serial buffer:
    incomingByte = Serial.read();
    switch (incomingByte & COM_MASK){
      case COM_MOT_LEFT_FOR:
        speed_mot_left_for = incomingByte << 3;
        speed_mot_left_back = 0;
        speed_mot_left = incomingByte << 3;
        digitalWrite(PIN_MOT_LEFT_DIRECTION, HIGH);
        analogWrite(PIN_MOT_LEFT_PWM, speed_mot_left);
//        brumBrum(PIN_MOT_LEFT_FOR, speed_mot_left_for);
        break;
      case COM_MOT_LEFT_BACK:
        speed_mot_left_back = (~incomingByte) << 3;
        speed_mot_left_for = 0;
        brumBrum(PIN_MOT_LEFT_BACK, speed_mot_left_back);
        break;
      case COM_MOT_RIGHT_FOR:
        speed_mot_right_for = incomingByte << 3;
        speed_mot_right_back = 0;
        brumBrum(PIN_MOT_RIGHT_FOR, speed_mot_right_for);
        break;
      case COM_MOT_RIGHT_BACK:
        speed_mot_right_back = (~incomingByte) << 3;
        speed_mot_right_for = 0;
        brumBrum(PIN_MOT_RIGHT_BACK, speed_mot_right_back);
        break;
      case COM_OLD_II:
        old(incomingByte);
        break;
      case COM_OLD_I:
        old(incomingByte);
        break;
    
    }
  }
  else{
    //Ha nincs bejovo utasitas lassul 
    lento();
  }
  analogWrite(PIN_MOT_LEFT_FOR, speed_mot_left_for);
  analogWrite(PIN_MOT_RIGHT_FOR, speed_mot_right_for);
  analogWrite(PIN_MOT_LEFT_BACK, speed_mot_left_back);
  analogWrite(PIN_MOT_RIGHT_BACK, speed_mot_right_back);
  delay(ACTION_DURATION);
}
//-----------------------------
//---------MOTOR---------------
void brumBrum(byte pin, byte speed){
  analogWrite(pin, speed);
}

//------------------------------
//---------OLD------------------
//------------------------------

void old(byte data){
  switch (data){
    case '0':
      show(Show.pin[0]);
      break;
    case '1':
      show(Show.pin[1]);
      break;
    case '2':
      show(Show.pin[2]);;
      break;  
    case '3':
      show(Show.pin[3]);
      break;
    case '4':
      show(Show.pin[4]);
      break;
    case '5':
      show(Show.pin[5]);
      break;
      //----HEAD-----
    case 'q':
      stepper_Stand = head_Rotate(stepper_Stand, -1 * HEAD_rSTEPS); //Az aktualis   
      break;
    case 'e':
      stepper_Stand = head_Rotate(stepper_Stand, HEAD_rSTEPS); //Az aktualis   
      break;
      //---Motor---- 
    case 'w':
      avanti();
      Serial.println(data);
      break;
    case 'a':
      turnLeft();
      break;
    case 's':
      indietro();
      break;
    case 'd':
      turnRight();
      break;
    case 'j':
      jajj(); 
      break;
    default:

      break;
  }
}


//-------------------------------
//--------MOVE-------------------

void avanti(){
  //Noveli a sebeseget
  //cucc = (a sebesseg korlaton belul maradok, ha gazt adok ? Ha igen, akkor gazt neki, : ha nem akkor max sebesseg)
  speed_mot_left_back = 0;
  speed_mot_right_back = 0;
  speed_mot_left_for = (speed_mot_left_for + TURN_STEP < TOP_SPEED ? speed_mot_left_for + TURN_STEP : TOP_SPEED);
  speed_mot_right_for = (speed_mot_right_for + TURN_STEP < TOP_SPEED ? speed_mot_right_for +TURN_STEP : TOP_SPEED);
  //Serial.println("AVANTI RAGAZZI");
  Serial.print(speed_mot_left_for, DEC);
  Serial.print("/t");
  Serial.print(speed_mot_right_for, DEC);
}
void indietro(){
  //Hatramenet
  speed_mot_left_for = 0;
  speed_mot_right_for = 0;
  speed_mot_left_back = (speed_mot_left_back + TURN_STEP < TOP_SPEED ? speed_mot_left_back + TURN_STEP : TOP_SPEED);
  speed_mot_right_back = (speed_mot_right_back + TURN_STEP < TOP_SPEED ? speed_mot_right_back +TURN_STEP : TOP_SPEED);
}

void lento(){
  //Csokkenti a sebesseget
  speed_mot_left_for = (speed_mot_left_for - TURN_STEP >= 0 ? speed_mot_left_for - TURN_STEP : 0);
  speed_mot_left_back = (speed_mot_left_back - TURN_STEP >= 0 ? speed_mot_left_back - TURN_STEP : 0);
  speed_mot_right_for = (speed_mot_right_for - TURN_STEP >= 0 ? speed_mot_right_for - TURN_STEP : 0);
  speed_mot_right_back = (speed_mot_right_back - TURN_STEP >= 0 ? speed_mot_right_back - TURN_STEP : 0);
}

void turnRight(){
  speed_mot_left_back = 0;
  speed_mot_right_back = 0;
  speed_mot_left_for = (speed_mot_left_for + TURN_STEP < TOP_SPEED ? speed_mot_left_for + TURN_STEP : TOP_SPEED);
  speed_mot_right_for = (speed_mot_right_for - TURN_STEP >= 0 ? speed_mot_right_for - TURN_STEP : 0);

}
void turnLeft(){
  speed_mot_left_back = 0;
  speed_mot_right_back = 0;
  speed_mot_left_for = (speed_mot_left_for - TURN_STEP >= 0 ? speed_mot_left_for - TURN_STEP : 0);
  speed_mot_right_for = (speed_mot_right_for + TURN_STEP < TOP_SPEED ? speed_mot_right_for +TURN_STEP : TOP_SPEED);
}

void jajj(){
  speed_mot_left_for = 0;
  speed_mot_right_for = 0;
  analogWrite(PIN_MOT_LEFT_FOR, speed_mot_left_for);
  analogWrite(PIN_MOT_RIGHT_FOR, speed_mot_right_for);
}

int head_Rotate(int state, int steps){
  if(state + steps > -stepsPerRevolution && state + steps  < stepsPerRevolution){
    state += steps;
    myStepper.step(steps);
  }
  return state;
}


void show(int i){
  //Be van-e kapcsolva?
  if (Show.onOff[i]){
    // be volt kapcsolva, kikapcs.
    Show.onOff[i] = 0;
    digitalWrite(i, LOW);
  }
  else{
    Show.onOff[i] = 1;
    digitalWrite(i, HIGH);
  }
}

