# Arctic Analysis

**UTRA Hacks 2026**

Arctic Analytics is a robotics and data analytics project built at **UTRA Hacks 2026**. The system combines a robot that navigates a winter biathlon-style track with a web-based analytics platform that visualizes sensor data and provides AI-driven feedback for future performance improvements.

---

## 🧠 Project Overview

Our robot collects real-time sensor data while navigating the track, including:

* Motor speed (RPM)
* Distance measurements (ultrasonic sensors)
* Line-following data
* Color detection data

This data is stored, analyzed, and visualized through our website dashboard. Generative AI is then used to interpret the data and generate actionable insights, which can also be delivered through text-to-speech.

---

## 🌐 Web Dashboard

We built a web-based analytics dashboard that:

* Displays live and historical robot sensor data
* Visualizes metrics such as speed, distance, color detection, and line following
* Connects directly to MongoDB for data storage and retrieval
* Uses AI-generated insights to explain robot performance and suggest improvements

---

## 🤖 Hardware & Firmware

The robot is powered by Arduino-based firmware and includes:

* Ultrasonic distance sensors
* IR sensors for line following
* Color sensors for track detection
* Motor controllers for movement and steering

### Firmware Files

* `motor.ino`, `motors.ino` – Motor control logic
* `UltrasonicSensor.ino`, `ultrasonic.ino` – Distance sensing
* `LineFollow.ino` – Line-following behavior
* `ColorSensor.ino` – Color detection
* `Arm.ino` – Actuator/arm control
* `sketch_jan31a.ino` – Main integrated sketch

---

## 🗄️ Data & AI Stack

### MongoDB

* Used as the primary database for storing robot sensor data
* Flexible schema enabled fast iteration during the hackathon
* Powers the analytics dashboard

### ElevenLabs

* Used for text-to-speech generation
* Converts AI-generated feedback into natural spoken explanations
* Improves accessibility and usability of analytics insights

---

## 🚀 Future Improvements!

* Real-time AI feedback during robot operation
* Enhanced anomaly detection and performance optimization
* Expanded analytics and run-to-run comparisons
* Improved autonomy and navigation accuracy

---

## 🏁 Built At

**UTRA Hacks 2026**
Team **6 Penguins**
