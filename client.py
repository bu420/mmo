import sys
import socket
import os
import fcntl
import errno

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Set socket to non-blocking.
fcntl.fcntl(sock, fcntl.F_SETFL, os.O_NONBLOCK)

try:
    sock.connect(('mmo.severinsen.se', 2000))
except ConnectionRefusedError:
    print('Failed to connect.')
    exit(-1)

while True:
    try:
        data = sock.recv(4096)
    except socket.error as e:
        err = e.args[0]

        if err != errno.EAGAIN and err != errno.EWOULDBLOCK:
            print(e)
            sys.exit(-1)
    else:
        print(data)