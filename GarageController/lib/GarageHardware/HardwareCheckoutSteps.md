
# To use your *Proxy Hardware*:
1. Ensure that `GarageHardwareProxy.cpp` starts with a `#if 1`
1. Ensure that `GarageHardwareReal.cpp` starts with a `#if 0`

# To use the *Real Hardware*:
1. Ensure that `GarageHardwareProxy.cpp` starts with a `#if 0`
1. Ensure that `GarageHardwareReal.cpp` starts with a `#if 1`

# Checkout Process:
1. Have your work reviewed by a TA using the simulated environment.
1. Search your main .ino and verify that it doesn't ever call `pinMode()`, `digitalWrite()`, `digitalRead()`, `analogWrite()`, `analogRead()`, or any other I/O other than method of the Serial object, like `Serial.println`.
1. Change the files to disable use of your proxy code and enable use of the code for the real hardware.
1. Reprogram your Photon.  Show a TA/Instructor that you have made the required changes and successfully uploaded your code.
1. Remove your Photon from your test setup.  (When you reinstall it later be sure you line it up correctly! Misalignment could do damage to I/O devices)
1. Work with a TA/Instructor to test your work with a mini garage.  Good Luck!
