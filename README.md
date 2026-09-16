# 2-DOF Ball-Balancing Platform


## About
A two-degree-of-freedom ball-balancing platform designed, built, and programmed as a personal engineering project.

The platform uses a resistive touchscreen to measure the ball position and two servomotors to tilt the surface. Two independent closed-loop PID controllers stabilize the ball near the center of the platform. 


## Project Goals

- Apply classical control concepts to a physical system.
- Design and manufacture the mechanical structure.
- Implement real-time position measurement and filtering.
- Develop and tune a closed-loop controller on an Arduino.
- Explore future control approaches such as state-space control and Kalman filtering.


## Hardware & Components

- Arduino UNO
- 2 × MG995 servo motors
- 8 inch 4-wire resistive touchscreen
- External 5 V power supply for the servo motors
- Custom 2-DOF mechanical platform (designed via CAD + 3D-printed)
- 25 mm diameter stainless-steel ball


## Design

The platform was designed as a two-degree-of-freedom mechanism capable of tilting independently around the X and Y axes.

The mechanical structure consists of:

- A fixed base.
- A moving platform that supports the resistive touchscreen.
- Two MG995 servomotors, one for each axis.
- Mechanical linkages that convert servo rotation into platform tilt.
- 3D-Printed structural components.
- A ball used as the controlled object.

All structural components were designed in CAD using Autodesk Fusion and manufactured by 3D printing in PLA. The design was iterated to achieve adequate rigidity, servo clearance, and a suitable range of platform motion. The final assembly uses bolts and nuts to join the printed parts, servomotors and base structure. The touchscreen is fitted into the tilting platform with minimal clearance to prevent displacement, avoiding the use of bolts that could apply unwanted stress to the touchscreen.

The platform tilt was limited to approximately ±10° on each axis. This constraint reduces excessive ball acceleration, keeps the ball in contact with the touchscreen, and prevents mechanical interference.


##  Software Architecture

The Arduino firmware is organized around a repeated sensing, filtering, control, and actuation loop.

1. **Position measurement**  
   The Arduino reads the X and Y coordinates from a 4-wire resistive touchscreen.

2. **Signal filtering**  
   A median filter rejects outliers from the touchscreen measurements. An Exponential Moving Average (EMA) then smooths the remaining noise.

3. **Control computation**  
   Independent PID controllers calculate the required correction for the X and Y axes. Each controller stores its own gains, filtered position, previous error, integral state, derivative state, and output limits.

4. **Actuation**  
   The controller outputs are converted into servo commands relative to the mechanical neutral position.

5. **Safety handling**  
   If the ball is not detected for a defined period, both servos return to the neutral position and the controller states are reset.

Main Code Components:

- `AxisControl`: Stores the state and parameters associated with one control axis.
- `median()`: Calculates the median of recent touchscreen measurements.
- `ema()`: Applies the exponential moving average filter.
- `PID()`: Calculates the control output for an axis.
- `restartAxis()`: Resets controller states after ball detection is lost.


## Troubleshooting

Several practical issues arose during development and were addressed as follows:

- **Excessive serial output slowed down the control loop.**  
  Frequent `Serial.print()` calls introduced delays and degraded the controller performance.  
  - **Solution:** Reduce or disable serial output during normal operation. Use `Serial.print()` only for debugging.

- **Inadequate servo power supply reduced actuator performance.**  
  Powering the servos from the Arduino 5 V pin resulted in slow and unreliable motion.  
  - **Solution:** Power the servos from a suitable external 5 V supply and connect its ground to the Arduino GND.

- **Derivative kick occurred when the ball was first detected.**  
  The derivative term produced a large output because the previous error was not initialized.  
  - **Solution:** Initialize the controller state after obtaining the first valid position measurements, then enable derivative control.

- **Filtering introduced a trade-off between noise reduction and response delay.**  
  Excessive filtering made the position estimate smoother but delayed the control action.  
  - **Solution:** Tune the median-filter window, EMA coefficient, and sampling interval to balance noise rejection and responsiveness.
