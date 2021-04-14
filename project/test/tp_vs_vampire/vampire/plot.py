from matplotlib import pyplot
import numpy
import fileinput

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

pyplot.scatter(numpy.array(SX), numpy.array(SY), c='g', marker='s', alpha=.1, s=400)
pyplot.scatter(numpy.array(UX), numpy.array(UY), c='r', marker='o', alpha=.1, s=400)

pyplot.legend(["Satisfiable", "Unsatisfiable"])
pyplot.xlabel("value for A")
pyplot.ylabel("value for C")

pyplot.show()
