```markdown
# ESP32 Web-Controlled Firework Thrower

[![Platform](https://img.shields.io/badge/platform-ESP32-blue)]()
[![Language](https://img.shields.io/badge/language-C%2B%2B-orange)]()
[![License](https://img.shields.io/badge/license-MIT-green)]()

## Overview

This project turns an ESP32 into a **WiFi web server** that lets you control three things from any phone or computer browser:

- **Servo motor** (angle from 0° to 180° via a slider)
- **Stepper motor** (28BYJ-48 with ULN2003 driver) – move by precise steps or full turns, change speed (RPM) on the fly
- **A toggle device** (like an LED or relay) – turn it ON/OFF with a coloured button

All controls are on one simple HTML page. The stepper moves asynchronously (one step per loop), so the web interface never freezes.

I built this as a general‑purpose motor controller for small robotics projects, but you can use it for anything that needs remote movement switching.

## Features

- **Servo** – slider sends new angle immediately
- **Stepper** – buttons for ±1/16, ±1/4, ±full turn, plus a STOP button
- **Stepper speed control** – slider from 1 to 40 RPM (real‑time)
- **Toggle device** – ON/OFF button that remembers state, changes colour
- **Responsive web page** – works on smartphones, tablets, desktops
- **WiFi connection** – device hosts its own access point or connects to your router

## Hardware Required

| Component | Quantity | Notes |
|-----------|----------|-------|
| ESP32 development board | 1 | any with WiFi |
| Servo motor (e.g. SG90, MG995) | 1 | signal pin to GPIO13 |
| 28BYJ‑48 stepper motor | 1 | 5V version |
| ULN2003 driver board | 1 | connects to stepper |
| LED + 220Ω resistor (for toggle demo) | 1 | optional |
| Jumper wires | many | as needed |
| USB cable | 1 | for power and programming |

## Wiring Diagram

### Servo
- **Signal** → GPIO 13
- **VCC** → 5V
- **GND** → GND

### Stepper (ULN2003 → ESP32)
| ULN2003 pin | ESP32 GPIO |
|-------------|------------|
| IN1 | 19 |
| IN2 | 18 |
| IN3 | 5 |
| IN4 | 15 |
| + | 5V (external power recommended) |
| - | GND |

*Note:* The 28BYJ‑48 can draw more current than the ESP32’s 5V pin can supply. For reliable operation, use a separate 5V power supply (e.g. a phone charger) and connect its GND to the ESP32 GND.

### Toggle device (LED example)
- **LED anode (long leg)** → 220Ω resistor → GPIO 12
- **LED cathode (short leg)** → GND

## Software Setup

1. **Install Arduino IDE** – from [arduino.cc](https://www.arduino.cc/)
2. **Add ESP32 board support** –  
   File → Preferences → Additional Boards Manager URLs → add:  
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`  
   Then Tools → Board → Boards Manager → search "ESP32" → install.
3. **Install required libraries** (all are built‑in or available via Library Manager):
   - `WiFi.h` (comes with ESP32 package)
   - `ESP32Servo.h` (comes with ESP32 package)
   - `Stepper.h` (built‑in)

4. **Get the code** – copy the full sketch from this repository (`motor_control.ino`).
5. **Change WiFi credentials** – edit these lines near the top:
   ```cpp
   const char* ssid = "YourWiFiName";
   const char* password = "YourWiFiPassword";
```

1. Upload – select the correct ESP32 board and port, then click Upload.

How to Use

1. After upload, open the Serial Monitor (115200 baud). The ESP32 will show its IP address (e.g. 192.168.1.123).
2. On any device connected to the same WiFi network, open a web browser and go to http://<ESP32_IP>.
3. You’ll see three control panels:
   · Servo – drag the slider, the servo moves immediately.
   · Stepper – click any turn button; the motor moves the requested amount.
          Use the speed slider to change how fast it turns.
          Click STOP to cancel any remaining movement.
   · Device Control – click the large button to turn the LED (or relay) ON/OFF. The button colour changes to green (ON) or red (OFF).

Code Structure

· The web page is sent as one big HTML string – not the most elegant, but it works and keeps everything in one file.
· Stepper movement is queued: pressing a button sets stepperStepsToMove. In every loop(), the ESP32 moves one step towards zero. This way the web server never blocks.
· The toggle device state is stored in deviceState and survives page reloads (the server remembers it).

Troubleshooting

Problem Likely fix
WiFi won’t connect Check SSID and password; move ESP32 closer to router.
Servo jitters Use a separate 5V power supply for the servo.
Stepper doesn’t move Check the pin order (IN1, IN3, IN2, IN4 – yes, the order is correct for this driver).
Stepper moves wrong direction Swap any two of the four wires (e.g. IN1 and IN2).
Web page loads slowly The ESP32 is busy moving the stepper one step at a time – it’s normal.
Device toggles but button doesn’t change colour The page auto‑reloads after toggle; wait a second or refresh manually.

Future Improvements

· Add a real‑time chart of stepper position.
· Control multiple servos or steppers.
· Add a simple password to the web interface.
· Use WebSockets for smoother slider feedback.

License

This project is open‑source under the MIT License.

Author

George Tomeh
