import subprocess # call
import pandas as pd # read_table
import matplotlib.pyplot as plt # show
from mpl_toolkits.mplot3d import Axes3D # scatter

# Generating Input
subprocess.call(['python', './tools/make_input.py' \
    , '../data/ImagenesCaras/', '3' \
    , '-s', '8'])

# Executing
subprocess.call(['./tp', 'data.in', 'data.out' \
    , '-v', './autovectores.txt'])

# Reading Output
vectores = pd.read_table('autovectores.txt', delim_whitespace=True, \
  names=['class','x','y','z'])

# Plotting
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(vectores['x'], vectores['y'], vectores['z'], c=vectores['class'])
plt.show()