#!/usr/bin/env python
from datetime import datetime
import socket, time, csv

# Arduino IP + port
arduino = ('10.0.1.10', 55055)
buffersize = 1024

#station = input("Please introduce name of current station: ")
station = 'e1'
date = datetime.today().strftime("%Y-%m-%d")
start_timestamp = datetime.today().strftime("%Y-%m-%d-%H-%M-%S")

filename = "".join((start_timestamp,station,".csv"))
log_filename = "".join((start_timestamp,station,"_log.csv"))

csvfile = open(filename, 'wt')
logfile = open(log_filename, 'wt')

csv_writer = csv.writer(csvfile, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
csv_writer.writerow(["station", "date", "depth", "var1", "var2", "var3"])

# UPD Socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", 45045))

sock.settimeout(2)  # 1 sec timeout

print("Receiving data: ... \n")

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
        data = recv_d.decode().split(";")
        pressure = data[1]
        temp1 = data[2]
        depth = data[3]
        altitude = data[4]

        print(f"[MS5] Depth: {depth} Pressure: {pressure} Altitude: {altitude} Temp: {temp1}")

        # Save log files

        # Save OK
        ms5_read = True

    except:
        ms5_read = False
        print("Error on ms5")
        pass

    time.sleep(2)   # little delay until the next command

    msg2send = "cdom"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        cdom_gain = data[1]
        cdom_ppb = data[2]
        cdom_mv = data[3]

        print(f"[CDOM] Gain: {cdom_gain} PPB: {cdom_ppb} mV: {cdom_mv}")

        # Save to log files

        # Save OK
        cdom_read = True

    except:
        cdom_read = False
        print("Error reading cdom")
        pass
    
    time.sleep(2)

    msg2send = "phy"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        phy_gain = data[1]
        phy_ppb = data[2]
        phy_mv = data[3]

        print(f"[PHY] Gain: {phy_gain} PPB: {phy_ppb} mV: {phy_mv}")

        # Save to log files

        # Save OK
        phy_read = True

    except:
        phy_read = False
        print("Error reading phy")
        pass

    time.sleep(2)

    msg2send = "chl"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        chl_gain = data[1]
        chl_ppb = data[2]
        chl_mv = data[3]

        print(f"[CHL] Gain: {chl_gain} PPB: {chl_ppb} mV: {chl_mv}")

        # Save to log files

        # Save OK
        chl_read = True

    except:
        chl_read = False
        print("Error reading chl")
        pass

    time.sleep(2)

    msg2send = "temp"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        temp2 = data[1]

        print(f"[Temp] temp2: {temp2}")

        # Save to log files

        # Save OK
        temp_read = True

    except:
        temp_read = False
        print("Error reading temp")
        pass

    if(ms5_read == True and cdom_read == True and phy_read == True and chl_read == True and temp_read == True):
        # Write to CSV
        csv_writer.writerow([station, date, depth, cdom_ppb, phy_ppb, chl_ppb])
    else:
        print("Unable to save on csv.")