# ESP32 Obstacle-Avoiding Car 🚗🤖

An autonomous obstacle-avoiding robot car built with an **ESP32**, an **HC-SR04 ultrasonic sensor** mounted on a **servo**, and an **L298N motor driver**. The car drives forward, detects obstacles in its path, scans left and right to find the clearest direction, and steers itself around them.

## Demo

The car continuously:
1. Drives forward while the path is clear.
2. Stops when an obstacle is detected within range.
3. Sweeps the ultrasonic sensor left and right to measure distance on both sides.
4. Turns toward whichever side is clearer.
5. If both sides are blocked, reverses and turns around.

## Features

- Real-time obstacle detection using an HC-SR04 ultrasonic sensor
- Servo-mounted sensor for left/right scanning (no need for two fixed sensors)
- PWM-based motor speed control via ESP32's LEDC peripheral
- Tunable stop distance, speed, and turn duration
- Serial output for live distance readings (debugging/tuning)

## Hardware Requirements

| Component | Qty |
|---|---|
| ESP32 Dev Board | 1 |
| L298N Motor Driver Module | 1 |
| DC Gear Motors + Wheels | 2 or 4 |
| Robot Car Chassis + Caster Wheel | 1 |
| HC-SR04 Ultrasonic Sensor | 1 |
| SG90 Micro Servo | 1 |
| Battery Pack (7.4V–12V) for motors | 1 |
| Jumper Wires | as needed |
| Resistors (1kΩ + 2kΩ) for voltage divider | 2 |

## Wiring / Pinout

| Component | Pin | ESP32 Pin |
|---|---|---|
| L298N | ENA | GPIO 14 (PWM) |
| L298N | IN1 | GPIO 27 |
| L298N | IN2 | GPIO 26 |
| L298N | IN3 | GPIO 25 |
| L298N | IN4 | GPIO 33 |
| L298N | ENB | GPIO 32 (PWM) |
| HC-SR04 | TRIG | GPIO 5 |
| HC-SR04 | ECHO | GPIO 18 (via voltage divider) |
| Servo | Signal | GPIO 13 |
| L298N | 12V/VCC | Battery + |
| L298N | GND | Battery − and ESP32 GND (common ground) |
| ESP32 | VIN/5V | Regulated 5V |

> ⚠️ **Warning:** The HC-SR04's ECHO pin outputs 5V logic. Feeding it directly into an ESP32 GPIO (3.3V logic) can damage the pin. Use a voltage divider (e.g., 1kΩ + 2kΩ resistors) to step it down to ~3.3V before connecting to GPIO 18.

> ⚠️ Power the motors from the battery pack — **not** from the ESP32's onboard 3.3V regulator, which cannot supply enough current.

## Software Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) (or PlatformIO)
- ESP32 board support package ([installation guide](https://github.com/espressif/arduino-esp32))
- [ESP32Servo library](https://github.com/madhephaestus/ESP32Servo) (install via Library Manager)

## Installation & Upload

1. Clone this repository:
   ```bash
   git clone https://github.com/Bimal-Web/-ESP32-Obstacle-Avoiding-Car.git
   cd -ESP32-Obstacle-Avoiding-Car
   ```
2. Open `esp32_obstacle_car.ino` in the Arduino IDE.
3. Install the ESP32 board package via **Boards Manager** (search "esp32").
4. Install **ESP32Servo** via **Library Manager**.
5. Select your ESP32 board and correct COM port under **Tools**.
6. Wire the hardware according to the pinout table above.
7. Click **Upload**.
8. Open the **Serial Monitor** at `115200` baud to view live distance readings.

## Configuration

Tune these constants at the top of the sketch to match your hardware:

```cpp
const int STOP_DISTANCE = 20;   // cm - distance at which the car stops and reroutes
const int SPEED = 200;          // motor speed, 0-255
const int TURN_TIME = 500;      // ms - how long it turns, tune for ~90° turns
```

## Project Structure

```
-ESP32-Obstacle-Avoiding-Car/
├── esp32_obstacle_car.ino   # Main Arduino sketch
└── README.md                 # This file
```

## Troubleshooting

| Issue | Possible Cause |
|---|---|
| Car doesn't move | Check motor driver power (separate from ESP32), common ground |
| Erratic distance readings | Missing/incorrect voltage divider on ECHO pin, sensor wiring |
| Car turns the wrong way | Swap motor direction pins (IN1–IN4) or adjust logic |
| Servo doesn't move | Confirm ESP32Servo library installed, check signal wire on GPIO 13 |
| ESP32 resets randomly | Insufficient/shared power supply — use separate supplies for logic and motors |

## Future Improvements

- [ ] Add Wi-Fi/Bluetooth remote control
- [ ] Add a camera module for vision-based navigation
- [ ] Add wheel encoders for more precise turning
- [ ] Add a mobile app / web dashboard for live monitoring

## Contributing

Contributions, issues, and feature requests are welcome. Feel free to open a pull request or issue.
