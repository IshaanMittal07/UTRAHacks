# ❄️ Arctic Analytics — Real-Time Robot Telemetry & AI-Driven Feedback

Arctic Analytics is a real-time robotics telemetry and data analytics system built for **UTRA Hacks 2026**.  
It combines **Arduino-based sensor logging**, a **live web dashboard**, and an **AI voice mascot powered by ElevenLabs** to transform raw robot data into intuitive, actionable insights.

Instead of scrolling through unreadable serial logs, Arctic Analytics lets you *see*, *hear*, and *understand* what your robot is experiencing — in real time.

---

## 🧠 Inspiration

Inspired by the Winter Olympics, particularly the biathlon’s balance of speed, precision, and control, this project explores the challenge of building robots that can navigate complex courses while performing accurate actions. During development, critical sensor insights are often buried in raw serial logs, so Arctic Analytics transforms this data into clear, real-time feedback to improve performance and reliability.

Arctic Analytics reimagines robot debugging and performance analysis by:
- Visualizing sensor data live
- Providing meaningful feedback instead of raw numbers
- Using a friendly **arctic-themed AI mascot** to narrate system states and performance

The arctic theme is inspired by exploration in harsh environments — just as polar explorers rely on instruments to survive, robots rely on sensors to navigate and perform.

---

## 🧊 Project Overview

At **UTRA Hacks 2026**, our robot navigated a **winter biathlon-style track** while collecting real-time sensor data. This data is streamed, visualized, and analyzed to generate AI-driven feedback for future performance improvements.

### Collected Data Includes:
- Motor speed (RPM)
- Distance measurements (ultrasonic sensors)
- Line-following sensor data
- Color detection data

This system helps teams quickly understand robot behavior, identify inefficiencies, and improve navigation accuracy.

---

## ✨ Features

- 🔌 **Arduino Sensor Logging**  
  Collects real-time sensor data directly from the robot.

- 🌐 **Live Web Dashboard**  
  A React-based interface for visualizing telemetry data in real time.

- 🐧 **AI Voice Mascot (ElevenLabs)**  
  An arctic-themed mascot that explains robot status using AI text-to-speech.

- 🧩 **Python Data Bridge**  
  Middleware that streams Arduino serial data to the web dashboard.

- ⚡ **Real-Time Updates**  
  Immediate feedback for faster debugging and iteration.

---

## 🤖 Hardware & Firmware

The robot is powered by Arduino-based firmware and includes:

- Ultrasonic distance sensors
- IR sensors for line following
- Color sensors for track detection
- Motor controllers for movement and steering
- Optional actuator/arm module

### Firmware Files
- `motor.ino`, `motors.ino` — Motor control logic  
- `UltrasonicSensor.ino`, `ultrasonic.ino` — Distance sensing  
- `LineFollow.ino` — Line-following behavior  
- `ColorSensor.ino` — Color detection  
- `Arm.ino` — Actuator/arm control  
- `Final` — Main integrated sketch  

---

## 🗂 Project Structure

```

UTRAHacks-feature-elevenlabs-mascot/
│
├── arduino/
│   └── arctic_logger/
│       └── arctic_logger.ino     # Arduino sensor logging code
│
├── dashboard/                    # Frontend (React + Vite)
│   ├── src/
│   ├── public/
│   ├── index.html
│   ├── package.json
│   └── .env.example
│
├── bridge.py                     # Python serial → dashboard bridge
├── DEVPOST.md                    # Hackathon submission write-up
└── .gitignore

````

---

## 🚀 Getting Started

### 1️⃣ Arduino Setup
1. Open `arduino/arctic_logger/arctic_logger.ino`
2. Upload the sketch to your Arduino board
3. Verify the correct serial port and baud rate

---

### 2️⃣ Backend (Python Bridge)

```bash
pip install -r requirements.txt
python bridge.py
````

Make sure your Arduino is connected and detected on the correct serial port.

---

### 3️⃣ Frontend Dashboard

```bash
cd dashboard
npm install
npm run dev
```

Open the local development URL shown in your terminal.

---

### 4️⃣ ElevenLabs Setup (Optional but Recommended)

1. Create an ElevenLabs account
2. Copy your API key
3. Create a `.env` file in `dashboard/` based on `.env.example`
4. Add your ElevenLabs API key

---

## 🧊 Tech Stack

* **Arduino (C++)** — sensor data collection
* **Python** — serial communication & data bridge
* **React + Vite** — real-time dashboard
* **ElevenLabs API** — AI text-to-speech mascot

---

## 💡 Future Improvements

* Expanded sensor support
* Historical data playback and analysis
* Configurable AI mascot personalities
* Improved autonomy and navigation accuracy
* Deployment-ready backend
* Mobile-friendly dashboard

---

## 🏆 Built At

**UTRA Hacks 2026**
Team **6 Penguins**

---

## 👥 Team

* Ishaan Mittal
* Omar Saraj
* Elil Thiru
* Jaran Khalid
* Catherine Susilo
* Yuhan Xu

Just tell me 👀
```
