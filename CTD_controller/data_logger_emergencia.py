from datetime import datetime
import socket
import time
import csv

# Esta version esta pensada para cuando el sensor de profundidad falla
# Es necesario introducir la profundidad de forma manual
# Usar solo cuando sea necesario.

if __name__ == "__main__":
    # Arduino IP + port [10.0.1.10 DHCP del barco]
    arduino = ('10.0.1.10', 55055)
    buffersize = 1024

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

    station = input("Introduce la estacion actual: ")
    start_timestamp = datetime.today().strftime("%d-%m-%Y")

    filename = "".join((station, "-", start_timestamp, ".csv"))

    # Creating the csv
    with open(filename, 'a', newline='') as f:
        if version == "v3":
            row = ["station","latitude", "longitude", "time", "depth", "temp1", "cdom [gain]", "cdom [ppb]", "cdom [mv]", "pe [gain]", "pe [ppb]", "pe [mv]", "chl [gain]", "chl [ppb]", "chl [mv]", "ce [uS/cm]"]
        else:
            row = ["station","latitude", "longitude", "time", "depth", "temp1", "temp2", "cdom [gain]", "cdom [ppb]", "cdom [mv]", "pe [gain]", "pe [ppb]", "pe [mv]", "chl [gain]", "chl [ppb]", "chl [mv]"]
        writer = csv.writer(f, delimiter=',')
        writer.writerow(row)

    
    print("Listo para recibir datos: ... \n")

    while True:
        depth = input("Introduce profundidad actual: ")
        temp1 = 0
        pressure = 0
        altitude = 0

        # Preguntamos por todos los valores

        # cdom -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
        # phy -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
        # chl -> Respuesta: <type>;<gain>;<measure>;<mv> (cyclops)
        # ms5 -> Respuesta: <type>;<pressure>;<temp>;<depth>;<altitude>
        # temp -> Respuesta: <type>;<measure> (i2c sensors)
        # (v3) ce -> Respuesta: <type>;<addr>;<value> (sdi-12 sensor)

        #msg2send = "ms5"
        #sock.sendto(msg2send.encode(), arduino)
        #try:
        #    recv_d, addr = sock.recvfrom(buffersize)
        #    data = recv_d.decode().split(";")
        #    if(data[1] == "error"):
        #        pressure = data[1]
        #        temp1 = data[1]
        #        depth = data[1]
        #        altitude = data[1]
        #    else:
        #        pressure = data[1]
        #        temp1 = data[2]
        #        depth = data[3]
        #        altitude = data[4]

        #        print(
        #            f"[MS5] Depth:{depth} Pressure:{pressure} Altitude:{altitude} Temp:{temp1}")

                # Save OK
        #        ms5_read = True
        #except:
        #    ms5_read = False
        #    print("Error. No response for depth.")
        #    pass

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
            print("Error. No hay respuesta de CDOM.")
            #pass

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
            print("Error. No hay respuesta de PE.")
            #pass

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
            print("Error. No hay respuesta de CHL.")
            #pass

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
            print("Error. No hay respuesta de Temp2.")
            #pass

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

        # Terminamos, guardamos en el CSV
        if (cdom_read and phy_read and chl_read and temp_read and ce_read):
            time = datetime.today().strftime("%H:%M:%S")

            # Preparing the row
            if version == "v3":
                row =  [station, time, depth, temp1, cdom_gain, cdom_ppb, cdom_mv, phy_gain, phy_ppb, phy_mv, chl_gain, chl_ppb, chl_mv, ce]
            else:
                row =  [station, time, depth, temp1, temp2, cdom_gain, cdom_ppb, cdom_mv, phy_gain, phy_ppb, phy_mv, chl_gain, chl_ppb, chl_mv]
            
            # Write to CSV
            with open(filename, 'a', newline='') as f:
                writer = csv.writer(f, delimiter=',')
                writer.writerow(row)

        else:
            print("Error. No se ha podido guardar en el CSV.\nAlgunos sensores no se han leido correctamente.")

        # Listo para continuar, ya puedes bajar
        print("Datos guardados, listo para cambiar de profundidad.\n")