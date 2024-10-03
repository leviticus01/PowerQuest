Updates as of 20241003:

-Defined all GPIO pins on the ESP32 and set appropriate pin modes

-Wrote and verified (software only) the follow code sections:
-loop() runs perpetually. It will alawys run and first checks the hall sensor state. If the hall sensor is active, it will run code for the selected peripheral. Otherwise nothing will happen.
-check_hall() is called in loop() and checks the state of the hall sensor. If the hall sensor state goes from inactive to active, it calls play_audio().
-motion_sense() is called in loop() when the hall sensor is active and the peripheral is selected. 
-play_audio() is not written yet. It will contain the functionality for the speaker and audio.
-run_motor() is not written yet. It will contain the functionality for the motor/potentiometer peripheral.
