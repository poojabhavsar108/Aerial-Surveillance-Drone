# 🚁 Aerial Surveillance Drone using ESP32-CAM and NEO-6M GPS

![Platform](https://img.shields.io/badge/Platform-ESP32--CAM-blue)
![Language](https://img.shields.io/badge/Language-Arduino-green)
![GPS](https://img.shields.io/badge/GPS-NEO--6M-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

## ⭐ Project Overview

An IoT-based **Aerial Surveillance Drone** developed to demonstrate **real-time wireless video streaming** and **GPS-based location tracking** using an **ESP32-CAM** and **NEO-6M GPS module**.

The project integrates a quadcopter platform with an onboard camera and GPS module, enabling users to monitor the surrounding environment through a live video feed while simultaneously viewing the drone's real-time location. The integrated software provides a web-based interface for viewing the live camera stream and GPS information.
This project was developed to explore the integration of embedded systems, wireless communication, IoT, and drone technology for surveillance applications.

---

## ⭐ Project Highlights

- ✔️ Live video streaming using ESP32-CAM over Wi-Fi
- ✔️ Real-time GPS location tracking using NEO-6M
- ✔️ Interactive web dashboard displaying camera feed and GPS telemetry
- ✔️ Google Maps integration for live drone location
- ✔️ Flight control using KK2.1.5 Flight Controller
- ✔️ Stable power supply using LM2596 Buck Converter and Arduino Uno
- ✔️ Modular software architecture with separate camera testing and integrated system code
- ✔️ Prototype successfully demonstrated with flight and live camera streaming

---

## 🎯 Project Objectives

The primary objectives of this project are:

- Develop a quadcopter capable of performing basic aerial surveillance.
- Implement real-time live video streaming using the ESP32-CAM module.
- Integrate the NEO-6M GPS module to provide the drone's current geographical location.
- Design a web-based dashboard to display live video, GPS coordinates, and telemetry information.
- Enable users to locate the drone through direct Google Maps integration.
- Gain practical experience in integrating embedded hardware, wireless communication, and IoT technologies into a complete working system.

---

## 🛠 Hardware Components

| Component | Purpose |
|-----------|---------|
| F450 Quadcopter Frame | Drone chassis and structural support |
| A2212 1000KV BLDC Motors (×4) | Generate thrust for flight |
| 30A ESCs (×4) | Control motor speed |
| KK2.1.5 Flight Controller | Flight stabilization and control |
| FS-CT6B Transmitter & Receiver | Wireless flight control |
| ESP32-CAM | Live video streaming over Wi-Fi |
| NEO-6M GPS Module | Real-time location tracking |
| Arduino Uno | Stable power supply and module interfacing |
| LM2596 Buck Converter | Voltage regulation |
| 3S Li-Po Battery | Power source for the drone |

### Hardware Components Overview

<p align="center">
  <img src="Hardware/Components_Used.png" width="900">
</p>

---

## 🏗️ System Architecture

The Aerial Surveillance Drone consists of two major subsystems:

- **Flight Control System** – Responsible for maintaining stable flight and controlling the drone's movement.
- **Surveillance System** – Responsible for live video streaming and real-time GPS tracking.

The RF transmitter sends control commands to the KK2.1.5 Flight Controller, which stabilizes the quadcopter and controls the BLDC motors through the Electronic Speed Controllers (ESCs). An ESP32-CAM mounted on the drone captures live video and streams it wirelessly over Wi-Fi. Simultaneously, the NEO-6M GPS module continuously acquires the drone's geographical coordinates and transmits them to the web dashboard, allowing users to monitor both the live video feed and the drone's location.

### System Block Diagram

<p align="center">
  <img src="Hardware/Block_diagram.png" width="900">
</p>

---

## ⚙️ Software Workflow

The embedded software initializes the ESP32-CAM and NEO-6M GPS module before connecting to a Wi-Fi network. Once connected, the ESP32-CAM continuously captures video frames while the GPS module receives satellite data and updates the drone's location. Both the live camera stream and GPS telemetry are transmitted to a web-based dashboard, enabling users to monitor the drone remotely in real time.

### Software Flowchart

<p align="center">
  <img src="Hardware/Software_Flowchart.png" width="500">
</p>

---

## ⚙️ Working Principle

The complete working of the Aerial Surveillance Drone is described below:

1. The drone is powered using a **3S Li-Po battery**, while the **LM2596 Buck Converter** and **Arduino Uno** provide a stable power supply to the ESP32-CAM and NEO-6M GPS module.

2. The **KK2.1.5 Flight Controller** receives control signals from the **FS-CT6B Transmitter and Receiver** and stabilizes the drone by controlling the speed of the four BLDC motors through the ESCs.

3. Once powered, the **ESP32-CAM** initializes the camera module, connects to the configured Wi-Fi network, and starts a web server for live video streaming.

4. Simultaneously, the **NEO-6M GPS module** continuously receives satellite signals and provides the drone's real-time geographical coordinates to the ESP32-CAM.

5. The integrated software processes both the camera feed and GPS data, creating a web-based dashboard that displays:
   - Live video stream from the ESP32-CAM
   - Latitude and Longitude
   - GPS Fix Status
   - Satellite Count
   - Altitude
   - Ground Speed
   - Course (Heading)
   - HDOP
   - GPS Date and Time
   - Direct Google Maps link for location tracking

6. Users connected to the same Wi-Fi network can access the dashboard through a web browser to monitor both the live aerial view and the drone's current location in real time.

This integration enables the drone to perform basic aerial surveillance while providing location information for monitoring and tracking purposes.

---

## 📂 Repository Structure

```text
Aerial-Surveillance-Drone/
│
├── Hardware/
│   ├── Block_diagram.png
│   ├── Components_Used.png
│   └── Software_Flowchart.png
│
├── Images/
│   ├── Drone_Prototype_1.png
│   ├── Drone_Prototype_2.jpg
│   └── GPS_Demonstration.jpg
│
├── Software/
│   ├── Camera_Test/
│   │   └── CameraWebServer.ino
│   │
│   └── Final_Integrated_System/
│       └── ESP32_CAM_GPS_System.ino
│
├── Videos/
│   ├── Flight_Demonstration.mp4
│   └── Camera_Stream_Demonstration.mp4
│
├── README.md
└── LICENSE
```

### Repository Contents

- **Hardware/** – System block diagram, software flowchart and hardware components used.
- **Images/** – Prototype photographs and GPS demonstration images.
- **Software/** – ESP32-CAM camera testing code and the final integrated Camera + GPS system.
- **Videos/** – Drone flight demonstration and live camera streaming demonstration.
- **README.md** – Complete project documentation.
- **LICENSE** – Open-source project license.

---

## 📸 Project Demonstration

### Drone Prototype

<p align="center">
  <img src="Images/Drone_Prototype_1.png" width="420">
  <img src="Images/Drone_Prototype_2.jpg" width="420">
</p>

The developed prototype integrates the flight control system, ESP32-CAM module, NEO-6M GPS module, Arduino Uno, and LM2596 Buck Converter on an F450 quadcopter frame. The prototype was successfully assembled and tested to demonstrate stable flight, live video streaming, and GPS-based location tracking.

---

## 🎥 Demonstration Videos

### 🚁 Flight Demonstration

The drone was successfully tested for take-off, hovering, and controlled flight to verify the stability of the quadcopter platform.

📹 **Flight Demonstration**

*(Videos/Flight_Demonstration.mp4)*

---

### 📷 ESP32-CAM Live Stream Demonstration

The ESP32-CAM mounted underneath the drone captures a live downward-facing aerial view and streams it wirelessly over Wi-Fi through the onboard web server.

📹 **Camera Stream Demonstration**

*(Videos/Camera_Stream_Demonstration.mp4)*

---

## 💻 Software and Technologies Used

### Programming Language
- Arduino (C/C++)

### Development Environment
- Arduino IDE

### Hardware Modules
- ESP32-CAM
- NEO-6M GPS Module
- Arduino Uno
- KK2.1.5 Flight Controller

### Libraries
- esp_camera
- WiFi
- WebServer
- TinyGPS++
- HardwareSerial

---

## ✨ Key Features

- Live wireless video streaming using ESP32-CAM
- Real-time GPS location tracking
- Integrated Camera and GPS web dashboard
- Google Maps location integration
- Stable quadcopter flight using KK2.1.5 Flight Controller
- Modular software architecture
- Portable aerial surveillance prototype

---

## 📚 Learning Outcomes

Through this project, I gained practical experience in:

- Drone hardware assembly and integration
- Working with the ESP32-CAM and NEO-6M GPS module
- Basic wireless video streaming
- Prototype testing and debugging
- Understanding the integration of multiple embedded hardware modules

---

## 🚀 Future Improvements

Possible future improvements include:

- Improve camera streaming performance and reduce latency.
- Enhance GPS tracking accuracy and dashboard functionality.
- Optimize power management for longer flight time.

---

## 🙏 Acknowledgements

This project was developed as part of my academic learning in Electronics and Communication Engineering.

I sincerely thank my college, faculty members, and mentors for their continuous guidance, encouragement, and support throughout the development of this project.

---

## 👩‍💻 Author

**Pooja Bhavsar**

Electronics and Communication Engineering Student

Interested in VLSI,  Embedded Systems, and Digital Design.

---

## 📄 License

This project is licensed under the MIT License.

See the LICENSE file for more details.

