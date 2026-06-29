#include <Wire.h>
#include <MD_MAX72xx.h>

// ENTER YOUR CALIBRATION OFFSETS HERE
int16_t offsetX = 0; // Replace with your X offset
int16_t offsetY = 0; // Replace with your Y offset

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW 
#define MAX_DEVICES 1
#define CS_PIN 10 
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#define TOUCH_1 2
#define TOUCH_2 3
#define TOUCH_3 4
#define TOUCH_4 5
#define SHIFT_PIN 6
#define BUZZER 7

int currentMode = 0; 

bool lastShiftState = LOW;

const int Compass_addr = 0x1E;
const int MPU_addr = 0x68;

// --- GAME VARIABLES ---
// Bowling
bool pins[4][8]; bool ballRolling = false; int bX = 3, bY = 7; unsigned long lastRollFrame = 0;
// Snake
int snakeX[64], snakeY[64]; int snakeLen = 3; int foodX, foodY, snakeDir = 1; unsigned long lastSnakeMove = 0; bool snakeGameOver = false;
// Flappy Dot
float birdY = 3.0; float birdVel = 0; float gravity = 0.04; float pipeX = 7.0; int pipeGapY = 3; unsigned long lastFlapFrame = 0; bool flappyOver = false;
// 8-Ball / Dice
unsigned long shakeTimer = 0; int randomResult = 0;

// --- Pixel Graphics ---
const uint8_t charN[8] = {0b11000011, 0b11100011, 0b11110011, 0b11011011, 0b11001111, 0b11000111, 0b11000011, 0};
const uint8_t charS[8] = {0b00111100, 0b01100110, 0b01100000, 0b00111100, 0b00000110, 0b01100110, 0b00111100, 0};
const uint8_t charE[8] = {0b01111110, 0b01100000, 0b01100000, 0b01111100, 0b01100000, 0b01100000, 0b01111110, 0};
const uint8_t charW[8] = {0b11000011, 0b11000011, 0b11000011, 0b11011011, 0b11111111, 0b11100111, 0b11000011, 0};
const uint8_t charY[8] = {0b11000011, 0b01100110, 0b00111100, 0b00011000, 0b00011000, 0b00011000, 0b00011000, 0};
const uint8_t charQ[8] = {0b00111100, 0b01100110, 0b00001100, 0b00011000, 0b00011000, 0b00000000, 0b00011000, 0}; 
const uint8_t char8[8] = {0b00111100, 0b01100110, 0b01100110, 0b00111100, 0b01100110, 0b01100110, 0b00111100, 0}; 

// Dice Graphics
const uint8_t dice[6][8] = {
  {0,0,0, 0b00011000, 0b00011000, 0,0,0}, 
  {0,0, 0b01100000, 0b01100000, 0, 0b00000110, 0b00000110, 0}, 
  {0, 0b01100000, 0b01100000, 0b00011000, 0b00011000, 0b00000110, 0b00000110, 0}, 
  {0, 0b01100110, 0b01100110, 0, 0, 0b01100110, 0b01100110, 0}, 
  {0, 0b01100110, 0b01100110, 0b00011000, 0b00011000, 0b01100110, 0b01100110, 0}, 
  {0b01100110, 0b01100110, 0b01100110, 0b01100110, 0b01100110, 0b01100110, 0, 0}  
};

void drawLetter(const uint8_t letterPattern[8]) {
  for (int row = 0; row < 8; row++) mx.setRow(0, row, letterPattern[row]);
}

void beep(int freq, int duration) { tone(BUZZER, freq, duration); }

void resetPins() {
  memset(pins, 0, sizeof(pins));
  pins[0][2]=1; pins[0][4]=1; pins[0][6]=1; pins[1][3]=1; pins[1][5]=1; pins[2][4]=1;
}

void resetSnake() {
  snakeLen=3; snakeX[0]=3; snakeY[0]=3; snakeX[1]=2; snakeY[1]=3; snakeX[2]=1; snakeY[2]=3;
  snakeDir=1; foodX=random(0,8); foodY=random(0,8); snakeGameOver=false;
}

void resetFlappy() {
  birdY = 3.0; birdVel = 0; pipeX = 7.0; pipeGapY = random(1, 5); flappyOver = false;
}

void setMode(int newMode) {
  if (currentMode != newMode) {
    currentMode = newMode;
    mx.clear();
    beep(1500, 50); // Click sound
    
    if (currentMode == 2) { ballRolling = false; resetPins(); }
    if (currentMode == 3) resetSnake();
    if (currentMode == 4) resetFlappy();
    if (currentMode == 5 || currentMode == 6) randomResult = 0; 
    delay(250); 
  }
}

