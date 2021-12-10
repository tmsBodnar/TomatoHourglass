#include <virtuabotixRTC.h>
#include <MD_MAX72xx.h>
#include <Adafruit_LSM303_Accel.h>
#include <Adafruit_Sensor.h>
#include "NormalSmallFonts.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
#define CLOCK_ARRAY_SIZE 5
#define COUNTDOWN_ARRAY_SIZE 2

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
virtuabotixRTC myRTC(6, 7, 8);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
sensors_event_t event;

int CLOCK_COL_POSITIONS[CLOCK_ARRAY_SIZE] = { 28, 22, 16, 13, 7 };
int REVERSE_CLOCK_POSITIONS[CLOCK_ARRAY_SIZE] = { 7, 13, 16, 22, 28 };
int WORK_COL_POSITIONS[COUNTDOWN_ARRAY_SIZE] = { 7, 3 };
int BREAK_COL_POSITIONS[COUNTDOWN_ARRAY_SIZE] = { 31, 27 };
const int displays = 3;
const int columns = 8;
int HOURGLASS_DEVICES_DOWN[displays] = { 1, 2, 3 };
int HOURGLASS_DEVICES_UP[displays] = { 2, 1, 0  };
int UP_HOURGLASS[displays][columns]= {{ 254, 254, 254, 254, 130, 130, 130, 254 },{ 130, 130, 68, 40, 56, 124, 254, 254 },{ 254, 130, 130, 130, 130, 130, 130, 130 }};
int DOWN_HOURGLASS[displays][columns] = {{ 127, 65, 65, 65, 127, 127, 127, 127 },{ 127, 127, 62, 28, 20, 34, 65, 65 },{ 65, 65, 65, 65, 65, 65, 65, 127 }};

int work = 3;
int shortBreak = 1;
int longBreak = 2;
double counter = 0;
int breakCounter = 1;
bool displayResetted = true;

void setup() {
   Serial.begin(57600);
  // myRTC.setDS1302Time(55, 47, 16, 7, 28, 11, 2021);
  mx.begin();
  mx.control(MD_MAX72XX::INTENSITY, 1);
  accel.begin();
}

void loop() {
  matrixOnOff();
  if (displayResetted) {
    getOrientation();  
  }
  delay(250);
}

void matrixOnOff() {
  float y;
  y = getYAccel();
  if (y > 7) {
    mx.control(MD_MAX72XX::SHUTDOWN, MD_MAX72XX::ON);
    resetCountdown();
  }
  if (y < 7 && !displayResetted) {
    mx.control(MD_MAX72XX::SHUTDOWN, MD_MAX72XX::OFF);
    displayResetted = true;
  }
}

void getOrientation() {
  float x, z;
  
  accel.getEvent(&event);
  x = event.acceleration.x;
  z = event.acceleration.z;
  if (z < -7) {
    printClock(true);
  }
  if (z > 7) {
    printClock(false);
  }
  if (x < -7)
  {
    printCountdown(true);
  }
  if (x > 7)
  {
    printCountdown(false);
  }
}

void printClock(bool reversed) {
  myRTC.updateTime();
  char delimeter = myRTC.seconds % 2 == 0 ? ':' : ' ';
  char timeText[5];
  char format[11] = {'%', '0', '2', 'd', delimeter, '%', '0', '2', 'd'};
  sprintf(timeText, format, myRTC.hours, myRTC.minutes);

  mx.clear();
  mx.update(MD_MAX72XX::OFF);
  mx.setFont(normal);
  
  if (reversed) {
    drawClockOrCountdown(CLOCK_ARRAY_SIZE, CLOCK_COL_POSITIONS, timeText);
    mx.transform(MD_MAX72XX::TFUD);
    mx.transform(MD_MAX72XX::TFLR);
  } else {
    drawClockOrCountdown(CLOCK_ARRAY_SIZE, CLOCK_COL_POSITIONS, timeText);
  }
  
  mx.update(MD_MAX72XX::ON);
}

