import subprocess # call
import os # FNULL
import pandas as pd # read_table
import numpy as np
import matplotlib.pyplot as plt # show
from mpl_toolkits.mplot3d import Axes3D # scatter

# Como al marica del python no le gustan las tildes hay que hacerse el angloparlante

FNULL = open(os.devnull, 'w')

# Variables
k = 1

images_true = []
images_false = []

images_false.append('../data/caraOno/nocaras/1.pgm')
images_false.append('../data/caraOno/nocaras/2.pgm')
images_false.append('../data/caraOno/nocaras/3.pgm')
images_false.append('../data/caraOno/nocaras/4.pgm')
images_false.append('../data/caraOno/nocaras/5.pgm')
images_false.append('../data/caraOno/nocaras/6.pgm')
images_false.append('../data/caraOno/nocaras/7.pgm')
images_false.append('../data/caraOno/nocaras/8.pgm')
images_false.append('../data/caraOno/nocaras/9.pgm')
images_false.append('../data/caraOno/nocaras/10.pgm')

images_true.append('../data/caraOno/caras/1.pgm')
images_true.append('../data/caraOno/caras/2.pgm')
images_true.append('../data/caraOno/caras/3.pgm')
images_true.append('../data/caraOno/caras/4.pgm')
images_true.append('../data/caraOno/caras/5.pgm')
images_true.append('../data/caraOno/caras/6.pgm')
images_true.append('../data/caraOno/caras/7.pgm')
images_true.append('../data/caraOno/caras/8.pgm')
images_true.append('../data/caraOno/caras/9.pgm')
images_true.append('../data/caraOno/caras/10.pgm')
images_true.append('../data/caraOno/caras/11.pgm')
images_true.append('../data/caraOno/caras/12.pgm')

resultados_true = []
resultados_false = []

# Probando con no-caras
print '# Probando con no-caras'
for img in images_false:
    print '\t', img,
    # Generating tp Input
    subprocess.call(['python', './tools/make_input.py' \
    , '../data/ImagenesCaras/', str(k), '-s', '30'])
    # Executing
    subprocess.call(['./tp','data.in','data.out','-c','2', img], \
    stdout=FNULL)
    # Reading res
    medicion = open('caraOno.dat', 'r')
    res = medicion.readline()
    resultados_false.append(res)
    if int(res):
    	print '\t', 'Cara'
    else:
    	print '\t','No-cara'

# Probando con caras
print '# Probando con caras'
for img in images_true:
    print '\t', img,
    # Generating tp Input
    subprocess.call(['python', './tools/make_input.py' \
    , '../data/ImagenesCaras/', str(k), '-s', '30'])
    # Executing
    subprocess.call(['./tp','data.in','data.out','-c','2', img], \
    stdout=FNULL)
    # Reading res
    medicion = open('caraOno.dat', 'r')
    res = medicion.readline()
    resultados_true.append(res)
    if int(res):
    	print '\t', 'Cara'
    else:
    	print '\t','No-cara'
