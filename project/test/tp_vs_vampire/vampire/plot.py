from matplotlib import pyplot
import numpy
import fileinput

SX = []
SY = []
UX = []
UY = []

title = None

S = set()
U = set()

for line in fileinput.input():
    if title is None:
        title = line
        pyplot.title(title)
        continue
    x, y, res = line.split()
    if res[0] == 'S':
        SX.append(int(x))
        SY.append(int(y))
        S.add((int(x), int(y)))
    else:
        UX.append(int(x))
        UY.append(int(y))
        U.add((int(x), int(y)))

# common = list(S.intersection(U))
# CX, CY = zip(*sorted(common))
#
# assert len(SX) == len(SY)
# assert len(UX) == len(UY)
#
# S = []
# for ind in range(len(SX)):
#     if (SX[ind], SY[ind]) not in common:
#         S.append((SX[ind], SY[ind]))
#
# U = []
# for ind in range(len(CX)):
#     if (CX[ind], CY[ind]) not in common:
#         U.append((CX[ind], CY[ind]))
#
# if len(S) > 0:
#     SX, SY = zip(*sorted(S))
# else:
#     SX, SY = [], []
#
# if len(U) > 0:
#     UX, UY = zip(*sorted(U))
# else:
#     UX, UY = [], []

# pyplot.plot(numpy.array(SX), numpy.array(SY), 'g+')
pyplot.scatter(numpy.array(SX), numpy.array(SY), c='g', marker='s', alpha=.1, s=400)
# pyplot.plot(numpy.array(UX), numpy.array(UY), 'r+')
pyplot.scatter(numpy.array(UX), numpy.array(UY), c='r', marker='o', alpha=.1, s=400)
# pyplot.plot(numpy.array(CX), numpy.array(CY), 'b+')

# pyplot.legend(["Satisfiable", "Unsatisfiable", "Sat/Unsat"])
pyplot.legend(["Satisfiable", "Unsatisfiable"])
pyplot.xlabel("value for A")
pyplot.ylabel("value for C")

pyplot.show()
