#  Node.js

Author: Laura Joy Erb, 2019-10-08

## Summary
In this skill, I used Node.js to implement a simple server to display sensor data. 

First, I went through the hello world tutorial and implemented several node modules to familiarize myself with node.

For displaying sensor data, I first wired up the ultrasonic sensor using the ADC output. I used a c file to read the sensor data and write the data to the serial port using printf(). On the other side, I created a node.js file to act as a server and read the sensor data from the serial port. The serial.js file successfully reads the data from the serial port and prints it again to the console as a demonstration. 

## Sketches and Photos
Here is a video of the node program reading and displaying the ultrasonic sensor data to the console:

https://drive.google.com/file/d/1ammDrIx_m2vOkDsiX8dcr_-Tzi_1fVtA/view?usp=sharing

## Challenges
One of the challenges I faced in this skill was making sure that the node_skill.c file could communicate seamlessly with the serial.js file. I spent awhile trying to get the ESP32 to read the ultrasonic sensor via UART, and I also tried to write data to the serial port using UART0. However, this proved very difficult, and, in the end, not necessary. I instead used a printf() function to write to the console, and the serial.js file was able to read that output from the serial port and handle it on that side.

-----

## Reminders
- Repo is private
