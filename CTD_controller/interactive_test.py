#!/usr/bin/env python
import socket, time

# IP del arduino + puerto arduinos
# Con el mini router => 10.0.1.10 o probe.lan
address = ('10.0.1.10', 55055)
buffersize = 1024

# Creates UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Dejamos a la escucha el puerto 45045
sock.bind(("", 45045))

sock.settimeout(2)  # 2 sec timeout

while True:
    msg = input("Command to send: ")
    sock.sendto(msg.encode(), address)
    try:
        data, ip = sock.recvfrom(1024)
        print("{}: {}".format(ip, data.decode()))

    except:
        print("No response")
        pass
    