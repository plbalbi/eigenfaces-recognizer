# -*- coding: utf-8 -*-
import subprocess # call
import os
import pandas as pd # read_table
import matplotlib.pyplot as plt # show
import numpy as np

# disclaimer
print("DISLAIMER: Ejecutame desde la carpeta experimentos por favor... gracias!")

# Experiment values
initial_value = 1
step = 1
max_value = 10
warm_up = 25# Para que el proce ya tenga todo lo necesario en cache WARM UP
trimming = 20 # Para quedarse con la media de todas la medidas obtenidas
# File parsing values
time_file = "./tiempo.dat"
x_axis = []
time_axis = []
# sys call 
input_maker = ['python', './tools/make_input.py' \
    , '-s', '42', '-train'\
    , '../data/ImagenesCaras/','1','-i'] # Concatenarle aca el numero de imagenes por sujeto
tp = ['./tp', 'data.in', 'data.out' \
    ,'-jt',time_file]

# PWD es ahora src/
# para que guarde el data.in ahi, y poder llamar a ./tp mas facil
os.chdir("../")

print "Calenteando..."
for w in range(0, warm_up):
    subprocess.call(input_maker + [str(initial_value)])
    subprocess.call(tp , stdout=open(os.devnull, 'wb'))

print("Empezando experimento...")
for imgs in range(initial_value, max_value+step, step):
    cant_imgs = 41*imgs
    print "Corriendo para imgs = ", cant_imgs
    temp_time = []
    x_axis.append(cant_imgs)
    subprocess.call(input_maker + [str(imgs)])
    for t in range(0,trimming):
        subprocess.call(tp ,stdout=open(os.devnull, 'wb'))
        t = open(time_file, "r")
        temp_time.append(float(t.read()))
        t.close()
    media_time = np.mean(temp_time)
    print "Tiempo medio: ", media_time
    time_axis.append(float(media_time))

# Cleaning
subprocess.call(["rm", time_file])

# Generando grafico
plt.plot(x_axis, time_axis, 'r-')
plt.ylabel('Segundos')
plt.xlabel('Cantidad de imágenes procesadas')
plt.savefig('plots/entrenamiento_n_time.pdf',format='pdf')
plt.clf()

# Guardando datos en un .data por si necestio los raw

with open("temp_results.data", "w") as f:
    f.write("K" + "   " + "tiempo" + "\n");
    for i in range(0, len(x_axis)):
        f.write(str(x_axis[i]) + "   " + str(time_axis[i]) + "\n");
    f.close()

