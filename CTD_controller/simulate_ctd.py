import socket,time
import random as rd

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
        message = "cdom;10;"+ str(rd.randrange(10, 25)) +";3000"
        time.sleep(0.5)
        sock.sendto(message.encode(), addr)
        
    if(decode_msg == "phy"):
        message = "phy;100;"+ str(rd.randrange(5, 10)) +";2400"
        time.sleep(0.5)
        sock.sendto(message.encode(), addr)

    if(decode_msg == "chl"):
        message = "chl;1;"+ str(rd.randrange(2, 8)) +";1500"
        time.sleep(0.5)
        sock.sendto(message.encode(), addr)

    if(decode_msg == "ms5"):
        message = "ms5;1;"+ str(rd.randrange(0, 30)) +";"+ str(rd.randrange(0,10))+";100"
        time.sleep(0.5)
        sock.sendto(message.encode(), addr)

    if(decode_msg == "temp"):
        message = "temp;" + str(rd.randrange(0,30))
        time.sleep(0.5)
        sock.sendto(message.encode(), addr)

    del data
    del decode_msg
    del addr