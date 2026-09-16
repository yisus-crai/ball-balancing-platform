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
- External 5 V power supply for the servo motors*
- Custom 2-DOF mechanical platform (designed via CAD + 3D-printed)
- 25 mm diameter stainless-steel ball


## Design


## Architecture


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
