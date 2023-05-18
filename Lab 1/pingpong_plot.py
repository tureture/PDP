import numpy
import matplotlib.pyplot as plt
from pathlib import Path



# Read data from file
p = Path(__file__).with_name('out.txt')
with p.open('r') as f:
    data = f.readlines()

    # Read data
    data = data[1:]
    for i in range(len(data)):
        data[i] = data[i].split()
        for j in range(len(data[i])):
            data[i][j] = float(data[i][j])

    print(data[1:5])

    size = []
    time = []
    rate = []

    for triple in data:
        size.append(triple[0])
        time.append(triple[1])
        rate.append(triple[2])

    # plot data in loglog scale using two y-axes
    fig, ax1 = plt.subplots()
    ax1.loglog(size, time, label="Time")
    ax1.set_xlabel("Size of message")
    ax1.set_ylabel("Time (µs)")
    ax1.legend()
    
    ax2 = ax1.twinx()
    ax2.loglog(size, rate, label="Rate", color="orange")
    ax2.set_ylabel("Rate (MB/s)")
    ax2.legend()
    plt.title("Time and rate of pingpong, TCP")
    plt.show()







