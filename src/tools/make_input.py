import os #listdir, path.join
import sys # argv

# LECTURA Y CONTROL DE LA ENTRADA
if len(sys.argv) == 1:
    print("HELP")
    print("La forma de ejecutar el script correctamente es: ")
    print(" python make_input.py dir n k test_image\n")
    print(" 'dir' es el directorio de la base de datos")
    print(" 'n' es la cantidad de sujetos a considerar")
    print(" 'k' es la cantidad de componentes principales a preservar")
    print(" 'test_image' es el número de imagen a usar para validar")
    print("")
    sys.exit(0)

if len(sys.argv) != 5:
    print("Número incorrecto de parámetros")
    print("Ejecute el script sin parámetros para recibir ayuda")
    sys.exit(1)

directory = sys.argv[1]
n = sys.argv[2]
k = sys.argv[3]
test_image = sys.argv[4]

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
sujetos = sujetos[0:int(n)] # poda
rutas = [os.path.join(directory, sub) for sub in sujetos]
with open(os.path.join('.', 'data.in'), 'w') as data:
    data.write(directory + ' ')
    data.write(str(height) + ' ')
    data.write(str(width) + ' ')
    data.write(str(len(sujetos)) + ' ')
    data.write(str(len(imagenes)-1) + ' ')
    data.write(k + '\n')

    for sub in sujetos:
        data.write(sub + '/ ')
        for i in list(range(1, len(imagenes)+1)):
            if str(i) != test_image:
                data.write(str(i) + ' ')
        data.write('\n')

    data.write(str(len(sujetos)) + '\n') #ntest
    for i in range(0, len(sujetos)):
        test_filename = test_image + ".pgm"
        data.write(os.path.join(rutas[i], test_filename))
        data.write(str(i+1) + '\n') #clase