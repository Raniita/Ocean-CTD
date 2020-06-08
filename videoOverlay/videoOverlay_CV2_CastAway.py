#!/usr/bin/env python
import cv2, time, os, sys, subprocess
import pandas as pd
import datetime as dt
import numpy as np
from subprocess import call

logfile = os.path.abspath('./tests/CASTAWAY/csv/CC1326008_20200220_090051.csv')
videofile = './tests/M1_GX011722.MP4'
outputfile = "".join(("M1-20-02-2020", "-out", ".mp4"))

def get_creation_time(filename):
    command = ['ffprobe', '-show_format', '-pretty', '-loglevel', 'quiet', filename]
    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print(filename)
    out, err = p.communicate()
    print("==== output ====")
    print(out.decode())
    if err:
        print("==== error ====") 
        print(err.decode())
    t = out.splitlines()
    time = (t[14][18:37]).decode()
    time = time.replace("T", " ")      # XD hack
    return time

# Import the log file
try:
    print (logfile)
    #data = pd.read_csv(logfile, delimiter=',')

except Exception as err:
    print(err)
    print(data)
    sys.exit(0)