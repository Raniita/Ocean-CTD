#!/usr/bin/env python
import cv2, time, os, sys, subprocess
import pandas as pd
import datetime as dt
import numpy as np
from subprocess import call

logfile = os.path.abspath('./tests/M11-20-02-2020.csv')
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
    data = pd.read_csv(logfile, delimiter=';', encoding='latin-1')
    print('Data loaded')
    time_rec = data['time']
    latitude = data['latitude']
    longitude = data['longitude']
    depth = data['depth']
    cdom = data['cdom [ppb]']
    pe = data['pe [ppb]']
    chl = data['chl [ppb]']
    
    # Creating dictionary
    lat_dic = dict(zip(time_rec, latitude))
    long_dic = dict(zip(time_rec, longitude))
    depth_dic = dict(zip(time_rec, depth))
    cdom_dic = dict(zip(time_rec, cdom))
    pe_dic = dict(zip(time_rec, pe))
    chl_dic = dict(zip(time_rec, chl))

    # Creating prev values
    lat_prev = 0
    long_prev = 0
    depth_prev = 0
    cdom_prev = 0
    pe_prev = 0
    chl_prev = 0

    # Open the video
    video = cv2.VideoCapture(videofile)
    status = video.isOpened()

    if status == True:
        FPS = video.get(cv2.CAP_PROP_FPS)
        width = video.get(cv2.CAP_PROP_FRAME_WIDTH)
        height = video.get(cv2.CAP_PROP_FRAME_HEIGHT)
        size = (int(width), int(height))
        total_frames = video.get(cv2.CAP_PROP_FRAME_COUNT)
        frame_lapse = (1/FPS)*1000

        #print(FPS)             120 FPS
        #print(size)            1920,1440
        #print(total_frames)    63720
        #print(frame_lapse)     8.341666

        # Start of time origin of the video
        t = get_creation_time(videofile)
        init = dt.datetime.strptime(t, "%Y-%m-%d %H:%M:%S")

        # Init frame counter
        current_frame = 0
        start = time.time()

        # Old way using CV2 to export
        codec = cv2.VideoWriter.fourcc('D', 'I', 'V', 'X')
        video_out = cv2.VideoWriter(videofile[:-4] + '.avi', codec, FPS, size, 1)

        print('Processing...')
        
        # Read each fram, calculate timestamp, place info and export
        while current_frame < total_frames:
            success, image = video.read()
            elapsed_time = video.get(cv2.CAP_PROP_POS_MSEC)
            current_frame = video.get(cv2.CAP_PROP_POS_FRAMES)
            timestamp = init + dt.timedelta(microseconds= elapsed_time*1000)
            t = timestamp + dt.timedelta(microseconds= -timestamp.microsecond)
            t = t + dt.timedelta(minutes=2)
            t = t.strftime("%H:%M:%S")
            # Set values and reading prev values
            lat_value = lat_dic.get(t,  lat_prev)
            lat_prev = lat_value
            long_value = long_dic.get(t, long_prev)
            long_prev = long_value
            depth_value = depth_dic.get(t, depth_prev)
            depth_prev = depth_value
            cdom_value = cdom_dic.get(t, cdom_prev)
            cdom_prev = cdom_value
            pe_value = pe_dic.get(t, pe_prev)
            pe_prev = pe_value
            chl_value = chl_dic.get(t, chl_prev)
            chl_prev = chl_value

            #overlay = np.zeros_like(image, "uint32")
            #imageo = np.array(np.clip(image+overlay, 0, 255), "uint8")
            # Overlay values
            cv2.putText(image, 'Date: ' + str(timestamp)[0:10], (50, int(height-150)), cv2.FONT_HERSHEY_COMPLEX_SMALL, 2, (255, 255, 255), 3)
            cv2.putText(image, 'Time: ' + str(timestamp)[11:-4], (50, int(height-100)), cv2.FONT_HERSHEY_COMPLEX_SMALL, 2, (255, 255, 255), 3)
            cv2.putText(image, 'Depth: ' + str(depth_value), (1450, int(height-250)), cv2.FONT_HERSHEY_COMPLEX_SMALL, 2, (255, 255, 255), 3)
            cv2.putText(image, 'CDOM [ppb]: ' + str(cdom_value), (1450, int(height-200)), cv2.FONT_HERSHEY_COMPLEX_SMALL, 2, (255, 255, 255), 3)
            cv2.putText(image, 'PE [ppb]: ' + str(pe_value), (1450, int(height-150)), cv2.FONT_HERSHEY_COMPLEX_SMALL, 2, (255, 255, 255), 3)
            cv2.putText(image, 'CHL [ppb]: ' + str(chl_value), (1450, int(height-100)), cv2.FONT_HERSHEY_COMPLEX_SMALL, 2, (255, 255, 255), 3)
            
            if success:
                print(t)
                video_out.write(image)
            #else:
                #print(current_frame)
                #print(total_frames)
        
        video.release()
        video_out.release()
        cv2.destroyAllWindows()

        # Calculate how long the timestampping took
        duration = (time.time() - float(start))/60

        print("Video has been writed")
        print("This job took: " + str(duration) + "minutes")
    else:
        print('Error: could not load video')
        sys.exit(0)
except Exception as err:
    print(err)
    sys.exit(0)
