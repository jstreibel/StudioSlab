from math import log, floor

def analyticDim():
    return log(4)/log(3)

def fractalDim(pointlist, eps):
    if eps >= 1.0: return -1, {}
    eps = float(eps)
    pointdict = {}
    for (x, y) in pointlist:
        i = int(floor(x/eps))
        j = int(floor(y/eps))
        index = (i, j)

        if index not in pointdict: pointdict[index] = 1

    N = len(pointdict.keys())
    if N == 0: return -1.0
    d =  log(N) / log(1/eps)
    return d, pointdict

if __name__ == "__main__":
    import koch
    from numpy import logspace, linspace, asarray

    koch_order = 7
    print("Generating fractal with order", koch_order)
    pointset, x, y = koch.makekoch(0, 0, 1, 0, koch_order)
    print("Done.")

    eps_list = []
    d_list = []
    N_list = []
    inveps_list = []
    eps_range = (logspace(-0.1,-7,75), linspace(1.e-6, 1.e-5, 20))[0]
    d_analytic = analyticDim()

    for eps in eps_range:
        d, pointdict = fractalDim(pointset, eps)
        print("eps =", eps, "\tdim =", d)
        eps_list.append(eps)
        d_list.append(d)
        N_list.append(len(pointdict.keys()))
        inveps_list.append(1./eps)
    eps_list = asarray(eps_list)
    d_list = asarray(d_list)

    if 1:
        from matplotlib import pyplot
        # loglog, semilogx, semilogy
        pyplot.semilogx(eps_list, d_list, label="Calulated for Koch set with " + str(koch_order) + " iterations.")
        pyplot.semilogx((eps_list.min(), eps_list.max()), (d_analytic, d_analytic), label="$dim_{Koch} = log4/log3$ (analytic)")
        pyplot.xlabel("$\epsilon$")
        pyplot.ylabel("$dim_b(set)=\dfrac{log(N(\epsilon))}{log(1/\epsilon)}$")
        #pyplot.plot(eps_list, d_list/d_analytic)
        #pyplot.plot((eps_list.min(), eps_list.max()), (1, 1))
        pyplot.grid()

        pyplot.legend()

        pyplot.show()

    elif 0:
        from matplotlib import pyplot
        pyplot.semilogx(inveps_list, N_list)
        pyplot.show()

    if 1:
        from matplotlib import pyplot

        if 1: pyplot.scatter(x, y, marker='.', label='Koch', s=1)
        else:
            boxes = []
            for key in pointdict:
                i, j = key
                x = i*eps
                y = j*eps

                sqr = mpatches.Rectangle((x, y), eps, eps, fill=False)
                boxes.append(sqr)

            colors = 100*np.random.rand(len(boxes))
            collection = PatchCollection(boxes)
            collection.set_array(colors)
            fig, ax = pyplot.subplots()
            ax.add_collection(collection)

        pyplot.ylim(0, 0.35)
        pyplot.show()
