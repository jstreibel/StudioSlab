import math
from matplotlib import pyplot

def fractaldim(pointlist, boxlevel):
        """Returns the approximate fractal dimension of pointlist,
via the box-counting algorithm.  The elements of pointset
should be three-element sequences of numbers between 0.0
and 1.0.  The boxlevel is the number of divisions made on
each dimension, and should be greater than 1."""

        if boxlevel <= 1.0: return -1

        pointdict = {}

        def mapfunction(val, level=boxlevel):
                return int(val * level)

        for point in pointlist:
                box = (int(point[0] * boxlevel),
                        int(point[1] * boxlevel),
                        int(point[2] * boxlevel))
                #box = tuple(map(mapfunction, point))
                if not box in pointdict:
                        pointdict[box] = 1

        num = len(pointdict.keys())

        if num == 0: return -1

        return math.log(num) / math.log(boxlevel)

print("line dimension")
list1d = []
for x in range(100):
    list1d.append((x/100.0, 0, 0))

for level in range(1,10):
    boxsize = int(2 ** level)
    print(level, fractaldim(list1d, boxsize))

print("plane dimension")
list2d = []
for x in range(100):
   for y in range(100):
       list2d.append((x/100.0, y/100.0, 0))

for level in range(1,10):
    boxsize = int(2 ** level)
    print(level, fractaldim(list2d, boxsize))

print("space dimension")
list3d = []
for x in range(30):
   for y in range(30):
      for z in range(30):
          list3d.append((x/30.0, y/30.0, z/30.0))

for level in range(1,10):
    boxsize = int(2 ** level)
    print(level, fractaldim(list3d, boxsize))

print("koch dimension")
def makekoch(x1, y1, x2, y2, level, flist=[], x=[], y=[]):
    xd = (x2 - x1)
    yd = (y2 - y1)
    xa = x1 + xd / 3.0
    ya = y1 + yd / 3.0
    xb = x1 + xd * 2.0 / 3.0
    yb = y1 + yd * 2.0 / 3.0
    xm = (x1 + x2) / 2.0 - 0.866 * yd / 3.0
    ym = (y1 + y2) / 2.0 + 0.866 * xd / 3.0
    flist.append((xa,ya,0))
    flist.append((xm,ym,0))
    flist.append((xb,yb,0))
    x.extend((xa, xm, xb))
    y.extend((ya, ym, yb))
    if (level > 0):
       makekoch(x1, y1, xa, ya, level-1, flist, x, y)
       makekoch(xa, ya, xm, ym, level-1, flist, x, y)
       makekoch(xm, ym, xb, yb, level-1, flist, x, y)
       makekoch(xb, yb, x2, y2, level-1, flist, x, y)
    return flist, x, y

listfract, x, y = makekoch(0,0,1,0,7)

pyplot.scatter(x, y, marker='.', label='Koch', s=1)
pyplot.legend()
pyplot.show()
for level in range(1,10):
    boxsize = int(2 ** level)
    print(level, fractaldim(listfract, boxsize))
