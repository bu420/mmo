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
