#  Console I/O

Author: Laura Joy Erb, 2019-09-14

## Summary
I communicated between my laptop and the ESP32 via the USB interface using the console. The console accepts different commands and inputs based on three separate modes. 

Mode 1 is an LED toggling mode. A user input of "t" toggles the LED on and off. An input of "s" switches to the next mode. Any other input is printed and ignored. 

Mode 2 is an echo mode. The console will echo any string given as an input from the user. An input of a single "s" switches to the next mode.

Mode 3 is a decimal to hexadecimal converter. The user may input any integer, and the program outputs the hexadecimal equivalent. An input of "s" switches back to Mode 1. Any input other than integers is printed and ignored.

## Sketches and Photos
Console Output:

<img src="./images/Screen Shot 2019-09-14 at 2.19.16 PM.png" width="45%" />
