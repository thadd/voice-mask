# Voice Mask

This project contains arduino code for a voice activated mask. Almost everything is inspired and based on this Medium post: https://medium.com/@tglaiel/how-to-make-a-voice-activated-led-facemask-diy-guide-b300bc146f38

## Building the mask

* [Adafruit Flora](https://www.adafruit.com/product/659)
* [MEMS Silicon Mic](https://www.adafruit.com/product/2716) (the Sparkfun mic should work too)
* [Flexible LED Matrix](https://www.adafruit.com/product/2612) (generic versions from Amazon work as well)
* Battery pack with Adafruit connector. I use a 9V but you can get a li-po or rechargeable 9V.

1. Solder the data line on the matrix to one of the pins on the Flora.
2. Solder the 5v line on the matrix to the VBATT pin on the Flora.
3. Solder the DC pin on the mic to one of the pins on the Flora.
4. Solder the Vin pin on the mic to the 3.3v pin on the Flora.
5. Solder the ground from the matrix and mic to the GND on the Flora.
6. Add a battery pack, upload code (make sure pin settings match where you soldered stuff), insert into mask.

You may need to tweak the `MAX_NORMAL_SPEECH` constant to tune your mic. 

**Have Fun!**
