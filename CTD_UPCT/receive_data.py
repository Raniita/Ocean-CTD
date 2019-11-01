import socket

# Aqui iria la ip del arduino
udp_ip = "127.0.0.1"
udp_port = 55055

buffersize = 1024

# Creates UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind to the socket
sock.bind((udp_ip, udp_port))

while True:
    data, addr = sock.recvfrom(buffersize)
    print("received message: ", data.decode('utf-8'))