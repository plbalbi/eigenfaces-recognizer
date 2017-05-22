# -*- coding: utf-8 -*-
import subprocess # call
import os
import pandas as pd # read_table
import matplotlib.pyplot as plt # show
import numpy as np

# disclaimer
print("DISLAIMER: Ejecutame desde la carpeta experimentos por favor... gracias!")

# Experiment values
initial_value = 10
step = 10
max_value = 300
warm_up = 50 # Para que el proce ya tenga todo lo necesario en cache WARM UP
trimming = 20 # Para quedarse con la media de todas la medidas obtenidas
# File parsing values
accuracy_file = "./metricas/KNN_accuracy.out"
time_file = "./tiempo.dat"
x_axis = []
acc_axis = []
time_axis = []
# sys call 
input_maker = ['python', './tools/make_input.py' \
    , '../data/ImagenesCaras/', '15' \
    , '-s', '30', '-ti', '[1,2,3]']
tp = ['./tp', 'data.in', 'data.out' \
    ,'-t',time_file , '-i']

# PWD es ahora src/
# para que guarde el data.in ahi, y poder llamar a ./tp mas facil
os.chdir("../")

print("Calenteando...")
for w in range(0, warm_up):
    subprocess.call(input_maker)
    subprocess.call(tp + [str(initial_value)], stdout=open(os.devnull, 'wb'))

print("Empezando experimento...")
for its in range(initial_value, max_value+step, step):
    print("Corriendo para ",its," iteraciones...")
    temp_time = []
    temp_acc = []
    x_axis.append(its)
    subprocess.call(input_maker)
    for t in range(0,trimming):
        subprocess.call(tp+[str(its)], stdout=open(os.devnull, 'wb'))
        acc_results = pd.read_table(accuracy_file, delim_whitespace=True, \
            names=['class','acc'])
        temp_acc.append(np.mean(acc_results["acc"]))
        t = open(time_file, "r")
        temp_time.append(float(t.read()))
        t.close()
    media_time = np.mean(temp_time)
    print("Tiempo medio: ", media_time)
    media_acc = np.mean(temp_acc)
    print("Accuracy media: ", media_acc)
    acc_axis.append(float(media_acc))
    time_axis.append(float(media_time))

# Cleaning
subprocess.call(["rm", time_file])


plt.plot(x_axis, time_axis, 'r-')
plt.ylabel('Segundos')
plt.xlabel('Cantidad de iteraciones en método potencia')
plt.savefig('metodo_potencia_time.pdf',format='pdf')
plt.clf()
plt.plot(x_axis, acc_axis, 'b-')
plt.ylabel('Accuracy')
plt.xlabel('Cantidad de iteraciones en método potencia')
plt.savefig('metodo_potencia_acc.pdf',format='pdf')





