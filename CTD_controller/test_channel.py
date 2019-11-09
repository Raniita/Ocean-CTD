import socket

# Aqui ip del arduino
udp_ip = "127.0.0.1"
udp_port = 55055

message = "cdom;10;34;3000"

print("IP: ", udp_ip)
print("Port: ", udp_port)
print("message: ", message)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    sock.sendto(message.encode('utf-8'), (udp_ip, udp_port))