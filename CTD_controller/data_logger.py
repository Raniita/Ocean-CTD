#!/usr/bin/env python
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from multiprocessing import Process
from datetime import datetime
import os, socket, time, csv
import gpsd

import salinity

# Plot
def live_plot(filename, station):
    """ Threaded func to get the CSV data and plotting it """
    
    # Ventana de datos
    win = 100     

    def animate(i):
        """ Animate Function """
        try:
            # Read the csv
            data = pd.read_csv(filename, delimiter=',')
            y_values = data['depth']
            x1_values = data['cdom [ppb]']
            last_cdom = str(x1_values.iloc[-1])
            x2_values = data['pe [ppb]']
            last_phy = str(x2_values.iloc[-1])
            x3_values = data['chl [ppb]']
            last_chl = str(x3_values.iloc[-1])
            x4_values = data['ce [uS/cm]']
            last_ce = str(x4_values.iloc[-1])
            x5_values = data['s [pps]']
            last_s = str(x5_values.iloc[-1])

            # Fix first row csv, empty values
            if(last_cdom == "cdom [ppb]"):
                last_cdom = "none"
                last_phy = "none"
                last_chl = "none"
                last_ce = "none"
                last_s = "none"

            # Limit the data
            imin = min(max(0, i - win), y_values.size - win)
            
            # Plotting all the data of the csv, show the last on legend
            plt.cla()
            plt.plot(x1_values[imin:i], y_values[imin:i], color="orange", label="cdom [ppb]")
            plt.plot([],[], ' ', label="CDOM: "+last_cdom)
            plt.plot(x2_values[imin:i], y_values[imin:i], color="blue", label="pe [ppb]")
            plt.plot([],[], ' ', label="PE: "+last_phy)
            plt.plot(x3_values[imin:i], y_values[imin:i], color="green", label="chl [ppb]")
            plt.plot([],[], ' ', label="CHL: "+last_chl)
            plt.scatter(0,0, color="red", label="ce [uS/cm]")
            plt.plot([],[], ' ', label="CE: "+last_ce)
            plt.scatter(0,0, color="yellow", label="s [pps]")
            plt.plot([],[], ' ', label="S: "+last_s)
            plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left', borderaxespad=0.)
        except pd.errors.EmptyDataError:
            print (filename, " is empty")
        except IndexError:
            pass
        
        plt.xlabel('PPB')
        plt.ylabel('Depth (m)')
        plt.title("".join(("CDOM-PE-CHL on ", station)))
        plt.gcf().autofmt_xdate()
        plt.gca().invert_yaxis()
        plt.tight_layout()
        
    ani = FuncAnimation(plt.gcf(), animate, 3000)
    #ani.save(videofilename, writer='ffmpeg')

    plt.tight_layout()
    plt.show()
    
