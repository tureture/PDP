import numpy
import matplotlib.pyplot as plt
from pathlib import Path


# Data for strong scaling
size = [1, 2, 4, 8, 16, 32, 64]
time = [147.754917, 73.017609, 37.844799, 19.944223, 10.641305, 5.378276, 2.990382] 

# Calculate speedup
speedup = [time[0]/t for t in time]

# plot data
plt.plot(size, time, label="Time")
plt.xlabel("Nr of processes")
plt.ylabel("Time (s)")
plt.title("Strong scaling for monte carlo malaria simulation")
plt.legend()
plt.show()

# plot speedup
plt.plot(size, speedup, label="Speedup")
plt.xlabel("Nr of processes")
plt.ylabel("Speedup")
plt.title("Speedup for monte carlo malaria simulation")
plt.legend()
plt.show()