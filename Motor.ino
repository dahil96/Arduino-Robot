
#define RUN_FORWARD 0
#define STOP 1
#define LEFT 1
#define RIGHT 2
#define TURN_90 580
#define OBSTICLE 1
#define TIMEOUT 2

//static int state = 0;
const int sonarPin = 7;

// Setup for stopping and minimum distance to start
const unsigned int STOP_DIST = 10;
const unsigned int INIT_STOP_DIST = 10;
const unsigned int PROX_DIST = 20;
const long runTimeForward = 3000;
unsigned long currentTime = 0;
unsigned long startTime = 0;
int returnState = 0;

int state = 1;

void setup () {
  //Setup Channel A
  pinMode(12, OUTPUT);
  pinMode(9, OUTPUT);
  
  //Setup Channel B
  pinMode(13, OUTPUT);
  pinMode(8, OUTPUT);
  
  startTime = millis();
  
  //Parsing
  Serial.begin(9600);
}

void loop()  {
    while (state ==1 )
    {
      delay(2000);
      run_avoidance_with_timer(3000);
      run_if_chased(3);
      state == 0;
    }
    
}
/*
*
*/
unsigned int range_meassure() {
  unsigned int cm = 0;
  unsigned int duration = 0;
  
  //StartW rangeing
  pinMode(sonarPin, OUTPUT);
  digitalWrite(sonarPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sonarPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(sonarPin, LOW);
  //End rangeing
  
  pinMode(sonarPin, INPUT);
  duration = pulseIn(sonarPin, HIGH);
  cm = duration / 72 / 2 * 2.54;
  Serial.println(cm);
  return cm;
}

/*
*
*/
void scan_to_free() {
  //turn right 90 deg
  int localRunState = STOP;
  axis_turn(TURN_90, RIGHT);
  unsigned int range = range_meassure();
  //Verify that there is not an obsticle infront
  if (range > INIT_STOP_DIST) {
    localRunState = RUN_FORWARD;
  }
  //Run untill hit obsticle
  while (localRunState == STOP){
    axis_turn(TURN_90/10, LEFT);
    range = range_meassure();
    if (range > STOP_DIST ) {
      localRunState = RUN_FORWARD;
    }
    //Serial.println(state);
  }
}

/*
*
*/
void run_if_chased(int times) {
  
 unsigned int proximityRange = 0;  
  for (int i = 0; i<times; i++) {
    axis_turn(TURN_90*2, LEFT);
    //axist_turn(TURN_90, LEFT);
    proximityRange = range_meassure();
    while (proximityRange > PROX_DIST) {
      proximityRange = range_meassure();
    }
    axis_turn(TURN_90*2, LEFT);
    run_avoidance_with_timer(runTimeForward);
  }
}

/*
* Exit by obsticle or time out
*/

int run_and_break(unsigned long runTimer, unsigned long startTime ) {
  int localRunState = RUN_FORWARD;
  unsigned int range = range_meassure();
  unsigned long currentTime = millis ();
  unsigned long timeLeft = 0;
  int endState = 0;
  //Verify that there is not an obsticle infront
  if (range < INIT_STOP_DIST) {
    localRunState = STOP;
  } else {
    run_forward();
  }
  while (localRunState == RUN_FORWARD)
  {
    range = range_meassure();
    currentTime = millis();
    if( range < STOP_DIST || currentTime - startTime > runTimer) {
      localRunState = STOP;
      if (range < STOP_DIST) {
        endState = OBSTICLE;
      } else {
        endState = TIMEOUT;
      }
    }   
  }
  stop_driving();
  return endState;
}

/*
*
*/
void stop_driving(){
   //break
  digitalWrite(9, HIGH); //break channel A
  digitalWrite(8, HIGH); //break channel B 
}

/*
*
*/
void axis_turn(long turnTime, int dir){
  /*
  * 600 ms quarter of a turn
  */
  unsigned long currentTurnTime = 0;
  unsigned long previousTurnTime = 0;
  int turnState = 0;
  digitalWrite(9, LOW); //Release break
  digitalWrite(8, LOW); //Release break
  
  if(dir == LEFT) {
    //Enable channel A
    digitalWrite(12, HIGH); //Enables forward speed
    //Enable channel B
    digitalWrite(13, HIGH); //Enables forward speed
  } else {
    digitalWrite(12, LOW); //Enables forward speed
    //Enable channel B
    digitalWrite(13, LOW); //Enables forward speed
  }
  
  previousTurnTime = millis();
  while (turnState  == 0) {
    currentTurnTime = millis();
    if (currentTurnTime - previousTurnTime > turnTime)
    {
      //previousTurnTime = currentTurnTime;
      if (turnState == 0) {
        turnState = 1;
       }
     }
      //forward full speeed
  analogWrite(3, 200); //Spinn motor
  analogWrite( 11, 200); //Spinn motor
  }
  stop_driving();
}

void run_forward() {
  //Enable channel A
  digitalWrite(12, HIGH); //Enables forward speed
  digitalWrite(9, LOW); //Release break
  //Enable channel B
  digitalWrite(13, LOW); //Enables forward speed
  digitalWrite(8, LOW); //Release break
  
  //forward full speeed
  analogWrite(3, 255); //Spinn motor
  analogWrite( 11, 255); //Spinn motor
}

void run_avoidance_with_timer (long runTimeForward) {
    int returnState = 0;
    unsigned long startTime = millis();
    unsigned long currentTime = millis();
    //while (currentTime - startTime > runTimeForward || returnState == 0 ){
      //currentTime = millis();
      returnState = run_and_break(runTimeForward, startTime);
      if (returnState == OBSTICLE) {
        scan_to_free();
        Serial.println("scanfree");
      }
    //}
}


