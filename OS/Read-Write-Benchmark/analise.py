import numpy
import matplotlib.pyplot as plt
import pandas
from os import path

# plt.style.use('_mpl-gallery')


RR = 'rand_read_0.txt'
RW = 'rand_write_0.txt'
SR = 'seq_read_0.txt'
SW = 'seq_write_0.txt'

rr = pandas.read_csv(path.join('Read-Write-Benchmark', RR))
rr.head()

# rr.plot.scatter(x='TIME', y='SIZE')
#
# plt.plot(rr.SIZE, rr.TIME)
