import numpy
import matplotlib.pyplot as plt
from pathlib import Path



# Read data from file


size = [1, 2, 4, 8, 16]
time = [8.203398, 4.279511, 2.491064, 1.802845, 1.763486]



# plot data
plt.plot(size, time, label="Time")
plt.xlabel("Nr of processes")
plt.ylabel("Time (s)")
plt.title("Time for calculating A^T*A*x using MPI")
plt.legend()
plt.show()

