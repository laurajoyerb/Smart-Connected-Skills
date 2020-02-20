#  RTOS Tasks

Author: Laura Joy Erb, 2019-09-20

## Summary

In this task, I used the real-time OS and tasks to achieve interaction between button presses, LEDs, and the alphanumeric display. The four LEDs count in binary either up or down, depending on the mode. The current mode is displayed by the alphanumeric display. The mode can be changed at any time by pressing the button. When the counter reaches 15, it wraps back around to 0, and vice versa when counting down.

The code uses three tasks to control the functionality. One task displays the current mode to the alphanumeric display, one task controls the LED counting, and one task handles the button press and changing of the global variable for mode. 

## Sketches and Photos

Counting Up:

<img src="./images/up.png" width="45%" />


Counting Down:

<img src="./images/down.png" width="45%" />


-----

## Reminders
- Repo is private