if __name__ == '__main__':
    # Arduino IP + port [10.0.1.10 DHCP del barco]
    arduino = ('10.0.1.10', 55055)
    buffersize = 1024

    # UPD Socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("", 45045))
    sock.settimeout(5)

    # Verify hardware version
    msg2send = "ping"
    sock.sendto(msg2send.encode(), arduino)
    try:
        recv_d, addr = sock.recvfrom(buffersize)
        data = recv_d.decode().split(";")
        pong = data[0]
        version = data[1]
        print(f"Hardware version: {version}")
    except:
        print("Unable to connect with probe")
        raise Exception

    station = input("Please introduce name of current station: ")
    start_timestamp = datetime.today().strftime("%d-%m-%Y")

    ## TODO:
    # Create day folder 
    #if not os.path.exists(start_timestamp):
    #    os.makedirs(start_timestamp)

    filename = "".join((station, "-", start_timestamp,".csv"))
    #videofilename = "".join((station, "-", start_timestamp,".mp4"))

    # Ask to use GPSd info?
    use_gpsd = input("Use GPS? [y/N]: ")
    if use_gpsd == "y":
        # GPSd en el router amarillo
        print("Using GPS on 10.0.1.1")
        gpsd.connect(host="10.0.1.1", port=2947)

        with open(filename, 'a', newline='') as f:
            if version == "v3":
                row = ["station","latitude", "longitude", "time", "depth", "temp1", "cdom [gain]", "cdom [ppb]", "cdom [mv]", "pe [gain]", "pe [ppb]", "pe [mv]", "chl [gain]", "chl [ppb]", "chl [mv]", "ce [uS/cm]", "s [pps]"]
            else:
                row = ["station","latitude", "longitude", "time", "depth", "temp1", "temp2", "cdom [gain]", "cdom [ppb]", "cdom [mv]", "pe [gain]", "pe [ppb]", "pe [mv]", "chl [gain]", "chl [ppb]", "chl [mv]"]
            writer = csv.writer(f, delimiter=',')
            writer.writerow(row)

        position = gpsd.get_current().position()
    else:
        with open(filename, 'a', newline='') as f:
            if version == "v3":
                row = ["station", "time", "depth", "temp1", "cdom [gain]", "cdom [ppb]", "cdom [mv]", "pe [gain]", "pe [ppb]", "pe [mv]", "chl [gain]", "chl [ppb]", "chl [mv]", "ce [uS/cm]", "s [pps]"]
            else:
                row = ["station", "time", "depth", "temp1", "temp2", "cdom [gain]", "cdom [ppb]", "cdom [mv]", "pe [gain]", "pe [ppb]", "pe [mv]", "chl [gain]", "chl [ppb]", "chl [mv]"]
            writer = csv.writer(f, delimiter=',')
            writer.writerow(row)

    
    print("Receiving data: ... \n")

    # Definition plot thread
    p = Process(target=live_plot, args=(filename, station,))
    p.start()

    while True:
        # cdom -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
        # phy -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
        # chl -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
        # ms5 -> Respuesta: <type>;<pressure>;<temp>;<depth>;<altitude>
        # temp -> Respuesta: <type>;<measure> (i2c sensors)
        # (v3) ce -> Respuesta: <type>;<addr>;<value> (sdi-12 sensor)

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

                print(f"[MS5] Depth:{depth} Pressure:{pressure} Altitude:{altitude} Temp:{temp1}")

                # Save OK
                ms5_read = True
        except:
            ms5_read = False
            print("Error. No response for depth.")
            pass

        msg2send = "cdom"
        sock.sendto(msg2send.encode(), arduino)
        try:
            recv_d, addr = sock.recvfrom(buffersize)
            data = recv_d.decode().split(";")
            cdom_gain = data[1]
            cdom_ppb = data[2]
            cdom_mv = data[3]

            print(f"[CDOM] Gain:x{cdom_gain} PPB:{cdom_ppb} mV:{cdom_mv}")

            # Save OK
            cdom_read = True
        except:
            cdom_read = False
            print("Error. No response for cdom.")
            pass
    
        msg2send = "phy"
        sock.sendto(msg2send.encode(), arduino)
        try:
            recv_d, addr = sock.recvfrom(buffersize)
            data = recv_d.decode().split(";")
            phy_gain = data[1]
            phy_ppb = data[2]
            phy_mv = data[3]

            print(f"[PE] Gain:x{phy_gain} PPB:{phy_ppb} mV:{phy_mv}")

            # Save OK
            phy_read = True
        except:
            phy_read = False
            print("Error. No response for pe.")
            pass

        msg2send = "chl"
        sock.sendto(msg2send.encode(), arduino)
        try:
            recv_d, addr = sock.recvfrom(buffersize)
            data = recv_d.decode().split(";")
            chl_gain = data[1]
            chl_ppb = data[2]
            chl_mv = data[3]

            print(f"[CHL] Gain:x{chl_gain} PPB:{chl_ppb} mV:{chl_mv}")

            # Save OK
            chl_read = True
        except:
            chl_read = False
            print("Error. No response for chl.")
            pass

        if not version == "v3":
            ce_read = True
            msg2send = "temp"
            sock.sendto(msg2send.encode(), arduino)
            try:
                recv_d, addr = sock.recvfrom(buffersize)
                data = recv_d.decode().split(";")
                temp2 = data[1]

                print(f"[Temp] temp2:{temp2}")

                # Save OK
                temp_read = True
                
            except:
                temp_read = False
                print("Error. No response for temp2.")
                pass
        else:
            temp_read = True
            msg2send = "ce"
            sock.sendto(msg2send.encode(), arduino)
            try:
                recv_d, addr = sock.recvfrom(buffersize)
                data = recv_d.decode().split(";")
                addr = data[1]
                ce = data[2]

                print(f"[CE] Sensor address:{addr}, ce: {ce}")

                # Save OK
                ce_read = True
            except:
                ce_read = False
                print("Error. No response for ce.")
                pass

        if(ms5_read and cdom_read and phy_read and chl_read and temp_read and ce_read):
            # Write to CSV
            time = datetime.today().strftime("%H:%M:%S")
            # GPSd info?
            if use_gpsd == "y":
                #position = gpsd.get_current().position()
                print(f"[GPS] Lattitude:{position[0]} Longitude:{position[1]}")
                if version == "v3":
                    s = round(salinity.get_salinity(temperature=temp1,depth=depth, ce=ce), 4)
                    row =  [station, position[0], position[1], time, depth, temp1, cdom_gain, cdom_ppb, cdom_mv, phy_gain, phy_ppb, phy_mv, chl_gain, chl_ppb, chl_mv, ce, s]
                else:
                    row =  [station, position[0], position[1], time, depth, temp1, temp2, cdom_gain, cdom_ppb, cdom_mv, phy_gain, phy_ppb, phy_mv, chl_gain, chl_ppb, chl_mv]
            else:
                if version == "v3":
                    s = round(salinity.get_salinity(temperature=temp1,depth=depth, ce=ce), 4)
                    row =  [station, time, depth, temp1, cdom_gain, cdom_ppb, cdom_mv, phy_gain, phy_ppb, phy_mv, chl_gain, chl_ppb, chl_mv, ce, s]
                else:
                    row =  [station, time, depth, temp1, temp2, cdom_gain, cdom_ppb, cdom_mv, phy_gain, phy_ppb, phy_mv, chl_gain, chl_ppb, chl_mv]

            with open(filename, 'a', newline='') as f:
                writer = csv.writer(f, delimiter=',')
                writer.writerow(row)
        else:
            print("Error. Unable to save CSV.\nSome sensors have not been read.")

    p.join()