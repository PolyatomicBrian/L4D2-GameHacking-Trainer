# L4D2-Trainer

#### Brian Jopling, 2022

A simple trainer for L4D2 written in C++ that modifies assembly instructions 
loaded into memory to provide cheats like infinite ammo and infinite health.
This trainer is expected to be used in Single Player mode.

Static offsets from base address were discovered via reverse engineering 
using tools like ReClass, Cheat Engine, & IDA Pro.

The Windows API is used to perform external memory manipulation. As such,
this source code **must be compiled and run on a Windows machine.**

Written & tested in Visual Studio w/ C++14 on Windows 10.

## Note

To avoid getting VAC banned, run L4D2 with the `-insecure` parameter.
You accept sole responsibility by using this trainer.
