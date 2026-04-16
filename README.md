# Self-Balancing Robot (PID Control)

This project uses an **Arduino Uno** and an **MPU6050** to create a self-balancing robot with **12V JGA25-371 DC motors**.

## 🛠 Features
* **PID Control Loop:** Uses Proportional, Integral, and Derivative logic to maintain balance.
* **Fixed Timing:** The loop is locked to **10ms (100Hz)** for consistent sensor readings and motor response.
* **Safety Kill Switch:** Automatically shuts down motors if the tilt exceeds **45 degrees** to prevent hardware damage.
* **Motor Deadband Fix:** Ensures the motors receive enough power to overcome gear friction at low speeds.

## 🔌 Hardware Setup
* **Controller:** Arduino Uno
* **IMU:** MPU6050 (connected via I2C)
* **Motor Driver:** L298N H-Bridge
* **Power:** Li-ion 18650

## 🚀 How to Run
1.  **Open this folder** in VS Code with the PlatformIO extension.
2.  **Click the Checkmark (Build)** to automatically download the `MPU6050_light` library.
3.  **Upload** to the Arduino.
4.  **Important:** Hold the robot perfectly still and upright during the **"Calibration" phase**!
