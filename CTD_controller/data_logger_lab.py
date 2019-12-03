#!/usr/bin/env python
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from multiprocessing import Process
from datetime import datetime
import socket, time, csv

# Arduino IP + port
arduino = ('10.0.1.10', 55055)
buffersize = 1024

# CSV
station = input("Please introduce name of current station: ")
start_timestamp = datetime.today().strftime("%d-%m-%Y")

filename = "".join((station, "-", start_timestamp,".csv"))
#videofilename = "".join((station, "-", start_timestamp,".mp4"))

with open(filename, 'a') as f:
    row = ["station", "time", "depth", "temp1", "temp2", "cdom_gain", "cdom_ppb", "cdom_mv", "pe_gain", "pe_ppb", "pe_mv", "chl_gain", "chl_ppb", "chl_mv"]
    writer = csv.writer(f)
    writer.writerow(row)

# UPD Socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", 45045))
sock.settimeout(5)  
print("Receiving data: ... \n")

# Plot
def live_plot():
    """ Threaded func to get the CSV data and plotting it """
    
    # Ventana de datos
    win = 100     

    def animate(i):
        """ Animate Function """
        try:
            data = pd.read_csv(filename)
            x_values = data['time']
            y1_values = data['cdom_ppb']
            y2_values = data['pe_ppb']
            y3_values = data['chl_ppb']
            imin = min(max(0, i - win), x_values.size - win)    # Ajustes de la ventana
            plt.cla()
            plt.plot(x_values[imin:i], y1_values[imin:i], label="cdom_ppb")
            plt.plot(x_values[imin:i], y2_values[imin:i], label="pe_ppb")
            plt.plot(x_values[imin:i], y3_values[imin:i], label="chl_ppb")
            plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)
        except pd.io.common.EmptyDataError:
            print (filename, " is empty")    
        
        plt.xlabel('Time')
        plt.ylabel('PPB')
        plt.title("".join(("CDOM-PE-CHL on ", station)))
        plt.gcf().autofmt_xdate()
        plt.tight_layout()
        
    ani = FuncAnimation(plt.gcf(), animate, 1000)
    #ani.save(videofilename, writer='ffmpeg')

    plt.tight_layout()
    plt.show()
    
# Definition plot thread
p = Process(target=live_plot)
p.start()

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
        if(data[1] == "error"):
            pressure = data[1]
            temp1 = data[1]
            depth = data[1]
            altitude = data[1]
        else:
            pressure = data[1]
            temp1 = data[2]
            depth = data[3]
            altitude = data[4]

            print(f"[MS5] Depth: {depth} Pressure: {pressure} Altitude: {altitude} Temp: {temp1}")

            # Save OK
            ms5_read = True
    except:
        ms5_read = False
        print("Error on ms5")
        pass

    #time.sleep(1)

    msg2send = "cdom"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        cdom_gain = data[1]
        cdom_ppb = data[2]
        cdom_mv = data[3]

        print(f"[CDOM] Gain: {cdom_gain} PPB: {cdom_ppb} mV: {cdom_mv}")

        # Save OK
        cdom_read = True

    except:
        cdom_read = False
        print("Error reading cdom")
        pass
    
    #time.sleep(1)

    msg2send = "phy"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        phy_gain = data[1]
        phy_ppb = data[2]
        phy_mv = data[3]

        print(f"[PHY] Gain: {phy_gain} PPB: {phy_ppb} mV: {phy_mv}")

        # Save OK
        phy_read = True

    except:
        phy_read = False
        print("Error reading phy")
        pass

    #time.sleep(1)

    msg2send = "chl"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        chl_gain = data[1]
        chl_ppb = data[2]
        chl_mv = data[3]

        print(f"[CHL] Gain: {chl_gain} PPB: {chl_ppb} mV: {chl_mv}")

        # Save OK
        chl_read = True

    except:
        chl_read = False
        print("Error reading chl")
        pass

    #time.sleep(1)

    msg2send = "temp"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        temp2 = data[1]

        print(f"[Temp] temp2: {temp2}")

        # Save OK
        temp_read = True

    except:
        temp_read = False
        print("Error reading temp")
        pass

    if(ms5_read == True and cdom_read == True and phy_read == True and chl_read == True and temp_read == True):
        # Write to CSV
        time = datetime.today().strftime("%H:%M:%S")
        row = [station, time, depth, temp1, temp2, cdom_gain, cdom_ppb, cdom_mv, phy_gain, phy_ppb, phy_mv, chl_gain, chl_ppb, chl_mv]

        with open(filename, 'a') as f:
            writer = csv.writer(f)
            writer.writerow(row)
    else:
        print("Unable to save on csv.")
        
p.join()