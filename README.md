
This repository contains a simple example of how to use the the PCIe Hard IP in the Cyclone® 10 GX.

This example is designed to work with the Intel® Cyclone® 10 GX FPGA Development Kit.

User LEDs 0-3 are controllable with bits 0-3 of the first byte of BAR0.

A simple Linux kernel module is included to expose the LEDs as /sys/class/leds/fpga_board::user0-3
