#include <AccelStepper.h>
#include <EEPROM.h>
#define NUM_STEPS 200

int Config;

int reel[21]={0, 8, 10, 10, 10, 10, 8, 10, 10, 10, 8, 10, 10, 10, 10, 10, 8, 10, 8, 10, 10};

int GrapeL[21] = {1, 4, 6, 10, 13, 15, 18};
int GrapeC[21] = {3, 7, 11, 15, 19};
int GrapeR[21] = {3, 7, 11, 15, 19};

int NCGrapeL[5][5] = {{1, 4, 10, 13, 18}, {1, 10, 18}, {1, 6, 10, 15, 18}, {1, 4, 10, 13, 18}, {1, 6, 10, 15, 18}};

int RepL[21] = {3, 7, 12, 16, 19};
int RepC[21] = {1, 5, 9, 13, 17};
int RepR[21] = {4, 8, 12, 16, 20};

int NCRepL[5][5] = {{3, 12, 19}, {3, 7, 12, 16, 19}, {7, 16, 19}, {3, 12, 19}, {7, 16, 19}};

int ClownL[21] = {9, 20};
int ClownC[21] = {20};
int ClownR[21] = {5, 9, 13, 17};

int BellL[21] = {2, 11};
int BellC[21] = {4, 12};
int BellR[21] = {2, 6, 10, 14, 18};

int BarL[21] = {8, 17};
int BarC[21] = {8, 16};
int BarR[21] = {1};

int CherryL[21] = {5, 14};
int CherryC[21] = {2, 6, 10, 14, 18};

int NCCherryL[21] = {0, 1, 2, 3, 7, 8, 9, 10, 11, 12, 16, 18, 19, 20};

int PayLineDiffer[5][3] = {{-1, -1, -1}, {0, 0, 0}, {1, 1, 1}, {-1, 0, 1}, {1, 0, -1}};
int Conf_Table[7][2] = {{4, 1}, {3, 2}, {4, 2}, {5, 2}, {6, 2}, {8, 1}, {10, 2}};
AccelStepper Rstepper(AccelStepper::FULL4WIRE, 2, 3, 4, 5);
AccelStepper Lstepper(AccelStepper::FULL4WIRE, 10, 11, 12, A5);
AccelStepper Cstepper(AccelStepper::FULL4WIRE, 9, 8, 6, 7);
AccelStepper steppers[3] = {Lstepper, Cstepper, Rstepper};
int sensors[3] = {15, 16, 17};
int base = 184;
int target;
int mode;
int FreeSpin;
int phase;
int mod;
int reels[22];
bool status[3] = {false, false, false};
bool status2[3] = {false, false, false};
bool status3[3] = {false, false, false};
int timer[3] = {30, 30, 30};
int motor_pre[3] = {0, 0, 0};
int numReels = 3;
int targetValue[3] = {0, 0, 0};
int targetOfset[3] = {0, 0, 0};
int cmode = 0;
bool is_Calibrate = false;
bool temp = false;
bool is_wait = true;
bool is_first = true;
bool auto_play = false;
int Position[3] = {0, 0, 0};
int PositionDiff[3] = {0, 0, 0};
bool rotate[3] = {false, false, false};
int hoge = 0;

void setup()
{  
  Serial.begin(9600);
  Config = EEPROM.read(0);
  if(Config>7){
    Config = 1;
    EEPROM.write(0, Config);
  }
  for(int i=0; i<21; i++){
  reels[i] = sum(reel, 0, i);
  }
  for(int i=0; i<numReels; i++){
    pinMode(sensors[i], INPUT);
    pinMode(A4, INPUT_PULLUP);
    pinMode(A0, INPUT_PULLUP);
    steppers[i].setMaxSpeed(110);
    steppers[i].setAcceleration(500);
  }
  int r = analogRead(A7)+analogRead(A6) + analogRead(A7);
  randomSeed(sq(r));
  CalibrateReel();
}

