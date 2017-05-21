# -*- coding: utf-8 -*-

import os # listdir, path.join
import sys # argv
import argparse # ArgumentParser
import ast # literal_eval


# PARSEO
if len(sys.argv) == 1:
    sys.exit("Utilice los flags -h y --help para consultar los parámetros de entrada\n")

parser = argparse.ArgumentParser( description="Genera un archivo de entrada válido para el ejecutable 'tp'" )
parser.add_argument( "dir", help="ruta de la base de datos" )
parser.add_argument( "k", help="cantidad de componentes principales", type=int )
parser.add_argument( "-s", help="cantidad de sujetos a considerar (default = all)", default=1000000, type=int )
parser.add_argument( "-i", help="cantidad de imágenes por sujeto considerar (default = all)", default=1000000, type=int )
parser.add_argument( "-ts", help="lista con sujetos para testear (default = none)", default="[]")
parser.add_argument( "-ti", help="lista con números de imagenes para testear (default = [1])", default="[1]")
args = parser.parse_args()

directory = args.dir
k = args.k
lim_sujetos = args.s
lim_imgs = args.i
test_rows = ast.literal_eval(args.ts)
test_cols = ast.literal_eval(args.ti)


# CONTROL DE LA ENTRADA
# TODO verificar que los parámetros están bien

# Verificación de la integridad de la base
if not os.path.exists(directory):
    print("Error: no existe una base de datos en {}".format(dir))
    sys.exit(1)

archivos = [os.path.join(directory, f) for f in os.listdir(directory)]
sujetos = [f for f in archivos if os.path.isdir(f)]

if len(sujetos) == 0:
    print("Error: No hay sujetos en la base de datos ubicada en {}".format(directory))
    sys.exit(1)

for sub in sujetos:
    archivos = [os.path.join(sub, f) for f in os.listdir(sub)]
    imagenes = [f for f in archivos if os.path.isfile(f)]
    if len(imagenes) == 0:
        print("Error: No hay imagenes para el sujeto ubicado en {}".format(sub))
        sys.exit(1)

# Verificación de dimensiones congruentes
def get_dimensions(img):
    with open(img, 'rb') as pgm:
        temp = pgm.readline().split()
        assert temp[0] == b'P5'
        if len(temp) == 4:
            (width, height) = [int(b) for b in temp[1:3]]
        else:
            temp = pgm.readline().rstrip()
            if temp[0:1] == b'#':
                (width, height) = [int(b) for b in pgm.readline().split()]
            else:
                (width, height) = [int(b) for b in temp.split()]
    return (width, height)

imagen_base = imagenes[0]
(width, height) = get_dimensions(imagen_base)

for sub in sujetos:
    archivos = [os.path.join(sub, f) for f in os.listdir(sub)]
    imagenes = [f for f in archivos if os.path.isfile(f)]
    for img in imagenes:
        (w, h) = get_dimensions(img)
        if w != width or h != height:
            print("Error: Incongruencia de tamaños")
            print("Tamaño de la imagen {}: {} x {}".format(imagen_base, width, height))
            print("Tamaño de la imagen {}: {} x {}".format(img, w, h))
            sys.exit(1)


# GENERACIÓN DEL ARCHIVO
# Nota: 'sujetos' antes eran rutas, y ahora son nombres (porque Python me deja)
sujetos = sorted([f for f in os.listdir(directory) if os.path.isdir(os.path.join(directory, f))])
sujetos = sujetos[0:lim_sujetos] # poda
rutas = [os.path.join(directory, sub) for sub in sujetos]

imgs_per_subject = min(len(imagenes), lim_imgs)
nimgp = imgs_per_subject - len(test_cols)

with open(os.path.join('.', 'data.in'), 'w') as data:
    # Primer linea
    data.write(directory + ' ')
    data.write(str(height) + ' ')
    data.write(str(width) + ' ')
    data.write(str(len(sujetos)) + ' ')
    data.write(str(nimgp) + ' ')
    data.write(str(k) + '\n')

    # Imágenes de entrenamiento
    for j in range(0, len(sujetos)):
        if j in test_rows:
            continue
        data.write(sujetos[j] + '/ ')
        for i in list(range(1, imgs_per_subject+1)):
            if not i in test_cols:
                data.write(str(i) + ' ')
        data.write('\n')

    # Imágenes de testeo
    n_test = len(test_cols) * len(sujetos) + len(test_rows) * nimgp
    data.write(str(n_test) + '\n')
    for j in range(0, len(sujetos)):
        if not j in test_rows:
            continue
        for i in list(range(1, imgs_per_subject+1)):
            test_filename = str(i) + ".pgm"
            data.write(os.path.join(rutas[j], test_filename))
            data.write(' ' + str(j+1) + '\n') #clase
    for i in test_cols:
        for j in range(0, len(sujetos)):
            test_filename = str(i) + ".pgm"
            data.write(os.path.join(rutas[j], test_filename))
            data.write(' ' + str(j+1) + '\n') #clase
