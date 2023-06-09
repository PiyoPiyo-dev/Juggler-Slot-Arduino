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
int Conf_Table[6][2] = {{3, 1}, {2, 1}, {3, 3}, {1, 2}, {0, 1}, {0, 4}};
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
int timer[3] = {25, 25, 25};
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
int payline;

static uint32_t x = 123456789;
static uint32_t y = 362436069;
static uint32_t z = 521288629;
static uint32_t w = 88675123;
static uint32_t _x = 123456789;
static uint32_t _y = 362436069;
static uint32_t _z = 521288629;
static uint32_t _w = 88675123;

int hoge = 0;
void setup()
{  
  Serial.begin(9600);
  Serial.print("9999,99,99,99");
  Config = EEPROM.read(0);
  if(Config>6){
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
    steppers[i].setAcceleration(1100);
  }
  int r = analogRead(A7)+analogRead(A6) + analogRead(A7);
  randomSeed(sq(r));
  int t = analogRead(A7)+analogRead(A6) + analogRead(A7);
  while(true){
    if(t==r){
      t = analogRead(A7)+analogRead(A6) + analogRead(A7);
    }
    else{
      break;
    }
  }
  xorshift128_seed(sq(t) + analogRead(A7) + analogRead(A6) + analogRead(A7));
  CalibrateReel();
}

