# -*- coding: utf-8 -*-
import subprocess # call
import os
import pandas as pd # read_table
import matplotlib.pyplot as plt # show
import numpy as np

# disclaimer
print("DISLAIMER: Ejecutame desde la carpeta experimentos por favor... gracias!")

# Experiment values
warm_up = 25# Para que el proce ya tenga todo lo necesario en cache WARM UP
trimming = 30 # Para quedarse con la media de todas la medidas obtenidas
# warm_up = 5
# trimming = 1 
times = [0,0,0,0]
# sys call 
input_maker = ['python', './tools/make_input.py' \
    , '-s', '42'\
    , '../data/ImagenesCaras/','1', '-ti','[1]'] # Concatenarle aca el numero de imagenes por sujeto
tp = ['./tp', 'data.in', 'data.out' \
    ,'-p']

# PWD es ahora src/
# para que guarde el data.in ahi, y poder llamar a ./tp mas facil
os.chdir("../")

subprocess.call(input_maker)
print "Calenteando..."
for w in range(0, warm_up):
    subprocess.call(tp , stdout=open(os.devnull, 'wb'))

print("Empezando experimento...")
for k in range(0, trimming):
    print k, " ..."
    subprocess.call(tp ,stdout=open(os.devnull, 'wb'))
    temp = np.loadtxt("PROFILE.dat")
    for i in range(0,4):
        times[i] = times[i] + temp[i][1]

# Cleaning
subprocess.call(["rm", "PROFILE.dat"])

for i in range(0,4):
    times[i] = times[i]/trimming
print times

# Plotting

from pylab import *

# make a square figure and axes
import matplotlib.pyplot as plt
sizes = [x/max(times)*100 for x in times]

# Pie chart, where the slices will be ordered and plotted counter-clockwise:
labels = []
for i in range(0,4):
    labels.append("Etapa " + str(i+1)+": "+ "{:.6f}".format(times[i]) + " seg.")

explode = (0.6, 0, 0.2, 0.4)  # only "explode" the 2nd slice (i.e. 'Hogs')

plt.pie(sizes, explode=explode, autopct='%1.1f%%',
         startangle=180, pctdistance=1.15)
plt.legend(labels, loc="best")
plt.axis('equal')  # Equal aspect ratio ensures that pie is drawn as a circle.

plt.savefig('plots/time_profiling.pdf',format='pdf')
plt.show()
