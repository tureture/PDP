import numpy
import matplotlib.pyplot as plt
from pathlib import Path



# Data for weak scaling
p = [1, 2, 4, 8, 16, 32, 64]
time = [149.742400, 146.725736, 183.851135, 153.890661, 166.523417, 168.944424, 167.380044]

# calc speedup
speedup = [time[0]/t for t in time]


# plot speedup
plt.plot(p, speedup, label="Speedup", marker="o", linestyle="dashed")
plt.xlabel("Nr of processes")
plt.ylabel("Speedup")
plt.title("Speedup for weak scaling of monte carlo malaria simulation")
plt.legend()
plt.show()
