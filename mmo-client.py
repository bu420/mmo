import socket

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

address = ('mmo.severinsen.se', 2000)

print(f'Connecting to {address[0]}:{address[1]}...')

try:
    sock.connect(address)
except ConnectionRefusedError:
    print('Failed to connect to server.')

