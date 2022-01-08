# FE-Attendance-Counter
The software to make and run the custom Furnal Equinox event attendance counters. 

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
