#line 1 "c:\\Users\\dylch\\Desktop\\coden\\Project S.L.O.S\\all the code\\sign languagse glove\\The_ALS_Glove_starter_code\\mpu.h"
#include "MPU9250.h"

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);

int aacelY = 0;
int accelZ = 0;
int accelX = 0;

int gyroX = 0;
int gyroY = 0;
int gyroZ = 0;

void set_range()
{
    // setting the accelerometer full scale range to +/-8G 
  IMU.setAccelRange(MPU9250::ACCEL_RANGE_8G);
  // setting the gyroscope full scale range to +/-500 deg/s
  IMU.setGyroRange(MPU9250::GYRO_RANGE_500DPS);
  // setting DLPF bandwidth to 20 Hz
  IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_20HZ);
  // setting SRD to 19 for a 50 Hz update rate
  IMU.setSrd(19);
}

void print_mpu() 
{

  Serial.print(IMU.getAccelY_mss(),6);
  Serial.print("\t");
  Serial.print(IMU.getAccelY_mss(),6);
  Serial.print("\t");
  Serial.print(IMU.getAccelZ_mss(),6);
  Serial.print("\t");
  Serial.print(IMU.getGyroX_rads(),6);
  Serial.print("\t");
  Serial.print(IMU.getGyroY_rads(),6);
  Serial.print("\t");
  Serial.print(IMU.getGyroZ_rads(),6);
  Serial.print("\t");
  Serial.print(IMU.getMagX_uT(),6);
  Serial.print("\t");
  Serial.print(IMU.getMagY_uT(),6);
  Serial.print("\t");
  Serial.print(IMU.getMagZ_uT(),6);
  Serial.print("\t");
  Serial.println(IMU.getTemperature_C(),6);
  delay(20);
}

void readmpu()
{
  int aacelY = IMU.getAccelY_mss();
  int accelZ = IMU.getAccelZ_mss();
  int accelX = IMU.getAccelX_mss();
  int gyroX = IMU.getGyroX_rads();
  int gyroY = IMU.getGyroY_rads();
  int gyroZ = IMU.getGyroZ_rads();

}
