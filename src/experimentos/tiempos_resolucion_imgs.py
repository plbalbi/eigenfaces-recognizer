import subprocess # call
import pandas as pd # read_table
import matplotlib.pyplot as plt # show
from mpl_toolkits.mplot3d import Axes3D # scatter

# Como al marica del python no le gustan las tildes hay que hacerse el angloparlante

# Variables
k = 5

sizes = []
sizes.append((23,28))
sizes.append((35,42))
sizes.append((52,63))
sizes.append((58,70))
sizes.append((69,84))
sizes.append((81,98))

tiempos = []

for size in sizes:

    # Creating images
    base = '../data/ImagenesCaras_' + str(size[0]) + 'x' + str(size[1])
    subprocess.call(['rm', '-f', '-r', base])
    subprocess.call(['./tools/resizePGM.sh', \
    '../data/ImagenesCaras',str(size[0]),str(size[1])])

    # Generating tp Input
    subprocess.call(['python', './tools/make_input.py' \
    , base+'/', str(k), '-ti', '[2]'])

    # Executing
    subprocess.call(['./tp','data.in','data.out','-t'])

    # Saving measured time
    medicion = open('tiempo.dat', 'r')
    tiempo = medicion.readline()

    tiempos.append(tiempo)
    subprocess.call(['rm', '-f', '-r', base])

print sizes
print tiempos
