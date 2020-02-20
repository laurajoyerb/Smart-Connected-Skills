#  Ultrasonic Range Sensor

Author: Laura Joy Erb, 2019-10-05

## Summary
I wired up a Maxbotics Range Finder ultrasonic sensor and used it to sense distance. I used the analog voltage output of the ultrasonic sensor to read the raw values. I converted the raw values to a distance in mm by multiplying the raw units by 5. The ultrasonic was sampled every 10ms, and the average was printed to the console every second. The ultrasonic provides valid distance values for distances greater than 30cm. 

## Sketches and Photos
Here is a video of the ultrasonic displaying distances to the console. I smoothly moved my hand from immediately above the sensor to a standing height. I then removed my hand, and the last readings are the distance from the sensor to the ceiling. 

https://drive.google.com/file/d/1CcJvPIoJREx96ecbeUqacsCK3uf714t2/view?usp=sharing

## Modules, Tools, Source Used in Solution
https://www.maxbotix.com/documents/HRLV-MaxSonar-EZ_Datasheet.pdf