void loop()
{
    xorshift128();
    _xorshift128();
    if(!digitalRead(A0) && !temp) {
      if(cmode==2){
        unsigned long int t = millis();
        while(!digitalRead(A0)){
          delay(5);
        }
        if(millis() - t >=1000){
            digitalWrite(13, LOW);
            delay(1000);
            flash(10);
            int r = analogRead(A7)+analogRead(A6) + analogRead(A7);
            randomSeed(sq(r));
            r = analogRead(A7)+analogRead(A6) + analogRead(A7);
            xorshift128_seed(sq(r) + analogRead(A7) + analogRead(A6) + analogRead(A7));
            mode = 0;
            target = 0;
            FreeSpin = 0;
            cmode = 0;
            Config = 6;
        }
        else{
          //mod=1 通常モード
          //mod=2 次ゲームREP確定
          //mod=3 次ゲームRB確定
          //mod=4 次ゲームBB確定
          //mod=5 次ゲームSB確定
          mod += 1;
          if(mod==6){
            mod = 1;
          }
          Show_Config(mod);
          digitalWrite(13, HIGH);
        }
      }
      else
      if(cmode==1){
        unsigned long int t = millis();
        while(!digitalRead(A0)){
          delay(5);
        }
        if(millis() - t >=1000){
          if(Config==7){
            cmode = 2;
            mod = 1;
            flash(5);
            digitalWrite(13, HIGH);
          }
          else{
            EEPROM.write(0, Config);
            digitalWrite(13, LOW);
            delay(1000);
            flash(10);
            int r = analogRead(A7)+analogRead(A6) + analogRead(A7);
            randomSeed(sq(r));
            r = analogRead(A7)+analogRead(A6) + analogRead(A7);
            xorshift128_seed(sq(r) + analogRead(A7) + analogRead(A6) + analogRead(A7));
            r = analogRead(A7)+analogRead(A6) + analogRead(A7);
            _xorshift128_seed(sq(r) + analogRead(A7) + analogRead(A6) + analogRead(A7));
            mode = 0;
            target = 0;
            FreeSpin = 0;
            cmode = 0;
          }
        }
        else{
          Config += 1;
          if(Config == 7){
            flash(20);
            digitalWrite(13, HIGH);
          }
          else{
            if(Config == 8){
              Config = 1;
            }
            Show_Config(Config);
            digitalWrite(13, HIGH);
          }       
        }
      }
      else{
        unsigned long int t = millis();
        while(!digitalRead(A0)){
          delay(5);
        }
        if(millis() - t >=1000){
          cmode = 1;
          digitalWrite(13, HIGH);
        }
        else{
          Show_Config(Config);
        }
      }
      
    }

    if( (!digitalRead(A4) && !temp && cmode==0) || (auto_play && !temp && cmode==0)){
          payline = random(0, 5);
          //mode = 0 通常時
          //mode = 1 BB
          //mode = 2 RB
          //mode = 3 SB
          //ゲーム性
          //通常時子役カット
          //1/10でボーナス抽選
          //BB 1/2(獲得 4G 24枚)
          //RB 2/5(獲得 2G 12枚)
          //SB 1/10(獲得 4G + 上乗せ 平均獲得 13G)
          //SBの上乗せについて 残り1Gで抽選->追加4G獲得(継続率83%)
          //設定  継続率 REP率 機械割
          //設定1   60%   18%    90%
          //設定2   70%   18%    96%
          //設定3   60%   36%   106%
          //設定4   80%   27%   113%
          //設定5   90%   18%   132%
          //設定6   90%   45%   157%
          if(!auto_play){
            unsigned long int t = millis();
            while(!digitalRead(A4)){
              delay(5);
            }
            if(millis() - t >=1000){
              auto_play = true;
            }
          }
          else{
            if(!digitalRead(A4)){
              auto_play = false;
              while(!digitalRead(A4)){
                delay(5);
              }
              return;
            }
            else{
              for(int i=0; i<2500; i++){
                delay(1);
                xorshift128();
                _xorshift128();
              }
              
            }
          }
          if(mode==0){
            int random_flg = shift128_random();
            if(mod==1){
              if(random_flg==0){
                random_flg = _shift128_random();
                if(random_flg <= 4){
                  //BB当選
                  target = 7;
                  FreeSpin = 4;
                  mode = 1;
                }
                else
                if(random_flg != 5){
                  //RB当選
                  target = 6;
                  FreeSpin = 2;
                  mode = 2;
                }
                else{
                  //SB当選
                  target = 8;
                  FreeSpin = 4;
                  mode = 3;
                }
              }
              else
              if(random_flg <=Conf_Table[Config-1][1]){
                //REP当選
                target = 3;
                FreeSpin = 0;
              }
              else{
                  target = 0;
                  FreeSpin = 0;
                }
            }
            else{
              switch(mod){
                case 2:
                  target = 3;
                  FreeSpin = 0;
                  mode = 0;
                  break;
                case 3:
                  target = 6;
                  FreeSpin = 2;
                  mode = 2;
                  break;
                case 4:
                  target = 7;
                  FreeSpin = 4;
                  mode = 1;
                  break;
                case 5:
                  target = 8;
                  FreeSpin = 4;
                  mode = 3;
                  break;
              }
              mod = 1;
            }
          }
          else
          if(mode == 1){
            FreeSpin -= 1;
            if(FreeSpin == 0){
              mode=0;
            }
            target = 1;
          }
          else
          if(mode == 2){
            FreeSpin -= 1;
            if(FreeSpin == 0){
              mode=0;
            }
            target = 1;
          }
          else
          if(mode == 3){
            FreeSpin -= 1;
            if(FreeSpin == 0){
              if(shift128_random() <=Conf_Table[Config-1][0]){
                FreeSpin = 4;
                target = 8;
              }
              else{
                mode=0;
              }
            }
            else{
              target = 1;
            }          
          }    
          targetValue[0] = Set_Position(target, payline, 0);
          targetValue[1] = Set_Position(target, payline, 1);
          targetValue[2] = Set_Position(target, payline, 2);
          timer[0] = 25;
          timer[1] = 25;
          timer[2] = 25;
          temp = true;
          is_wait = false;
          steppers[1].STOP2(true, false, true, true);
          delay(7);
          is_wait = true;
          for(int i=0; i<numReels; i++) {
              steppers[i].setCurrentPosition(0);
              steppers[i].STOP1();
              if(i==1){
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
          }
          for(int i=0; i<numReels; i++) {
            if(auto_play){
              targetOfset[i] = NUM_STEPS*2 +  (motor_pre[i])- reels[targetValue[i]] + PositionDiff[i];
            }
            else{
              targetOfset[i] = NUM_STEPS*i + NUM_STEPS*3 + (motor_pre[i])- reels[targetValue[i]] + PositionDiff[i];
            }       
          }
          if(targetOfset[1] - targetOfset[0] < 0 && !auto_play){
              targetOfset[0] -= 200;
          }          
          if(targetOfset[1] - targetOfset[0] < 70 && !auto_play){
              targetOfset[1] += 200;
              targetOfset[2] += 200;
          }
          if(targetOfset[2] - targetOfset[1] < 0 && !auto_play){
              targetOfset[1] -= 200;
          }
          if(targetOfset[2] - targetOfset[1] < 70  && !auto_play){
            targetOfset[2] += 200;
          }
          if(targetOfset[2] - targetOfset[1] > 270 && !auto_play){
              targetOfset[2] -= 200;
          }
          for(int i=0; i<numReels; i++) {
            steppers[i].moveTo(targetOfset[i]);
            Position[i] = reels[targetValue[i]];
            motor_pre[i] = reels[targetValue[i]];
            status[i] = true;
            status3[i] = true;
          }
          
      }

      for(int i=0; i<numReels; i++){
        if(int(status3[0]) + int(status3[1]) + int(status3[2]) != 0){
          if(!steppers[i].run() && status[i]){
            status[i] = false;
            status2[i] = true;
          }
          else{
            if(!steppers[i].run()){
              if(status2[i]){
                  if(timer[i]<=0){
                    status2[i] = false;
                    status3[i] = false;
                    steppers[i].STOP1();
                    if(int(status3[0]) + int(status3[1]) + int(status3[2]) == 0){
                      if(is_first){
                        is_first = false;
                      }
                      else{
                        //1 モード
                        //2 フリースピン残り回数
                        //3 当選状況
                        //4 ペイライン
                        //5-13 停止状況
                        Serial.print(mode);
                        Serial.print(FreeSpin);
                        Serial.print(target);
                        Serial.print(payline);
                        Serial.print(",");
                        Serial.print(targetValue[0]);
                        Serial.print(",");
                        Serial.print(targetValue[1]);
                        Serial.print(",");
                        Serial.print(targetValue[2]);
                        temp = false;
                      }
                      
                    }
                  }
                  timer[i] = timer[i] - 1;
              }
              if(!status[i] && status2[i] && is_wait){
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
          if(!is_wait){
            steppers[i].STOP1();
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
      steppers[i].setMaxSpeed(270.7092);
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

int Set_Position(int flg, int PayLine, int index){
    //flg = 0 何も当選しなかった
    //flg = 1 ブドウ当選
    //flg = 2 単チェリー当選
    //flg = 3 リプレイ当選
    //flg = 4 ピエロ当選
    //flg = 5 ベル当選
    //flg = 6 RB当選
    //flg = 7 BB当選
    //flg = 8 SB当選
    int seven[5] = {20, 0, 1, 1, 20};
    //PayLine = 0 下
    //PayLine = 1 中
    //PayLine = 2 上
    //PayLine = 3 傾き正
    //PayLine = 4 傾き負
    if(flg==0){
      switch(index){
        case 0:
          return NCCherryL[random(0, 14)];   
        case 1:
          return random(0, 21);
        case 2:
          int target[5][2] = {
            {index2id(nsum(targetValue[0], -1), 0), index2id(nsum(targetValue[1], -1), 1)},
            {index2id(nsum(targetValue[0], 0), 0), index2id(nsum(targetValue[1], 0), 1)},
            {index2id(nsum(targetValue[0], 1), 0), index2id(nsum(targetValue[1], 1), 1)},
            {index2id(nsum(targetValue[0], 1), 0), index2id(nsum(targetValue[1], 0), 1)},
            {index2id(nsum(targetValue[0], -1), 0), index2id(nsum(targetValue[1], 0), 1)}
          };
          if(target[0][0] == target[0][1]){
              return nsum(get_index(target[0][0]), 1);
          }
          if(target[1][0] == target[1][1]){
              return nsum(get_index(target[1][0]), 0);
          }
          if(target[2][0] == target[2][1]){
              return nsum(get_index(target[2][0]), -1);
          }
          if(target[3][0] == target[3][1]){
              return nsum(get_index(target[3][0]), 1);
          }
          if(target[4][0] == target[4][1]){
              return nsum(get_index(target[4][0]), -1);
          }
          else{
            return random(0, 21);
          }
      }
    }
    if(flg==1){
      int ind = 5;
      if(PayLine==1){
        ind = 3;
      }
      switch(index){
        case 0:
          return diffsum(NCGrapeL[PayLine][random(0, ind)], PayLine, 0);
        case 1:
          return diffsum(GrapeC[random(0, 5)], PayLine, 1);
        case 2:
          return diffsum(GrapeR[random(0, 5)], PayLine, 2);
      }
    }
    if(flg==2){
      switch(index){
        case 0:
          return NCCherryL[random(0, 14)];
        case 1:
          return random(0, 21);
        case 2:
          int target[5][2] = {
            {index2id(nsum(targetValue[0], -1), 0), index2id(nsum(targetValue[1], -1), 1)},
            {index2id(nsum(targetValue[0], 0), 0), index2id(nsum(targetValue[1], 0), 1)},
            {index2id(nsum(targetValue[0], 1), 0), index2id(nsum(targetValue[1], 1), 1)},
            {index2id(nsum(targetValue[0], 1), 0), index2id(nsum(targetValue[1], 0), 1)},
            {index2id(nsum(targetValue[0], -1), 0), index2id(nsum(targetValue[1], 0), 1)}
          };
          if(target[0][0] == target[0][1]){
              return nsum(get_index(target[0][0]), 1);
          }
          if(target[1][0] == target[1][1]){
              return nsum(get_index(target[1][0]), 0);
          }
          if(target[2][0] == target[2][1]){
              return nsum(get_index(target[2][0]), -1);
          }
          if(target[3][0] == target[3][1]){
              return nsum(get_index(target[3][0]), 1);
          }
          if(target[4][0] == target[4][1]){
              return nsum(get_index(target[4][0]), -1);
          }
      }
    }
    if(flg==3){
      int ind = 3;
      if(PayLine==1){
        ind = 5;
      }
      switch(index){
        case 0:
          return diffsum(NCRepL[PayLine][random(0, ind)], PayLine, 0);
        case 1:
          return diffsum(RepC[random(0, 5)], PayLine, 1);
        case 2:
          return diffsum(RepR[random(0, 5)], PayLine, 2);
      }
    }
    if(flg==4){
      switch(index){
        case 0:
          return diffsum(ClownL[random(0, 2)], PayLine, 0);
        case 1:
          return diffsum(ClownC[0], PayLine, 1);
        case 2:
          return diffsum(ClownR[random(0, 4)], PayLine, 2);
      }
    }
    if(flg==5){
      switch(index){
        case 0:
          return diffsum(BellL[random(0, 2)], PayLine, 0);
        case 1:
          return diffsum(BellC[random(0, 2)], PayLine, 1);
        case 2:
          return diffsum(BellR[random(0, 5)], PayLine, 2);
      }
    }
    if(flg==6){
      switch(index){
        case 0:
          return diffsum(BarL[random(0, 2)], PayLine, 0);
        case 1:
          return diffsum(BarC[random(0, 2)] , PayLine, 1);
        case 2:
          return diffsum(0, PayLine, 2);
      }
    }
    if(flg==7){
      switch(index){
        case 0:
          return diffsum(0, PayLine, 0);
        case 1:
          return diffsum(0, PayLine, 1);
        case 2:
          return diffsum(0, PayLine, 2);
      }
    }
    if(flg==8){
      switch(index){
        case 0:
          return diffsum(BarL[random(0, 2)], PayLine, 0);
        case 1:
          return diffsum(BarC[random(0, 2)], PayLine, 1);
        case 2:
          return diffsum(BarR[0], PayLine, 2);
      }
    }

}

int diffsum(int base, int _payline, int _index){
    if(base + PayLineDiffer[_payline][_index] >= 21){
      return 0;
    }
    if(base + PayLineDiffer[_payline][_index] <= -1){
      return 20;
    }
    else{
      return base + PayLineDiffer[_payline][_index];
    }
}

int get_index(int id){
  int E0[21] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  int E1[21] = {2, 5, 6, 9, 10, 13, 14, 17, 18};
  int E2[21] = {4, 8, 12, 16, 20};
  int E3[21] = {2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15, 17, 18, 19};
  int E4[21] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
  int E5[21] = {2, 3, 4, 6, 7, 8, 10, 11, 12, 14, 15, 16, 18, 19, 20};
  int E6[21] = {3, 4, 5, 7, 8, 9, 11, 12, 13, 15, 16, 17, 19, 20};
  int E[7] = {20, 20, 16, 16, 21, 17, 16};
  if(id==0){
    return E0[random(0, 19)];
  }
  if(id==1){
    return E1[random(0, 9)];
  }
  if(id==2){
    return E2[random(0, 5)];
  }
  if(id==3){
    return E3[random(0, 14)];
  }
  if(id==4){
    return E4[random(0, 19)];
  }
  if(id==5){
    return E5[random(0, 15)];
  }
  if(id==6){
    return E6[random(0, 14)];
  }
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

int index2id(int index, int _index){
  //7:0
  //Bar:1
  //ブドウ:2
  //リプレイ:3
  //チェリー:4
  //ピエロ:5
  //ベル:6
    if(_index==0){
      if(index==0){
        return 0;
      }
      if(search(BarL, 2, index)){
        return 1;
      }
      if(search(GrapeL, 7, index)){
        return 2;
      }
      if(search(RepL, 5, index)){
        return 3;
      }
      if(search(CherryL, 2, index)){
        return 4;
      }
      if(search(ClownL, 2, index)){
        return 5;
      }
      else{
        return 6;
      }
    }
    else
    if(_index==1){
      if(index==0){
        return 0;
      }
      if(search(BarC, 2, index)){
        return 1;
      }
      if(search(GrapeC, 5, index)){
        return 2;
      }
      if(search(RepC, 5, index)){
        return 3;
      }
      if(search(CherryC, 5, index)){
        return 4;
      }
      if(index==20){
        return 5;
      }
      else{
        return 6;
      }
    }
    else{
      if(index==0){
        return 0;
      }
      if(index==1){
        return 1;
      }
      if(search(GrapeR, 5, index)){
        return 2;
      }
      if(search(RepR, 5, index)){
        return 3;
      }
      if(search(ClownR, 4, index)){
        return 5;
      }
      else{
        return 6;
      }
    }
}

bool search(int arr[], int length, int index){
    for (int i=0; i<length; i++) {
      if (arr[i] == index) {
        return true;
      }
  }
  return false;
}

void xorshift128_seed(uint32_t seed){
    if (seed != 0){
        x = y = z = w = seed;
    }
}

uint32_t xorshift128(){
    uint32_t t;
    t = x ^ (x << 11);
    x = y; y = z; z = w;
    w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    return w;
}

void _xorshift128_seed(uint32_t seed){
    if (seed != 0){
        _x = _y = _z = _w = seed;
    }
}

uint32_t _xorshift128(){
    uint32_t t;
    t = _x ^ (_x << 11);
    _x = _y; _y = _z; _z = _w;
    _w = (_w ^ (_w >> 19)) ^ (t ^ (t >> 8));
    return _w;
}

int shift128_random(){
  String rnd = String(xorshift128());
  return rnd.substring(rnd.length() - 1).toInt();
  
}

int _shift128_random(){
  String rnd = String(_xorshift128());
  return rnd.substring(rnd.length() - 1).toInt();
}

bool chance(int top, int bottom){
  return random(0, 65536) <= (65536/bottom)*top;
}

void Show_Config(int conf){
  flash(10);
  digitalWrite(13, LOW);
  delay(1000);
  for(int i=0; i<conf; i=i+1) {
	   digitalWrite(13, HIGH);
	   delay(300);
	   digitalWrite(13, LOW);
	   delay(300);
	}
  delay(500);
  flash(10);
}
void flash(int n){
  for(int i=0; i<n; i=i+1) {
	   digitalWrite(13, HIGH);
	   delay(50);
	   digitalWrite(13, LOW);
	   delay(50);
	}
}
