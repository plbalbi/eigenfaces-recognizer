import numpy as np
import os #listdir, path.join
import sys # argv

IMG_SIZE = 92*112

# ------------ separador de bajo presupuesto ------------
def read_pgm(image_route):
    pgmf = open(image_route, 'r')
    """Return a raster of integers from a PGM as a list of lists."""
    assert pgmf.readline() == 'P5\n'
    if peek(pgmf, 1) == '#':
        # Next line is a comment, ignore it
        pgmf.readline()
    (width, height) = [int(i) for i in pgmf.readline().split()]
    depth = int(pgmf.readline())
    assert depth <= 255
    raster = []
    for y in range(height*width):
        raster.append(ord(pgmf.read(1)))
    return raster

def peek(self, cnt):
    data = self.read(cnt)
    self.seek(cnt * -1, 1)
    return data

# ------------ separador de bajo presupuesto ------------
if len(sys.argv) == 1:
    print("HELP")
    print("La forma de ejecutar el script correctamente es: ")
    print(" python canary.py dir k\n")
    print(" 'dir' es el directorio de la base de datos")
    print(" 'k' es la cantidad de componentes principales a preservar")
    print("")
    sys.exit(0)

if len(sys.argv) != 3:
    print("Numero incorrecto de parametros")
    print("Ejecute el script sin parametros para recibir ayuda")
    sys.exit(1)

directory = sys.argv[1]
k = sys.argv[2]


if not os.path.exists(directory):
    print("Error: no existe una base de datos en {}".format(dir))
    sys.exit(1)

contents = [os.path.join(directory, f) for f in os.listdir(directory)]
sujetos = [f for f in contents if os.path.isdir(f)]
# print(sujetos)
# sys.exit(0)
carasPorSujeto = []
for i in range(len(sujetos)):
    dirSujeto = sujetos[i]
    fotinguis = [os.path.join(dirSujeto, f) for f in os.listdir(dirSujeto)]
    carasPorSujeto.append(fotinguis)

# print(fotinguis)
faces = []
for s in range(len(carasPorSujeto)):
    caras_temp = []
    for i in range(len(carasPorSujeto[s])):
        row = np.reshape(read_pgm(carasPorSujeto[s][i]), (1,IMG_SIZE))
        caras_temp.append(row)
    faces.append(caras_temp)

cant_sujetos = len(faces)
img_por_sujeto = len(faces[0])

# ------------ separador de bajo presupuesto ------------
# aplano las caras en un solo vector
plain_faces = []
for s in range(cant_sujetos):
    for i in range(img_por_sujeto):
        plain_faces.append(faces[s][i])
# matriz de caras
X = np.reshape(plain_faces, (cant_sujetos*img_por_sujeto, IMG_SIZE))
# calculo la media para "normalizar"
media = np.zeros(IMG_SIZE)
for i in range(cant_sujetos*img_por_sujeto):
    media = media + plain_faces[i]
media = media/(cant_sujetos*img_por_sujeto)
# Le resto la media a cada cara
for i in range(len(X)):
    X[i] = X[i] - media
# armo matriz de covarianza
Xtranspose = X.T
M_x = np.dot(Xtranspose, X)
