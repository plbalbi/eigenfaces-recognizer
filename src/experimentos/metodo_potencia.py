import subprocess # call
import pandas as pd # read_table
import matplotlib.pyplot as plt # show
from mpl_toolkits.mplot3d import Axes3D # scatter

# Generating Input
# python tools/make_input.py ../data/ImagenesCaras/ 5 -s 10 -ti [1,2,3]
subprocess.call(['python', './tools/make_input.py' \
    , '../data/ImagenesCaras/', '5' \
    , '-s', '10', '-ts', '[1,2,3]'])

# Executing
# ./tp  data.in salida.out -v autovectores.txt -i 500
subprocess.call(['./tp', 'data.in', 'data.out' \
    , '-v', './autovectores.txt', '-i', '50'])

# Reading Output
vectores = pd.read_table('autovectores.txt', delim_whitespace=True, \
  names=['class','x','y','z'])

# Cleaning Up
subprocess.call(['rm', 'data.in', 'data.out' , 'autovectores.txt'])

# Plotting
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(vectores['x'], vectores['y'], vectores['z'], c=vectores['class'])
plt.show()
