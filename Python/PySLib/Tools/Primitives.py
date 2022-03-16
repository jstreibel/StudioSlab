def Plane(size=(1.0, 1.0), n=(2, 2), uv_repeat=(1, 1)):
    """
    Plane centered at origin, lying on the XY-plane

    Parameters
    ----------
    size : float
       plane length size

    n : int
        Tesselation level
    """

    nx, ny = n
    nx, ny = max(2, nx), max(2, ny)

    indexOffsets = np.array([0, 1, nx + 1, 0, nx + 1, nx], dtype=np.uint32)
    print("Generating plane mesh")
    print("\tDimensions: {}x{}".format(size[0], size[1]))
    print("\tMesh tess: {}x{}={} vertices".format(nx, ny, nx * ny))
    print("\tFaces: {}x{}={} faces".format(nx-1,ny-1,(nx-1)*(ny-1)))
    print()

    Tx = np.linspace(0, 1, nx, endpoint=True)
    Ty = np.linspace(0, 1, ny, endpoint=True)

    X, Y = np.meshgrid(Tx - 0.5, Ty - 0.5)
    X = X.ravel() * size[0]
    Y = Y.ravel() * size[1]
    U, V = np.meshgrid(Tx, Ty)
    U = U.ravel() * uv_repeat[0]
    V = V.ravel() * uv_repeat[1]


    # print("Vertices:")
    # for i in range(nx * ny):
    #     print(i, X[i], Y[i])

    # print("\nFaces index offsets:", indexOffsets)

    def viewIndices(I, comment='', end=' '):
        for i, face in enumerate(I):
            print(comment+"{}: ".format(i), end=end)
            print(face)


    # print("\nIndices arange: ")
    I = np.arange((nx-1) * (ny-1), dtype=np.uint32)
    # viewIndices(I, 'face ')

    # print("\nIndices remapped: ")
    I += I//(nx-1)
    # viewIndices(I, 'Base index of face ')

    # print("\nIndices repeat and reshape: ")
    I = np.repeat(I, 6).reshape((ny-1),(nx-1), 6)
    # viewIndices(I, end='\n')

    # print("\nIndices: ")
    I[:, :] += indexOffsets
    # viewIndices(I, end='\n')

    # print("\nIndices arange: ")
    # I = np.arange((nx - 1) * (ny-1), dtype=np.uint32)
    # viewIndices(I)

    # print("\nIndices reshape: ")
    # I = I.reshape(nx - 1, ny)
    # viewIndices(I)

    # # I = I[:, :-1].T
    # print("\nIndices cut column and transpose: ")
    # I = I[:, :-1].T
    # viewIndices(I)

    # print("\nIndices pre-offset: ")
    # I = np.repeat(I.ravel(), 6).reshape(nx - 1, ny - 1, 6)
    # viewIndices(I)

    # print("\nIndices: ")
    # I[:, :] += indexOffsets
    # viewIndices(I)

    # print()

    # exit(0)

    vtype = [('position', np.float32, 3),
             ('texcoord', np.float32, 2),
             ('normal', np.float32, 3)]
    itype = np.uint32

    vertices = np.zeros((6, nx * ny), dtype=vtype)
    vertices["texcoord"][..., 0] = U
    vertices["texcoord"][..., 1] = V
    vertices["position"][0, :, 0] = X
    vertices["position"][0, :, 1] = Y
    vertices["position"][0, :, 2] = 0
    vertices["normal"][0] = 0, 0, 1

    vertices = vertices.ravel()
    indices = np.array(I, dtype=itype).ravel()
    return vertices.view(gloo.VertexBuffer), indices.view(gloo.IndexBuffer)


import numpy as np
from glumpy import gloo, data