# mmo
MMORPG server over Telnet written in C23.

Requires support for UTF-8 unicode and 24-bit color.

## Connect
Use a Telnet client to connect to server. 
```
> telnet 95.217.10.143 2000
```

### Windows
If you are on Windows do not use telnet.exe. Use something like [PuTTY](https://www.putty.org/). 

## Build
```sh
git clone https://github.com/bu420/mmo
cd mmo
make
```