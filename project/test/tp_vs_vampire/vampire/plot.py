import fileinput

import numpy
from matplotlib import pyplot

SX = []
SY = []
UX = []
UY = []

title = None

for line in fileinput.input():
    if title is None:
        title = line
        pyplot.title(title)
        continue
    x, y, res = line.split()
    if res[0] == 'S':
        SX.append(int(x))
        SY.append(int(y))
    else:
        UX.append(int(x))
        UY.append(int(y))

# the next two lines OF CODE are inspired (referenced) 
# from https://stackoverflow.com/questions/21081453/drawing-scatter-graph-using-matlibplot-pyplot-when-points-are-duplicate
# accessed on 15th of April 2021
pyplot.scatter(numpy.array(SX), numpy.array(SY), c='g', marker='s', alpha=.1, s=400)
pyplot.scatter(numpy.array(UX), numpy.array(UY), c='r', marker='o', alpha=.1, s=400)

pyplot.legend(["Satisfiable", "Unsatisfiable"])
pyplot.xlabel("value for A")
pyplot.ylabel("value for C")

pyplot.show()
