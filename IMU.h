#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"


class IMUHandler {
  public:
    MPU6050 mpu;
    bool dmpReady = false;
    uint16_t packetSize;
    uint8_t fifoBuffer[64];
    Quaternion q;
    VectorFloat gravity;
    float ypr[3];


    bool begin() {
      mpu.initialize();
      uint8_t devStatus = mpu.dmpInitialize();


      //Custom Calibration Offests for MPU6050
      mpu.setXAccelOffset(-2159);
      mpu.setYAccelOffset(-3779);
      mpu.setZAccelOffset(2794);
      mpu.setXGyroOffset(147);
      mpu.setYGyroOffset(57);
      mpu.setZGyroOffset(7);


      if (devStatus == 0) {
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.setDMPEnabled(true);
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
        return true;
      }
      return false; //Initialization failed
    }


    bool update() {
      if (!dmpReady) return false;

      //Extract operational sensor data
      if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {

        //raw measurements into 3D coordinate frame arrays
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity); //Computes Euler spatial dynamics
        return true;


      }
      return false; //buffer empty or packet Transmission inerrupted
    }


    float getPitch() {
      return ypr[1] * 180 / M_PI;
    }


    float getYaw() {
      return ypr[0] * 180 / M_PI;
    }

    float getRoll() {
      return ypr[2] * 180 / M_PI; // 
    }
};
