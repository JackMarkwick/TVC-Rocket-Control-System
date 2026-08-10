#include <SD.h>
#include <Wire.h>
#include <Servo.h>
#include "PID_Controller.h" 
#include "IMU.h" 
#include "Logger.h"     

// --- Hardware Pins ---

//Electronic Speed Controller (ESC) PWN Output Pins
const int motorL_Pin = 8;
const int motorR_Pin = 9;

//Servo Actuator Pins
const int servoR1_Pin = A0; 
const int servoR2_Pin = A1;  
const int servoL1_Pin = A2;   
const int servoL2_Pin = A3;

//SD Card Adapter Module
const int sd_Card = 4;

// --- Hardware Setup ---
IMUHandler imu;
Logger logger;
Servo motorL, motorR;  
Servo sR1, sR2, sL1, sL2; 

// --- PID Configuration ---
// PIDController(dt, Kp, Ki, Kd, min_out, max_out)
PIDController pidPitch(0.01, 0.3, 0.0, 0.05, -30.0, 30.0);
PIDController pidRoll(0.01, 0.3, 0.0, 0.05, -30.0, 30.0);
PIDController pidYaw(0.01, 0.1, 0.0, 0.3, -30.0, 30.0);

// Setpoints: pole to stay at 0 degrees (vertical)
const double targetPitch = 0.0;
const double targetRoll = 0.0;
const double targetYaw = 0.0;

//Time Tracking Variables
unsigned long lastMicros = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000); // High speed I2C for fast corrections

  //servos PWN output Pins with safe pulse ranges
  sR1.attach(servoR1_Pin, 1000, 2000);
  sR2.attach(servoR2_Pin, 1000, 2000);
  sL1.attach(servoL1_Pin, 1000, 2000);
  sL2.attach(servoL2_Pin, 1000, 2000);

  // Initial Centering
  moveGimbals(90);

  //Motor PWM output Pins
  motorL.attach(motorL_Pin, 1000, 2000);
  motorR.attach(motorR_Pin, 1000, 2000);

  //Arming Sequence
  motorL.writeMicroseconds(1000);
  motorR.writeMicroseconds(1000);
  Serial.println(F("Waiting 5 seconds for ESCs to arm..."));
  delay(5000);

  //Debugging code
  if (imu.begin()) {
    Serial.println(F("IMU Initialized. System Standing By..."));
  } else {
    Serial.println(F("IMU Connection Failed!"));
    while (1);
  }

  if (logger.begin(sd_Card)) {
    Serial.println(F("SD Initialized"));
  } else{
    Serial.println(F("SD Failed"));
  }
  
  delay(2000); // Safety delay to step back
  lastMicros = micros();
}

void loop() {
  unsigned long currentMicros = micros();

  // 100Hz Control Loop (every 10ms)
  if (currentMicros - lastMicros >= 10000) {
    lastMicros = currentMicros;

    if (imu.update()) {
      // 1. Get current orientation from MPU6050
      double currentPitch = imu.getPitch();
      double currentYaw = imu.getYaw();
      double currentRoll = imu.getRoll();

      // 2. Calculate PID Adjustments
      double adjPitch = pidPitch.calculate(targetPitch, currentPitch);
      double adjRoll = pidRoll.calculate(targetRoll, currentRoll);
      double adjYaw = pidYaw.calculate(targetYaw, currentYaw);

      // 3. 3D Mixing Logic
      double outL1 = 90 + adjRoll + adjPitch; 
      double outR1 = 90 + adjRoll - adjPitch;

      double outL2 = 90 + adjRoll + adjPitch; 
      double outR2 = 90 + adjRoll - adjPitch;

      //SD Card Logging
      logger.logData(millis(), currentPitch, currentRoll, currentYaw, outL1, outR1, outL2, outR2);

      // 4. Final Output with Safety Constraints
      sL1.write(constrain(outL1, 50, 120));
      sL2.write(180 - constrain(outR1, 50, 120));

      sR1.write(constrain(outL2, 50, 120));
      sR2.write(180 - constrain(outR2, 50, 120));

      //Fixed Throttle Setting
      motorL.writeMicroseconds(1400);
      motorR.writeMicroseconds(1500);

      // Debugging: Monitor the stability
      static unsigned long lastPrint = 0;
      if (millis() - lastPrint > 100) {
        Serial.print("P: "); Serial.print(currentPitch);       
        Serial.print(" | R: "); Serial.print(currentRoll);
        Serial.print(" | Y: "); Serial.print(currentYaw);
        Serial.print(" | OutL1: "); Serial.println(outL1);
        Serial.print(" | OutR1: "); Serial.println(outR1);
        Serial.print(" | OutL2: "); Serial.println(outL2);
        Serial.print(" | OutR2: "); Serial.println(outR2);
        Serial.print(" | adjP: "); Serial.print(adjPitch);
        Serial.print(" | adjR: "); Serial.print(adjRoll);
        Serial.print(" | adjY: "); Serial.println(adjYaw);
        lastPrint = millis();
      }
    }
  }
}

void moveGimbals(int pos) {
  sR1.write(pos);
  sR2.write(pos);
  sL1.write(pos);
  sL2.write(pos);
}