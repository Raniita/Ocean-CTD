import socket, time

# IP del arduino + puerto arduinos
address = ('10.0.0.127', 55055)
buffersize = 1024

# Creates UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# Dejamos a la escucha el puerto 45045
sock.bind(("", 45045))

while True:
    msg = "hola"
    sock.sendto(msg.encode(), address)

    data, ip = sock.recvfrom(1024)
    print("{}:{}".format(ip, data.decode()))