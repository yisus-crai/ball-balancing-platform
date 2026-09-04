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
 
 * Note: the servo power supply and Arduino must share a common GND reference. 

## Design

## Architecture


## Troubleshooting

This project involved several practical issues, that were solved:

- Excessive Serial.print() output introduced control-loop delays and degraded performance. Solution: reduce/eliminate transmitted data. Use only Serial.print() for debugging.
- Operating servo motors powered by Arduino 5V pin significantly reduced their response speed. Solution: suitable external power supply connected to Arduino ground.
- Derivative action requires careful initialization to avoid derivative kick when the ball is first detected.
- Filtering must reduce measurement noise without adding excessive delay.


