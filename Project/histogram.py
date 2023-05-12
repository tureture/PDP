import numpy
import matplotlib.pyplot as plt
from pathlib import Path



# Read data from file
p = Path(__file__).with_name('out.txt')
with p.open('r') as f:
    data = f.readlines()

    # Read headers
    headers = data[0].split(",")
    N = headers[0].split(":")[1]
    Time = headers[1].split(":")[1]
    NrBins = headers[2].split(":")[1]
    Min = headers[3].split(":")[1]
    Max = headers[4].split(":")[1]

    # Read data
    data = data[1:]
    for i in range(len(data)):
        data[i] = data[i].strip().split(" ")
        for j in range(len(data[i])):
            data[i][j] = int(data[i][j])

    x = []
    w = []
    y = []
    for triple in data:
        x.append(triple[1])
        w.append(triple[0]-triple[1])
        y.append(triple[2])



    # Plot histogram (as a bar plot since we have already computed bins)
    plt.bar(x, y, width=w, align='edge', edgecolor='black')
    plt.title("Nr susceptible humans in " + N + " MC SSA Malaria simulations")
    plt.xlabel("Nr susceptible humans")
    plt.ylabel("Frequency")
    plt.show()


