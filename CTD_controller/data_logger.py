from datetime import date
import socket, time, csv

# Arduino IP + port
arduino = ('10.0.0.20', 55055)
buffersize = 1024

station = input("Please introduce name of current station: ")

filename = "-".join((date.today(),station,".csv"))
log_filename = "-".join((date.today(),station,".log"))

# UPD Socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", 45045))

sock.settimeout(1)  # 1 sec timeout

while True:
    # cdom -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
    # phy -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
    # chl -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
    # ms5 -> Respuesta: <type>;<pressure>;<temp>;<depth>;<altitude>
    # temp -> Respuesta: <type>;<measure> (i2c sensors)

    msg2send = "ms5"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.split(";")
        pressure = data[1]
        temp1 = data[2]
        depth = data[3]
        altitude = data[4]

        print("Depth: %f Pressure: %f Altitude: %f Temp: %f", depth, pressure, altitude, temp1)

    except:
        print("Error on %s", "ms5")
        pass

    time.sleep(1)   # little delay until the next command

    msg2send = "cdom"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.split(";")
        cdom_gain = data[1]
        cdom_ppb = data[2]
        cdom_mv = data[3]
    
    except:
        pass
    
    time.sleep(1)

    msg2send = "phy"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.split(";")
        phy_gain = data[1]
        phy_ppb = data[2]
        phy_mv = data[3]

    except:
        pass

    time.sleep(1)

    msg2send = "chl"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.split(";")
        chl_gain = data[1]
        chl_ppb = data[2]
        chl_mv = data[3]

    except:
        pass

    time.sleep(1)

    msg2send = "temp"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.split(";")
        temp2 = data[1]