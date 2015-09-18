#include <Stepper.h>

int speed_mot_left = 0;
byte direction_mot_left = 1;	//1-forward, 0-backward
int speed_mot_right = 0;
byte direction_mot_right = 1;	//1-forward, 0-backward

#define PIN_MOT_LEFT_DIRECTION 7		//HIGH-forward, LOW-backward
#define PIN_MOT_LEFT_PWM 5
#define PIN_MOT_RIGHT_DIRECTION 8	//HIGH-forward, LOW-backward
#define PIN_MOT_RIGHT_PWM 6

#define TURN_STEP 15      //PWM freki lépés, ha gazt kap ennyivel emeli a pwm frekvenciat
#define TOP_SPEED 255
#define ACTION_DURATION 50
void lento();		//break

//----------------
//-----HEAD--------
const int stepsPerRevolution = 200;     //Ennyi lepes alatt fordul korbe a fej
Stepper myStepper(stepsPerRevolution, 10,11,12,13);         //2, 8, 12, 13 pineken kapcsolodik a fejmozgato stepper
#define HEAD_SPEED 30      //fejforgas sebesseg
#define HEAD_rSTEPS 30      //hany lepest tegyen meg egy fordulas utasitasra

int stepper_Stand = 0;          //Hol all most a fej kerdest megvaloszolo valtozo

int head_Rotate(int state, int steps);

//---------------
//--COMMUNICATION
byte incomingByte;

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
//---------------
//----SHOW-------
//Szorakoztato eszkozok
typedef struct ShowTime{
 	byte onOff[6];// = {0,0,0,0,0,0};
	int pin[6];
}ShowTime;
ShowTime Show{{0,0,0,0,0,0}, {2,3,4,9,A0,A1}};

void show(int i);

//----OLD-----
void old(byte data);



void setup(){
	Serial.begin(9600);

	myStepper.setSpeed(HEAD_SPEED);
}


void loop(){
	if (Serial.available() > 0){
		incomingByte = Serial.read();
		switch (incomingByte & COM_MASK){
			case COM_MOT_LEFT_FOR:
				speed_mot_left = incomingByte << 3;
				direction_mot_left = 1;	//1-forward, 0-backward
				break;
			case COM_MOT_LEFT_BACK:
				speed_mot_left = (~incomingByte) << 3;
				direction_mot_left = 0;	//1-forward, 0-backward
				break;
			case COM_MOT_RIGHT_FOR:
				speed_mot_right = incomingByte << 3;
				direction_mot_right = 1;	//1-forward, 0-backward
				break;
			case COM_MOT_RIGHT_BACK:
				speed_mot_right = (~incomingByte) << 3;
				direction_mot_right = 0;	//1-forward, 0-backward
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
		lento();
	}
	Serial.print(direction_mot_left, DEC);	Serial.print("_");	Serial.print(speed_mot_left, DEC); Serial.print("\t\t"); Serial.print(direction_mot_right, DEC); Serial.print("_"); Serial.println(speed_mot_right, DEC);
	digitalWrite(PIN_MOT_LEFT_DIRECTION, direction_mot_left);
	analogWrite(PIN_MOT_LEFT_PWM, speed_mot_left);
	digitalWrite(PIN_MOT_RIGHT_DIRECTION, direction_mot_right);
	analogWrite(PIN_MOT_RIGHT_PWM, speed_mot_right);
	delay(ACTION_DURATION);
}

void lento(){
	//cucc = (a sebesseg korlaton belul maradok, ha gazt adok ? Ha igen, akkor gazt neki, : ha nem akkor max sebesseg)
	speed_mot_left = (speed_mot_left - TURN_STEP >= 0 ? speed_mot_left - TURN_STEP: 0);
	speed_mot_right = (speed_mot_right - TURN_STEP >= 0 ? speed_mot_right - TURN_STEP: 0);
}
void old(byte data){
  switch (data){
    case '0':
      show(Show.pin[0]);
      break;
    case '1':
      show(Show.pin[1]);
      break;
    case '2':
      show(Show.pin[2]);
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
			direction_mot_left = 1; direction_mot_right = 1;
			speed_mot_left = (speed_mot_left + TURN_STEP < TOP_SPEED ? speed_mot_left + TURN_STEP : TOP_SPEED);
			speed_mot_right = speed_mot_left;
      break;
    case 'a':
			direction_mot_left = 1; direction_mot_right = 1;
			speed_mot_left = (speed_mot_left - TURN_STEP >= 0 ? speed_mot_left - TURN_STEP : 0);
			speed_mot_right = (speed_mot_right + TURN_STEP < TOP_SPEED ? speed_mot_right + TURN_STEP : TOP_SPEED);
      break;
    case 's':
			direction_mot_left = 0; direction_mot_right = 0;
			speed_mot_left = (speed_mot_left + TURN_STEP < TOP_SPEED ? speed_mot_left + TURN_STEP : TOP_SPEED);
			speed_mot_right = speed_mot_left;
      break;
    case 'd':
			direction_mot_left = 1; direction_mot_right = 1;
			speed_mot_left = (speed_mot_left + TURN_STEP < TOP_SPEED ? speed_mot_left + TURN_STEP : TOP_SPEED);
			speed_mot_right = (speed_mot_right - TURN_STEP >= 0 ? speed_mot_right - TURN_STEP : 0);
      break;
    case 'j':
			direction_mot_left = 1; direction_mot_right = 1;
			speed_mot_left = 0; speed_mot_right = 0;
      break;
    default:

      break;
  }
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