// SETUP
void setup() {
  Serial.begin(9600);
  pinMode(TOUCH_1, INPUT); pinMode(TOUCH_2, INPUT); 
  pinMode(TOUCH_3, INPUT); pinMode(TOUCH_4, INPUT);
  pinMode(SHIFT_PIN, INPUT); pinMode(BUZZER, OUTPUT);

  mx.begin(); mx.control(MD_MAX72XX::INTENSITY, 2); mx.clear();
  Wire.begin(); Wire.setWireTimeout(3000, true); 
  
  Wire.beginTransmission(Compass_addr); Wire.write(0x02); Wire.write(0x00); Wire.endTransmission();
  Wire.beginTransmission(MPU_addr); Wire.write(0x6B); Wire.write(0); Wire.endTransmission(true);

  randomSeed(analogRead(0));
  resetPins();
  beep(1000, 100); delay(100); beep(2000, 150);
}

// MAIN LOOP
void loop() {
  bool shift = digitalRead(SHIFT_PIN);
  
  // Menu selection
  if (digitalRead(TOUCH_1)) setMode(shift ? 1 : 0);
  if (digitalRead(TOUCH_2)) setMode(shift ? 3 : 2);
  if (digitalRead(TOUCH_3)) setMode(shift ? 5 : 4);
  if (digitalRead(TOUCH_4)) setMode(shift ? 7 : 6);

  if (currentMode == 4 && shift == HIGH && lastShiftState == LOW && !flappyOver) {
    birdVel = -0.25;
    beep(1200, 15); 
  }
  lastShiftState = shift;

  switch (currentMode) {
    case 0: runNeedleCompass(); break;
    case 1: runLetterCompass(); break;
    case 2: runArcadeBowling(); break;
    case 3: runSnakeGame(); break;
    case 4: runFlappyDot(); break;
    case 5: runDice(); break;
    case 6: runMagic8Ball(); break;
    case 7: runBubbleLevel(); break;
  }
}

// MPU SENSOR HELPER
void readMPU(int16_t &calX, int16_t &calY) {
  Wire.beginTransmission(MPU_addr); Wire.write(0x3B); Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 4, true);
  calX = (Wire.read() << 8 | Wire.read()) - offsetX;
  calY = (Wire.read() << 8 | Wire.read()) - offsetY;
}

// MODES 0 & 1: COMPASS
float getHeading() {
  int16_t x,y,z; Wire.beginTransmission(Compass_addr); Wire.write(0x03); Wire.endTransmission(false);
  Wire.requestFrom(Compass_addr, 6);
  if(Wire.available()>=6){ x=Wire.read()<<8|Wire.read(); z=Wire.read()<<8|Wire.read(); y=Wire.read()<<8|Wire.read(); }
  float h = atan2(y, x); if(h<0) h+=2*PI; return h;
}

void runNeedleCompass() {  
  float h = getHeading(); int s = (int)(h * 180 / M_PI + 22.5) / 45 % 8;
  int lx=3, ly=3;
  switch(s){case 0:lx=3;ly=0;break; case 1:lx=7;ly=0;break; case 2:lx=7;ly=3;break; case 3:lx=7;ly=7;break; case 4:lx=4;ly=7;break; case 5:lx=0;ly=7;break; case 6:lx=0;ly=4;break; case 7:lx=0;ly=0;break;}
  mx.clear(); mx.setPoint(ly, lx, true); delay(100);
}

void runLetterCompass() {
  float deg = getHeading() * 180 / M_PI; 
  if (deg>=315 || deg<45) drawLetter(charN); else if (deg>=45 && deg<135) drawLetter(charE);
  else if (deg>=135 && deg<225) drawLetter(charS); else if (deg>=225 && deg<315) drawLetter(charW);
  delay(150);
}

// MODE 2: BOWLING
void runArcadeBowling() {
  int16_t calX, calY; readMPU(calX, calY);
  if (!ballRolling) {
    bY = 7; bX = map(calX, -4000, 4000, 0, 7); bX = constrain(bX, 0, 7);
    if (calY > 6000 || calY < -6000) { ballRolling = true; beep(400, 100); }
  } else {
    if (millis() - lastRollFrame > 80) {
      lastRollFrame = millis(); bY--;
      if (bY >= 0 && bY <= 2 && pins[bY][bX] == 1) {
        pins[bY][bX] = 0; beep(200, 50); 
      }
      if (bY < 0) {
        ballRolling = false; bool left=false;
        for(int r=0;r<3;r++) for(int c=0;c<8;c++) if(pins[r][c]==1) left=true;
        if(!left) { resetPins(); beep(800, 300); } 
      }
    }
  }
  mx.clear();
  for(int r=0;r<3;r++) for(int c=0;c<8;c++) if(pins[r][c]==1) mx.setPoint(r, c, true);
  mx.setPoint(bY, bX, true); delay(20);
}