void printCountdown(boolean upDown) {
  char timeText[2];
  char format[11] = {'%', '0', '2', 'd' };
  int *countdownTime;
  counter += 0.25;
  mx.clear();
  mx.update(MD_MAX72XX::OFF);
  mx.setFont(small);
  if (upDown) {
    if (breakCounter == 4) {
      sprintf(timeText, format, longBreak);
      countdownTime = &longBreak;
    } else {
      sprintf(timeText, format, shortBreak);
      countdownTime = &shortBreak;
    }
    drawClockOrCountdown(COUNTDOWN_ARRAY_SIZE, BREAK_COL_POSITIONS, timeText);
    rotateCW(3);
    rotateCW(3);
    rotateCW(3);
    calculateHourglass(HOURGLASS_DEVICES_UP, UP_HOURGLASS, *countdownTime);
  } else {
    sprintf(timeText, format, work);
    countdownTime = &work;
    drawClockOrCountdown(COUNTDOWN_ARRAY_SIZE, WORK_COL_POSITIONS, timeText);
    rotateCW(0);
    calculateHourglass(HOURGLASS_DEVICES_DOWN, DOWN_HOURGLASS, *countdownTime);
  }
  
  mx.update(MD_MAX72XX::ON); 
  if (counter == 60) {
    *countdownTime -= 1;
    counter = 0;
  }
  if ((*countdownTime) == 0) {
    if (upDown) {
      if (breakCounter == 4) {
        breakCounter = 1;
      } else {
        breakCounter += 1;
      }
    }
   displayBlinker(upDown);
  } 
}

void drawClockOrCountdown(int arraySize, int positionArray[], char textArray[]){
   for (int i = arraySize - 1; i >= 0; i--) {
      mx.setChar(positionArray[i], textArray[i]);
    }
}

void calculateHourglass(int devices[], int positions[displays][columns], int countdownTime){
  printHourglass(devices, positions);
}

void printHourglass(int devices[], int positions[displays][columns]){
  for (int i = 0; i < displays; i ++) {
    for (int j = columns - 1; j >= 0; j--) {
      mx.setColumn(devices[i], j, positions[i][j]);
    }
  }
  animateSand(devices);
}

void animateSand(int devices[]){
  Serial.println(mx.getColumnCount());
  for (int i = 0; i < sizeof(devices) - 0; i++){
    int on = counter / 0.25;
    mx.setPoint(4, i + 6, (on % 2 == 0) );
}
}

void displayBlinker(bool upDown) {
  int pos[2];
  mx.clear();
  resetCountdown();
  while (getYAccel() < 7)
  {
    if (upDown) {
      pos[0] = HOURGLASS_DEVICES_UP[0];
      pos[1] = HOURGLASS_DEVICES_UP[2];
    } else {
      pos[0] = HOURGLASS_DEVICES_DOWN[2];
      pos[1] = HOURGLASS_DEVICES_DOWN[0];
    }
    Serial.println(pos[0]);
    Serial.println(pos[1]);
    mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::ON);
    mx.transform(MD_MAX72XX::TINV);
    delay(500);
    mx.transform(MD_MAX72XX::TINV);
    delay(500);
    mx.control(MD_MAX72XX::WRAPAROUND, MD_MAX72XX::OFF);
  }
}

void resetCountdown() {
  work = 3;
  shortBreak = 1;
  longBreak = 2;
  counter = 0;
  displayResetted = false;
}

float getYAccel() {
  
  float y;
  accel.getEvent(&event);
  y = event.acceleration.y;
  return y; 
}

void rotateCW(int buff) {
  uint8_t t[ROW_SIZE];
  for (uint8_t i=0; i<ROW_SIZE; i++) {
    t[i] = mx.getColumn(buff, COL_SIZE-1-i);
  }
  for (uint8_t i=0; i<ROW_SIZE; i++) {
    mx.setRow(buff, i, t[i]);
  }
}
