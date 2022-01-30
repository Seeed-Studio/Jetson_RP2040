# Jetson_RP2040

This is RP2040 firmware for Jetson.

How to use
------------
Step1: Get SDK
```
git clone https://github.com/raspberrypi/pico-setup.git
./pico_setup.sh
```

Step2: Build
```
mkdir build
cd build
cmake ..
make -j4
```

Step3: Flash
```
gdb-multiarch Jetson_RP2040.elf
(gdb) load
(gdb) monitor reset init
(gdb) continue
```