void loop()
{
    if( !digitalRead(A4) & !temp){
        temp = true;
        timer[0] = 25;
        timer[1] = 25;
        timer[2] = 25;        
        for(int i=0; i<numReels; i++) {
              if(i==1){
                  steppers[i].STOP2(true, false, true, true);
                  delay(7);
                  if(Position[i]%4==0){
                    PositionDiff[i] = -1;
                  }
                  else{
                    PositionDiff[i] = 1;
                  }
              }
              else{
                  if(Position[i]%4==0){
                    PositionDiff[i] = 0;
                  }
                  else{
                    PositionDiff[i] = 2;
                  }
              }
              steppers[i].setCurrentPosition(0);
              steppers[i].setMaxSpeed(270);
              steppers[i].setAcceleration(5400);
              rotate[i] = true;
              status[i] = true;
              status3[i] = true;
          }
          
      }
    for(int i=0; i<numReels; i++){
        if(rotate[i]){
          steppers[i].move(100);
          steppers[i].run();
        }
        if(!steppers[i].run() & status[i]){
          status[i] = false;
          status2[i] = true;
        }
        else{
        if(!steppers[i].run()){
          if(status2[i]){
                if(timer[i]<0){
                  status2[i] = false;
                  status3[i] = false;
                  if(int(status3[0]) + int(status3[1]) + int(status3[i]) == 0){
                    temp = false;
                  }
                }
                timer[i] = timer[i] - 1;
            }
            if(!status[i] & status2[i]){
              if(i==1){
                  if(Position[i]%4==0){
                    steppers[i].STOP2(true, true, false, true);
                  }
                  else{
                     steppers[i].STOP2(true, true, true, false);
                  }
              }
              
            }
            else{
              steppers[i].STOP1();
            }
          }
        }
      }
}

void serialEvent(){
  int index = Serial.parseInt();
  int mt = Set_Position(0, 0, index);
  steppers[index].moveTo(mt + PositionDiff[index]);
  steppers[index].setMaxSpeed(270);
  steppers[index].setAcceleration(5400);
  Position[index] = mt;
  rotate[index] = false;
  while(Serial.available()){
    char t = Serial.read();
  }
}



int Set_Position(int flg, int PayLine, int ind){
    int position = (steppers[ind].currentPosition() - Position[ind])%200;
    if(190>position & sum(reel, 0, 14)<position){
      Serial.println("Good");
      return (steppers[ind].currentPosition()/200)*200 + 200;
    }
    else{
      for(int i=0; i<21; i++){
        if((steppers[ind].currentPosition()/200)*200 + reels[i + 1] - steppers[ind].currentPosition()>= 0){
          if((steppers[ind].currentPosition()/200)*200 + reels[i + 1] - steppers[ind].currentPosition() <= 5){
            if(i==19){
              return (steppers[ind].currentPosition()/200)*200 + reels[0] + 200;
            }
            if(i==20){
              return (steppers[ind].currentPosition()/200)*200 + reels[1] + 200;
            }
            else{
              return (steppers[ind].currentPosition()/200)*200 + reels[i + 2];
            }
          }
          else{
            if(i==20){
              return (steppers[ind].currentPosition()/200)*200 + reels[0] + 200;
            }
            else{
              return (steppers[ind].currentPosition()/200)*200 + reels[i + 1];
            }
          }
        }
      }
      
    }

}


void CalibrateReel(){
    is_Calibrate = true;
    for(int i=0; i<numReels; i++) {
      steppers[i].setCurrentPosition(0);
      steppers[i].move(NUM_STEPS);
      status[i] = true;
      status3[i] = true;
      while(digitalRead(sensors[i])){
        steppers[i].run();
      }
      unsigned long start = millis();
      while (millis() < start + 1000) {
        steppers[i].STOP2(true, true, true, true);
      }
      steppers[i].STOP1(); 
      steppers[i].setCurrentPosition(0);
      steppers[i].moveTo(base);
      steppers[i].setMaxSpeed(270);
      steppers[i].setAcceleration(3100);
    }
    is_Calibrate = false;
}

int sum(int arr[], int s1, int s2){
  int fooo = 0;
   for (int i = s1; i <= s2; i += 1) {
        fooo += arr[i];
    }
    return fooo;
}


int nsum(int base, int offset){
  if(base + offset >= 21){
    return 0;
  }
  if(base + offset <= -1){
    return 20;
  }
  else{
    return base + offset;
  }
}


bool chance(int top, int bottom){
  return random(0, 65536) <= (65536/bottom)*top;
}


