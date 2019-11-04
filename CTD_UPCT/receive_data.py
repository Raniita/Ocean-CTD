import socket

# Aqui iria la ip del arduino
udp_ip = "10.0.0.247"
udp_port = 45045

buffersize = 1024

# Creates UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind to the socket
sock.bind((udp_ip, udp_port))

while True:
    data, addr = sock.recvfrom(buffersize)
    print("received message: ", data.decode('utf-8'))