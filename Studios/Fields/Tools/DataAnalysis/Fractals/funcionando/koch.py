def makekoch(x1, y1, x2, y2, level, flist=[], x=[], y=[]):
    xd = (x2 - x1)
    yd = (y2 - y1)
    xa = x1 + xd / 3.0
    ya = y1 + yd / 3.0
    xb = x1 + xd * 2.0 / 3.0
    yb = y1 + yd * 2.0 / 3.0
    xm = (x1 + x2) / 2.0 - 0.866 * yd / 3.0
    ym = (y1 + y2) / 2.0 + 0.866 * xd / 3.0
    flist.append((xa,ya))
    flist.append((xm,ym))
    flist.append((xb,yb))
    x.extend((xa, xm, xb))
    y.extend((ya, ym, yb))
    if (level > 0):
       makekoch(x1, y1, xa, ya, level-1, flist, x, y)
       makekoch(xa, ya, xm, ym, level-1, flist, x, y)
       makekoch(xm, ym, xb, yb, level-1, flist, x, y)
       makekoch(xb, yb, x2, y2, level-1, flist, x, y)

    return flist, x, y
