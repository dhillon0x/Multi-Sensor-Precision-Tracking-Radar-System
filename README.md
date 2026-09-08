# Advanced Multi-Sensor Tracking Radar & Autonomous Target Acquisition System

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Arduino](https://img.shields.io/badge/Platform-Arduino-blue.svg)](https://www.arduino.cc/)
[![Status](https://img.shields.io/badge/Project-Active-success.svg)]()

---

## 🎥 Inspiration, Conceptual Roots & Origin

Every robust engineering project begins with a spark of curiosity, and this advanced multi-sensor tracking radar system is no exception. The core conceptual architecture, mechanical servo arrangement, and foundational telemetry layout were heavily inspired by open-source maker channels and embedded systems tutorials available online. Specifically, the mechanical realization of a rotating sensor mast paired with real-time serial plotting was adapted from foundational community guides such as the [Arduino Radar Project Tutorial on YouTube](https://www.youtube.com/watch?v=dQw4wWgXcQ). 

By studying these reference frameworks, we identified a critical limitation in standard hobbyist radar builds: they rely exclusively on a single sweeping ultrasonic sensor, which is highly prone to false-positive triggers, environmental noise, and poor angular resolution at longer distances. This realization drove the design evolution of our project. We sought to bridge the gap between simple hobby electronics and industrial-grade sensor fusion by engineering a dual-stage tracking architecture. Instead of just copying a basic tutorial, we expanded the system logic to incorporate a secondary precision sensor interface, multi-LED status feedback hierarchies, active acoustic warning sequences, and advanced stability counter algorithms to filter out background interference.

---

## 🏗️ Comprehensive System Overview & Engineering Architecture

The Advanced Multi-Sensor Tracking Radar is engineered to function as an autonomous, dual-mode surveillance and targeting node. In complex physical environments, single-sensor arrays often fail due to acoustic reflections off walls, ambient thermal interference, or beam dispersion. To solve this, our system splits operation into two distinct, highly optimized operational states: **Wide Sweep Mode** and **Precision Target Lock & Tracking Mode**.

### 1. Phase 1: Wide Sweep Mode (Environmental Surveillance)
During initialization and normal patrol states, the system commands an SG90 micro-servo motor to sweep a continuous $180^\circ$ arc, rotating back and forth at a controlled, smooth velocity. 
* **Data Collection:** At each angular increment, the primary HC-SR04 ultrasonic transducer emits a high-frequency acoustic pulse ($40\text{ kHz}$) and measures the echo return time. 
* **Stability Filtering:** To prevent static environmental elements—such as room walls, furniture edges, or background clutter—from locking up the radar, the firmware implements a strict stability counter (`stableCount >= 3`). An object must be continuously detected within a tight proximity threshold ($2\text{ cm}$ to $30\text{ cm}$) across multiple consecutive sweep cycles before the system validates its presence. This algorithmic filter eliminates transient noise and ensures high-confidence target acquisition.
* **Visual & Audio Feedback:** While sweeping safely, the system illuminates a Green LED to denote normal patrol status, while an orange LED pulses slowly and a low-frequency periodic beep alerts operators that the sector is active.

### 2. Phase 2: Precision Target Lock & Tracking Mode (Active Engagement)
Once a target passes the multi-sample stability filter, the radar transitions instantly into Locked Mode. 
* **Dynamic Error Correction:** The system calculates the positional error between the current servo angle and the recorded target angle (`int error = targetAngle - angle`). Instead of snapping violently, it applies smooth proportional stepping ($6^\circ$ adjustments) to center the sensor array directly on the target.
* **Dual-Sensor Validation:** While the primary ultrasonic sensor maintains spatial awareness, a dedicated long-range analog interface (designed to integrate high-precision LiDAR or ToF sensors) continuously monitors the core vector. If the target attempts to evade or moves out of the primary sweep window ($>55\text{ cm}$) *and* the long-range sensor drops signal, the system executes an intelligent timeout protocol, safely resetting back to Wide Sweep Mode.
* **Proportional Proximity Alerting:** As objects close the distance to the radar array, the system escalates its warning protocols. A blue LED shifts its blink frequency dynamically using map functions (`map(safeDist, 5, 50, 100, 600)`), causing it to flash faster as proximity decreases. If an object breaches a critical safety threshold ($<5\text{ cm}$), a white warning LED strobes rapidly alongside a multi-tone high-frequency piezo buzzer alarm sequence.

---

## 🧰 Detailed Bill of Materials (BOM) & Cost Breakdown

Transparency in hardware prototyping is essential. Below is the complete itemized component breakdown required to replicate this build, including estimated market pricing:

| Component | Description & Specifications | Estimated Cost (USD) |
| :--- | :--- | :--- |
| **Arduino Uno / Nano** | ATmega328P microcontroller board running the embedded C++ control loops | $5.00 - $22.00 |
| **SG90 Micro Servo Motor** | Lightweight $180^\circ$ rotation servo responsible for precise angular positioning | $3.50 |
| **HC-SR04 Ultrasonic Sensor** | Dual-transducer acoustic distance measuring module ($2\text{ cm}$ to $400\text{ cm}$ range) | $2.00 |
| **Long-Range Sensor Module** | Precision analog/UART distance sensor (e.g., TF-Luna LiDAR or Sharp GP2Y0A21YK0F ToF) | $12.00 - $30.00 |
| **Active Piezo Buzzer** | 5V electronic audio signaling module for multi-tone acoustic alerts | $0.75 |
| **LED Indicator Array** | 5x Industrial LEDs (Green, Red, Orange, Blue, White) with current-limiting resistors | $1.00 |
| **Prototyping Infrastructure** | Full-size solderless breadboard, flexible jumper wires, and external power regulation | $3.00 |
| **Total Estimated Build Cost** | | **~$27.25 - $62.25** |

---

## 🔌 Comprehensive Hardware Pinout & Wiring Specifications

Accurate pin allocation is vital for preventing bus contention and ensuring stable PWM signal generation. The system maps its peripherals across digital and analog rails as follows:

| Component Peripheral | Microcontroller Pin | Signal Type | Functional Description |
| :--- | :--- | :--- | :--- |
| **Servo Motor Signal** | Pin 9 | PWM Digital Output | Drives internal motor commutation for smooth $180^\circ$ radar sweeps. |
| **Ultrasonic TRIG** | Pin 7 | Digital Output | Triggers the transmission of the $40\text{ kHz}$ ultrasonic burst. |
| **Ultrasonic ECHO** | Pin 8 | Digital Input (`pulseIn`) | Measures the return echo duration for precise time-of-flight distance calculation. |
| **Long-Range Sensor** | Pin A0 | Analog Input | Receives high-precision distance voltage metrics from secondary LiDAR/ToF hardware. |
| **Green Indicator LED** | Pin 2 | Digital Output | Illuminated continuously during Phase 1 Wide Sweep patrol state. |
| **Red Indicator LED** | Pin 3 | Digital Output | Illuminated continuously during Phase 2 Target Lock tracking state. |
| **Orange Status LED** | Pin 4 | Digital Output | Pulsing heartbeat indicator varying in rate based on system state. |
| **Piezo Buzzer** | Pin 5 | PWM / Tone Output | Drives variable frequency acoustic warning tones and alarm sequences. |
| **Blue Proportional LED** | Pin 13 | Digital Output | Blinking frequency dynamically scales relative to target proximity. |
| **White Critical LED** | Pin 12 | Digital Output | Strobes rapidly during critical proximity breaches ($<5\text{ cm}$). |

---

## 🌐 Real-World Applications Across Engineering Fields

Although built as an advanced prototyping platform, the underlying engineering principles of this project directly translate into commercial and industrial sectors:

1. **Aerospace & Unmanned Aerial Systems (UAS):** The dual-stage acquisition logic mirrors short-range obstacle detection systems and collision avoidance radar pods used on autonomous drones and quadcopters operating in GPS-denied environments.
2. **Autonomous Ground Vehicles (AGVs) & Logistics:** Warehouse mobile robots utilize similar multi-sensor fusion architectures to map static warehouse pillars while dynamically tracking moving personnel or forklifts in real time.
3. **Industrial Robotics & Safety Envelopes:** Automated manufacturing cells deploy perimeter scanning radar arrays to monitor restricted workspaces, slowing or halting industrial robotic arms if human operators breach safety boundaries.
4. **Environmental Monitoring & Telemetry:** Automated stationary nodes utilize similar continuous-sweep telemetry loops for structural health surveillance, liquid level tracking, or localized spatial mapping in hazardous zones.

---

## 🤖 AI Collaboration & Prototyping Note
Developed iteratively with advanced AI pair programming tools to structure clean non-blocking timing routines, optimize control loops for dual-sensor arbitration, and maintain modular scalability for hardware integration.