// MODE 3: SNAKE
void runSnakeGame() {
  if (snakeGameOver) { drawLetter(charN); delay(500); return; } 
  int16_t calX, calY; readMPU(calX, calY);
  if (calX > 4000 && snakeDir != 3) snakeDir = 1; else if (calX < -4000 && snakeDir != 1) snakeDir = 3; 
  else if (calY > 4000 && snakeDir != 0) snakeDir = 2; else if (calY < -4000 && snakeDir != 2) snakeDir = 0; 
  
  if (millis() - lastSnakeMove > 350) { 
    lastSnakeMove = millis();
    for(int i = snakeLen-1; i>0; i--) { snakeX[i]=snakeX[i-1]; snakeY[i]=snakeY[i-1]; }
    if(snakeDir==0) snakeY[0]--; if(snakeDir==1) snakeX[0]++; if(snakeDir==2) snakeY[0]++; if(snakeDir==3) snakeX[0]--;
    
    if(snakeX[0]<0 || snakeX[0]>7 || snakeY[0]<0 || snakeY[0]>7) { snakeGameOver=true; beep(150, 500); }
    for(int i=1; i<snakeLen; i++) if(snakeX[0]==snakeX[i] && snakeY[0]==snakeY[i]) { snakeGameOver=true; beep(150, 500); }
    
    if(snakeX[0]==foodX && snakeY[0]==foodY) { if(snakeLen<64) snakeLen++; foodX=random(0,8); foodY=random(0,8); beep(2000, 50); }
  }
  mx.clear(); if ((millis()/150)%2==0) mx.setPoint(foodY, foodX, true); 
  for(int i=0; i<snakeLen; i++) mx.setPoint(snakeY[i], snakeX[i], true);
}

// MODE 4: FLAPPY DOT 
void runFlappyDot() {
  if(flappyOver) { mx.clear(); beep(150, 500); delay(1000); resetFlappy(); return; }

  birdVel += gravity; 
  birdY += birdVel; 
  pipeX -= 0.08; 
  
  int pX = (int)round(pipeX);

  if (birdY > 7 || birdY < 0) flappyOver = true; 
  if (pipeX < -1.0) { pipeX = 7.0; pipeGapY = random(1, 5); beep(800, 20); }
  
  if (pX == 2) { 
    if (round(birdY) < pipeGapY || round(birdY) > pipeGapY + 2) flappyOver = true; 
  }

  mx.clear();
  mx.setPoint((int)round(birdY), 2, true); 
  
  if (pX >= 0 && pX <= 7) {
    for(int i=0; i<8; i++) {
      if (i != pipeGapY && i != pipeGapY + 1 && i != pipeGapY + 2) mx.setPoint(i, pX, true); 
    }
  }
  delay(40); 
}

// MODE 5 & 6: DICE & MAGIC 8-BALL
void handleShake(int maxRand) {
  int16_t calX, calY; readMPU(calX, calY);
  if (abs(calX) > 6000 || abs(calY) > 6000) { 
    shakeTimer = millis(); randomResult = random(1, maxRand + 1); beep(500, 50);
  }
}

void runDice() {
  handleShake(6);
  if (millis() - shakeTimer < 500) { 
    drawLetter(dice[random(0,6)]); delay(50); 
  } else if (randomResult > 0) { 
    drawLetter(dice[randomResult - 1]); 
  } else {
    mx.clear(); 
  }
}

void runMagic8Ball() {
  handleShake(3);
  if (millis() - shakeTimer < 1000) { 
    if ((millis()/100)%2==0) drawLetter(charQ); else mx.clear(); 
  } else if (randomResult > 0) {
    if(randomResult == 1) drawLetter(charY); 
    if(randomResult == 2) drawLetter(charN); 
    if(randomResult == 3) drawLetter(charQ); 
  } else {
    drawLetter(char8); 
  }
}

// MODE 7: BUBBLE LEVEL
void runBubbleLevel() {
  int16_t calX, calY; readMPU(calX, calY);
  int ledX = map(calX, -4000, 4000, 0, 7); ledX = constrain(ledX, 0, 7);
  int ledY = map(calY, -4000, 4000, 0, 7); ledY = constrain(ledY, 0, 7);
  mx.clear(); mx.setPoint(ledY, ledX, true); 
  if (ledX == 3 && ledY == 3) beep(3000, 20); 
  delay(30);
}