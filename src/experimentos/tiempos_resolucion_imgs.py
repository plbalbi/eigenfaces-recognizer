import subprocess # call
import os # FNULL
import pandas as pd # read_table
import numpy as np
import matplotlib.pyplot as plt # show
from mpl_toolkits.mplot3d import Axes3D # scatter

# Como al marica del python no le gustan las tildes hay que hacerse el angloparlante
# Python3 boi

FNULL = open(os.devnull, 'w')

# Variables
k = 1

sizes = []
sizes.append((23,28))
# sizes.append((25,30))
sizes.append((28,33))
# sizes.append((31,36))
sizes.append((35,42))
# sizes.append((37,45))
# sizes.append((39,47))
sizes.append((52,63))
# sizes.append((58,70))
# sizes.append((69,84))
# sizes.append((81,98))
sizes.append((112,92))

tiempos1 = []
tiempos2 = []

for size in sizes:
    print "Creating ", size, " images"

    # Creating images
    base = '../data/ImagenesCaras_' + str(size[0]) + 'x' + str(size[1])
    subprocess.call(['rm', '-f', '-r', base])
    subprocess.call(['./tools/resizePGM.sh', \
    '../data/ImagenesCaras',str(size[0]),str(size[1])])

    # Generating tp Input
    subprocess.call(['python', './tools/make_input.py' \
    , base+'/', str(k), '-ti', '[2]'])

    print "Running 'Cara A Cara'"

    muestras = []
    for i in range(0,3):
        # Executing
        subprocess.call(['./tp','data.in','data.out','-t', '-it', '20'], \
        stdout=FNULL)

        # Reading measured time
        medicion = open('tiempo.dat', 'r')
        tiempo = medicion.readline()

        muestras.append(float(tiempo))

    tiempos1.append(min(muestras))

    muestras = []
    for i in range(0,3):
        # Executing
        subprocess.call(['./tp','data.in','data.out','-t', '-m','-it', '20'], \
        stdout=FNULL)

        # Reading measured time
        medicion = open('tiempo.dat', 'r')
        tiempo = medicion.readline()

        muestras.append(float(tiempo))

    tiempos2.append(min(muestras))

    subprocess.call(['rm', '-f', '-r', base])


tiempos1[:] = [x * 1000 for x in tiempos1]
tiempos2[:] = [x * 1000 for x in tiempos2]

pixels = []
for size in sizes:
    pixels.append(size[0]*size[1])


###### TABLA #######

print '###### TABLE ######'
print 'Sizes: ', sizes
print 'Resolutions: ', pixels
print 'Tiempos X*Xt: ', tiempos1
print 'Tiempos Xt*X: ', tiempos2


####### BARRAS  #########

N = 3
ind = np.arange(N)  # the x locations for the groups
width = 0.4       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)

rects1 = ax.bar(ind, tiempos1[0:N], width, color='r')
rects2 = ax.bar(ind+width, tiempos2[0:N], width, color='g')

ax.set_ylabel('Tiempo [mseg]')
ax.set_xticks(ind+width)
ax.set_xticklabels( (\
str(sizes[0][0])+'x'+str(sizes[0][1]) + '\n' + str(pixels[0]) + ' pixels', \
str(sizes[1][0])+'x'+str(sizes[1][1]) + '\n' + str(pixels[1]) + ' pixels', \
str(sizes[2][0])+'x'+str(sizes[2][1]) + '\n' + str(pixels[2]) + ' pixels' ) )

ax.legend( (rects1[0], rects2[0]), ('Tiempos usando X*Xt', 'Tiempo usando la matriz de covarianza') , loc='upper left')

plt.savefig('tiempos_resolucion_imgs.pdf',format='pdf')


##### CURVAS #####


# plt.plot(pixels,tiempos1,'r-') # r- = red line
# plt.plot(pixels,tiempos2,'bo') # b- = blue line
#
# plt.yscale('log')
# plt.ylim(ymin=0)
#
# plt.ylabel('Tiempo [mseg]')
# plt.xlabel('Cantidad de pixels por imagen')
#
# plt.legend(['Utilizando la matriz de covarianza \n para calcular la transf. carac.'], loc='upper left')
#
# plt.savefig('tiempos_resolucion_imgs.pdf',format='pdf')
#
# plt.close()
