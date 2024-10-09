# mmo
MMORPG over Telnet written in C23.

## Connect
Use a Telnet client to connect to server. 
Do not use Windows' built-in `telnet.exe` as it supports neither UTF-8 nor 24-bit color.
A good client for Windows is [PuTTY](https://www.putty.org/). 
```
> telnet 95.217.10.143 2000
```

## Build
```sh
git clone https://github.com/bu420/mmo
cd mmo
make
```
