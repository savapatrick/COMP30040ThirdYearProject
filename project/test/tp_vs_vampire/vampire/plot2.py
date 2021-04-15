import fileinput

import numpy
from matplotlib import pyplot

total = {}
sat = {}

for line in fileinput.input():
    a, p, c, res = line.split()
    a = int(a)
    p = int(p)
    c = int(c)
    if (a, p, c) in total:
        total[(a, p, c)] += 1
    else:
        total[(a, p, c)] = 1

    if (a, p, c) not in sat:
        sat[(a, p, c)] = 0
    if res[0] == 'S':
        sat[(a, p, c)] += 1


points = {}
cm = []
for marker in ['-', '--', '-.', ':']:
    for color in ['g', 'y', 'r', 'b', 'm']:
        cm.append(color+marker)

points = {}
for a in range(1, 51):
    curCol = 0
    for p in [10, 35, 60, 100]:
        for c in [10, 25, 40, 50]:
            if cm[curCol] not in points:
                points[cm[curCol]] = ([], [])
            points[cm[curCol]][0].append(a)
            points[cm[curCol]][1].append(sat[(a, p, c)]/total[(a, p, c)] * 100)
            curCol += 1

# for col in range(8):
#     pyplot.scatter(points[cm[col]][0], points[cm[col]][1])
#     pyplot.plot(points[cm[col]][0], points[cm[col]][1])
# 
for col in range(8, 16):
    pyplot.scatter(points[cm[col]][0], points[cm[col]][1])
    pyplot.plot(points[cm[col]][0], points[cm[col]][1], linestyle='dashed')

labels = []
# for p in [10, 35]:
#     for c in [10, 25, 40, 50]:
#         labels.append(f"P={p} and C={c}")
#
for p in [60, 100]:
    for c in [10, 25, 40, 50]:
        labels.append(f"P={p} and C={c}")

pyplot.legend(labels)
pyplot.xlabel("value for A")
pyplot.ylabel("Probability of Satisfiability")

pyplot.show()
