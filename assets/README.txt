# Dual-Axis Thrust Vector Control (TVC) Flight System

An active stabilization Thrust Vector Control (TVC) system engineered for small-scale space vehicles. The system utilizes custom C++ Object-Oriented software on an Arduino Uno to process real-time orientation telemetry from an MPU6050 IMU, outputting correctional signals to a 3D-printed dual-axis gimbal mechanism.

Achieved stable, unassisted free-flight hovering for 5 seconds and successfully validated stabilization under static loads up to 2.5 kg at 50% throttle.

---

## Key Technical Specifications

* **Control Loop:** 100Hz non-blocking PID feedback loop running at 10ms intervals.
* **Actuation System:** Dual-axis gimbal driven by 4x MG996R high-torque servos utilizing custom 3D mixing logic.
* **Propulsion:** Twin Emax RSIII 2207 2500KV brushless motors driven by Skywalker 40A V2 ESCs powered by a 14.8V 4S LiPo battery.
* **Avionics & Logging:** Arduino Uno (ATmega328P) interfacing with MPU6050 6-DOF IMU and SPI Micro SD logging module capturing real-time telemetry.

---

## System Architecture & Hardware

### Complete Assembly & Avionics Housing
Custom-designed 3D-printed enclosures clamp directly to an aluminum tube spine, providing structural rigidity and shifting the Center of Mass (CoM) forward to optimize corrective torque.

| Full Airframe Build | Arduino & SD Card Enclosure |
| :---: | :---: |
| ![Full Rocket Assembly](assets/Full_Rocket.png) | ![Arduino Holder](assets/Arduino_Holder.png) |
| *Figure 1: Complete airframe with top-mounted battery.* | *Figure 2: Custom CAD housing with integrated SD bosses.* |

---

## Electrical & Wiring Architecture

To mitigate high-current motor spikes that caused initial logic rail failures, power isolation was implemented using a zero-shared-path strategy[cite: 2]. The propulsion system draws from the primary 14.8V LiPo rail, while the Arduino logic and IMU are powered independently via a 9V supply to ensure maximum signal integrity.

![Wiring Schematic](assets/Wiring_Schematic.png)
*Figure 3: Electrical schematic showing power distribution rails, ESC modulation, and servo regulators.*

---

## Gimbal Kinematics & Static Load Testing

| 3D Gimbal Mechanism | Static Payload Test (2.5 kg) |
| :---: | :---: |
| ![Gimbal Assembly](assets/Gimbal_Assembly.png) | ![Rocket Payload](assets/Rocket_Payload.png) |
| *Figure 4: Universal joint assembly with side-by-side servo mounts.* | *Figure 5: Static load testing with 2.5 kg payload applied.* |

### Derived Dynamics & Moment of Inertia
Experimental validation was conducted using restoring force measurement rigs and second-order polynomial curve fitting to derive the airframe's Moment of Inertia (I) about its Center of Mass:

* **Pitch Axis Inertia ($I_{pitch}$):** 0.167 kgm^2
* **Roll Axis Inertia ($I_{roll}$):** 0.157 KGM^2
* **Inertial Discrepancy:** 6.12% (validated independent PID variable gain performance)

---

## Repository Structure

├── TVC_Gimbal_Motor.ino    # Main 100Hz control loop & 3D mixing logic
├── PID_Controller.h        # Custom OOP PID class with output clamping
├── IMU.h                   # MPU6050 DMP register wrapper & Euler conversion
├── Logger.h                # Non-blocking SPI SD telemetry data logger
└── assets/                 # Schematics, CAD renders, and build photos
