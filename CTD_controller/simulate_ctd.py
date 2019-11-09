import socket

server_port = 55055

# Setup UDP Channel on 55055
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", server_port))

print("Simulate CTD on port:", server_port)
print("Receveing...")

while True:
    data, addr = sock.recvfrom(1024)
    decode_msg = data.decode()

    if(decode_msg == "cdom"):
        message = "cdom;10;34;3000"
        sock.sendto(message.encode(), addr)
    if(decode_msg == "phy"):
        message = "phy;100;24;2400"
        sock.sendto(message.encode(), addr)

    if(decode_msg == "chl"):
        message = "chl;1;10;1500"
        sock.sendto(message.encode(), addr)

    if(decode_msg == "ms5"):
        message = "ms5;1;20;2.5;100"
        sock.sendto(message.encode(), addr)

    if(decode_msg == "temp"):
        message = "temp;25"
        sock.sendto(message.encode(), addr)

    del data
    del decode_msg
    del addr