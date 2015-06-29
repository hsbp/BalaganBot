
#define LEFT_MOT 5	//Motor pin
#define RIGHT_MOT 6	//Motor pin
#define TOP_SPEED 254	//Max pwm kitoltes
#define ACTION_DURATION 300      //Meddig tart egy előre 
#define TURN_STEP 15      //PWM freki lépés, ha gazt kap ennyivel emeli a pwm frekvenciat

//gazpedal allasa [0, TOP_SPEED]
int left_Mot = 0;	
int right_Mot = 0;

//-------------------------
//-----Motor mozgatas------
void avanti();      //ELORE!!!!
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
int incomingByte;


//---------------
//----SHOW-------
//Szorakoztato eszkozok
typedef struct ShowTime{
  byte onOff[6];// = {0,0,0,0,0,0};
  int pin[6];
}ShowTime;
ShowTime Show{{0,0,0,0,0,0}, {3,9,10,11,12,13}};

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
    // if it's a capital H (ASCII 72), turn on the LED:
    switch (incomingByte){
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
      case 'e':
        stepper_Stand = head_Rotate(stepper_Stand, -1 * HEAD_rSTEPS); //Az aktualis   
        break;
      case 'q':
        stepper_Stand = head_Rotate(stepper_Stand, HEAD_rSTEPS); //Az aktualis   
        break;
      //---Motor---- 
      case 'w':
        avanti();
        //Serial.println(incomingByte);
        break;
      case 'a':
        turnLeft();
        break;
      case 's':
        lento();
        break;
      case 'd':
        turnRight();
        break;
      default:
        
        break;
    }
  }
  else{
    //Ha nincs bejovo utasitas lassul 
    lento();
  }
  analogWrite(LEFT_MOT, left_Mot);
  analogWrite(RIGHT_MOT, right_Mot);
  delay(ACTION_DURATION);
}


//-------------------------------
//--------MOVE-------------------

void avanti(){
  //Noveli a sebeseget
  //cucc = (a sebesseg korlaton belul maradok, ha gazt adok ? Ha igen, akkor gazt neki, : ha nem akkor max sebesseg)
  left_Mot = (left_Mot + TURN_STEP < TOP_SPEED ? left_Mot + TURN_STEP : TOP_SPEED);
  right_Mot = (right_Mot + TURN_STEP < TOP_SPEED ? right_Mot +TURN_STEP : TOP_SPEED);
  //Serial.println("AVANTI RAGAZZI");
  Serial.print(left_Mot, DEC);
  Serial.print("/t");
  Serial.print(right_Mot, DEC);
}

void lento(){
  //Csokkenti a sebesseget
  left_Mot = (left_Mot - TURN_STEP >= 0 ? left_Mot - TURN_STEP : 0);
  right_Mot = (right_Mot - TURN_STEP >= 0 ? right_Mot - TURN_STEP : 0);
}

void turnRight(){
  left_Mot = (left_Mot + TURN_STEP < TOP_SPEED ? left_Mot + TURN_STEP : TOP_SPEED);
  right_Mot = (right_Mot - TURN_STEP >= 0 ? right_Mot - TURN_STEP : 0);

}
void turnLeft(){
  left_Mot = (left_Mot - TURN_STEP >= 0 ? left_Mot - TURN_STEP : 0);
  right_Mot = (right_Mot + TURN_STEP < TOP_SPEED ? right_Mot +TURN_STEP : TOP_SPEED);
}

void jajj(){
  left_Mot = 0;
  right_Mot = 0;
  analogWrite(LEFT_MOT, left_Mot);
  analogWrite(RIGHT_MOT, right_Mot);
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

