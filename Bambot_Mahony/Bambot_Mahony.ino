#include "M5Stack.h"
#include "utility/MPU9250.h"
#include <GregMahony.h>
#include <Wire.h>

GregMahony AHRSfilter;
MPU9250 IMU;

void setup() {
  M5.begin();
  Wire.begin();

  IMU.calibrateMPU9250(IMU.gyroBias, IMU.accelBias);
  IMU.initMPU9250();

  M5.Lcd.setCursor(0, 0,2); M5.Lcd.print("P: ");
  M5.Lcd.setCursor(0,15,2); M5.Lcd.print("Y: ");
  M5.Lcd.setCursor(0,30,2); M5.Lcd.print("R: ");
  M5.Lcd.setCursor(0,45,2); M5.Lcd.print("T: ");
}

uint16_t totalTime = 0, lastTime = 0;

void loop() {
  uint16_t readTime;
  if (IMU.readByte(MPU9250_ADDRESS, INT_STATUS) & 0x01) {
    readAndScaleIMU();

    totalTime = micros() - lastTime;
    lastTime = micros();
    
    AHRSfilter.updateIMU(IMU.gx, IMU.gy, IMU.gz, IMU.ax, IMU.ay, IMU.az, totalTime);
  }

  M5.Lcd.setCursor(15, 0,2); M5.Lcd.print(AHRSfilter.getPitch()*RAD_TO_DEG); M5.Lcd.print("    ");
  M5.Lcd.setCursor(15,15,2); M5.Lcd.print(AHRSfilter.getYaw()*RAD_TO_DEG);   M5.Lcd.print("    ");
  M5.Lcd.setCursor(15,30,2); M5.Lcd.print(AHRSfilter.getRoll()*RAD_TO_DEG);  M5.Lcd.print("    ");
  M5.Lcd.setCursor(15,45,2); M5.Lcd.print(totalTime);
}

void readAndScaleIMU() {
  IMU.readAccelData(IMU.accelCount);
  IMU.getAres();
  IMU.ax = IMU.accelCount[0];
  IMU.ay = IMU.accelCount[1];
  IMU.az = IMU.accelCount[2];

  IMU.readGyroData(IMU.gyroCount);
  IMU.getGres();
  IMU.gx = (float)IMU.gyroCount[0]*IMU.gRes*DEG_TO_RAD;
  IMU.gy = (float)IMU.gyroCount[1]*IMU.gRes*DEG_TO_RAD;
  IMU.gz = (float)IMU.gyroCount[2]*IMU.gRes*DEG_TO_RAD;
}
