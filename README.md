# FE-Attendance-Counter
The software to make and run the custom Furnal Equinox event attendance counters. 

# Hardware
This uses a [Raspberry Pi Pico](https://www.raspberrypi.com/products/raspberry-pi-pico/), with a [Waveshare 2.23" OLED hat](https://www.waveshare.com/wiki/Pico-OLED-2.23) as a screen.

# Requirements
This project is built using the Raspberry Pi SDK found at https://github.com/raspberrypi/pico-sdk. This must be downloaded separately, it is not included. It also requires `cmake`.

# Compilation instructions
First, you need to set the path to the Raspberry Pi Pico c sdk with
`export PATH_TO_SDK=<Path to the sdk>`

Then, run the following:
```
mkdir build
cd build
cmake ..
make
```

You will then have a file called `main.uf2` that can be coppied to your Raspberry Pi Pico

Good luck :)
