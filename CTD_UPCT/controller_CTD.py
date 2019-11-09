import numpy as np
import socket, time
import matplotlib.pyplot as plt
from drawnow import *

# IP del arduino + puerto
address = ('10.0.0.127', 55055)
buffersize = 1024

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("", 45045))

# On that measures
cdom_read = []
phy_read = []
chl_read = []
pressure = []
temp1 = []
depth = []
alt = []
temp2 = []

