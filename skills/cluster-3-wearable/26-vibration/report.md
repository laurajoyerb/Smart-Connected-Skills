#  Vibration Switch (Hardware Interrupts)

Author: Laura Joy Erb, 2019-10-21

## Summary
I wired up the tilt switch with the ESP32 and the console. Whenever the device is tapped, the console prints "tapped!" to the console, and the red onboard LED blinks. 

I used a 10K resistor pull-up resistor to wire up the sensor. I implemented simple debouncing to ensure that one tap only produced one "tapped!" on the console, and one blink of the LED. 

## Sketches and Photos
Here is how the sensor was wired up:

<img src="./images/vibrate.png" width="95%" />

Here is a video of the tilt sensor "tapping" to the console and lighting up the led:

https://drive.google.com/open?id=1bR67QHUcVXFzGTOX3F_TBsLGbo963x7V
