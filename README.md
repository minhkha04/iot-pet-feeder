# IoT-Based Smart Pet Feeding System

This is a smart pet feeding system designed using **ESP8266 (NodeMCU)** and controlled via the **Blynk mobile app**. It helps automate food and water dispensing for pets, allowing **manual, scheduled, and smart feeding modes**, while keeping real-time monitoring available to users remotely.

---

## Features

-  Automated food dispensing using **load cell** + **servo motor**
-  Smart water control using **water level sensor** + **relay** + **water pump**
-  Blynk app integration for real-time updates and manual control
-  Real-time weight tracking via **HX711**
-  Fully offline-capable hardware when pre-configured
-  3 Operation Modes:
  - **Manual Mode**: Control from app
  - **Scheduled Mode**: Trigger on preset time
  - **Smart Mode**: Based on weight + time

---

##  Hardware Architecture

- **Microcontroller**: NodeMCU ESP8266
- **Servo Motor**: Food dispenser control
- **Load Cell + HX711**: Measures food weight
- **Water Level Sensor**: Detects low water level
- **Relay Module**: Controls water pump
- **Water Pump**: Dispenses water
- **Power Supply**: 5V/9V DC depending on modules

 Pin mappings are defined inside the `.ino` source code.

---

## Circuit Schematic

Refer to the full schematic in `Final_Project_Presentation.pptx` or project report.

---

## System Workflow

1. User opens the **Blynk app**
2. Depending on selected mode:
   - Manual: User presses a button to trigger servo/pump
   - Scheduled: Automatically triggers at defined times
   - Smart: Triggers only when bowl is empty
3. System updates values to cloud via Wi-Fi
4. User receives notifications about feeding/water levels

---

## How to Run

1. Flash `Final_Project_Source_Code.ino` to **NodeMCU ESP8266** using Arduino IDE
2. Configure your **Wi-Fi credentials** + **Blynk Auth Token** in the `.ino` file
3. Open the **Blynk app**, create a new project:
   - Add buttons, gauges, virtual pins
   - Assign them to match pin mappings from the `.ino`
4. Power on the hardware with 5V or battery supply
5. Monitor and control pet feeding remotely!

---

## Folder Structure (Recommendation)

```
iot-pet-feeder/
├── README.md
├── source/
│   └── Final_Project_Source_Code.ino
├── docs/
│   ├── Final_Project_Report.pdf
│   ├── Final_Project_Presentation.pptx
│   └── Final_Project_Demo.txt
```

---

## Demo Video

Watch the project demonstration on YouTube:  
 [https://youtu.be/vp49y4P6AcY](https://youtu.be/vp49y4P6AcY)

---

## Notes

- Make sure to use a **reliable 5V power supply** for servo and pump.
- Replace `delay()` functions with `millis()` if you intend to scale this for production.
- Ensure Blynk virtual pins and Wi-Fi credentials are correctly mapped.

---
