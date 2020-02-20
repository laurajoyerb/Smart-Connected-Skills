#  IR TX/RX

Author: Laura Joy Erb, 2019-11-19

## Summary
This skill used IR wireless transmitting and receiving to light up LEDs.

The IR LED transmits either "GREEN", "RED", "BLUE", or "OFF". The IR receiver receives the code, and in turn lights up either the Green, Red, or Blue led. The OFF command turns off all LEDs. 

The button triggers the transmission of a command. The transmitter will send the message until the receiver receives it. Once the receiver receives the command, it blocks further transmissions from the transmitter (until the next button press triggers a new round of transmissions). This ensures that the receiver receives the command from the transmitter. 

## Sketches and Photos
The video of the transmission (triggered by the button press) and the corresponding LED lighting up:

https://drive.google.com/open?id=1bJJ9AcXD907aFYOTM_R7Bim6c8ssuVWx

The wiring of the transmitter and receiver:
<img src="./images/ir_up.png" width="95%" />
<img src="./images/ir_side.png" width="95%"/>
