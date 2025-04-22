# Design
The network code and game code is separated but connected via a simple event system that lets the game know a client joined, left, sent keyboard input or resized their terminal.

The server is strictly single threaded with non-blocking sockets.

# Code Principles
Both `server/inc/mmo/arr/template.h` and `server/inc/mmo/list/template.h` is designed so that they essentially store references to their elements which means the owner of the containers must allocate memory for each element before appending it to the container aswell as explicitly freeing each element before calling the containers "free()" function.

# Network Protocol
The server and client uses a custom protocol to transmit text, keyboard input and terminal dimensions.

## Server to Client
All data sent from server to client is printed directly to terminal.

## Client to Server

### Handshake Packet

| Size                  | Description              |
| --------------------- | ------------------------ |
| 32-bit signed integer | Packet ID: **0**         |
| 32-bit signed integer | Protocol Version: **1**  |
| 32-bit signed integer | Terminal width in cells  |
| 32-bit signed integer | Terminal height in cells |

### Keyboard Input Packet

| Size                  | Description      |
| --------------------- | ---------------- |
| 32-bit signed integer | Packet ID: **1** |
| 32-bit signed integer | Number of bytes  |
| N bytes               | Keyboard input   |

### Terminal Dimensions Packet
Sent whenever client terminal dimensions change.

| Size                  | Description              |
| --------------------- | ------------------------ |
| 32-bit signed integer | Packet ID: **2**         |
| 32-bit signed integer | Terminal width in cells  |
| 32-bit signed integer | Terminal height in cells |
