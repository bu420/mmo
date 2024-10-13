# mmo
MMORPG server over Telnet written in C23.

## Connect
Use a Telnet client to connect to server. 
```
> telnet 95.217.10.143 2000
```

### Windows telnet.exe
To use telnet.exe you must first switch from ANSI mode to VT100 mode.
1. Open a terminal and enter `telnet`.
2. Enter 'd' to view settings. There will be a line saying `Preferred Term Type is ANSI`.
3. Enter `set term vt100`.
4. Enter 'q' to quit.

Alternatively use [PuTTY](https://www.putty.org/). 

## Build
```sh
git clone https://github.com/bu420/mmo
cd mmo
make
```
