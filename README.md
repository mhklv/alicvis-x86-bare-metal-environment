# Introduction

A freestanding environment capable of running on a bare metal
PC-compatible hardware in protected mode. Implemented APIs include
hard drive interaction (with ATA PIO Mode), keyboard input,
programmable interval timer, interrupt controller, drawing in 24 bit
color depth mode, drawing PSF-fonts and BMP images.

This could be used as a base for various utilities that need direct
access to hardware without any disturbance from the OS like memory
testers, bootloaders, some recovery software, etc. Or maybe for making
a size-constrained
[demoscene](https://en.wikipedia.org/wiki/Demoscene).


# Booting

Can be booted in QEMU. Helper scripts are included: `build.sh` and
`run.sh`.
