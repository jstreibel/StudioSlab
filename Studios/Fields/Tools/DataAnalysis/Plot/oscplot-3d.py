#!/usr/bin/env python3

# -*- coding: utf-8 -*-

import sys
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_field.py <filename>")
        sys.exit(1)

    filename = sys.argv[1]

    # Read the file
    with open(filename, 'rb') as f:
        # The first line is a Python dictionary.
        # We'll read it as a string, then eval it.
        header_line = f.readline().decode('utf-8').strip()

        # The provided dictionary line might have something like plot_theme: Dark2 which is not quoted.
        # If we trust the file format, we can provide a namespace that interprets "Dark2" as a string.
        env = {"False": False, "True": True, "Dark2": "Dark2"}
        params = eval(header_line, env)

        # Now read the separator line
        sep_line = f.readline()  # should be "\n<<<<-oOo->>>>\n"

        # Extract necessary parameters
        outresX = params["outresX"]
        outresY = params["outresY"]
        L = params["L"]

        # Now read the binary data as float64
        data_count = outresX * outresY
        data = np.fromfile(f, dtype=np.float64, count=data_count)
        Z = data.reshape((outresY, outresX))

    # Create coordinate arrays
    # The simulation is centered at the origin, space goes from -L/2 to L/2
    x = np.linspace(-L/2, L/2, outresX)
    y = np.linspace(-L/2, L/2, outresY)
    X, Y = np.meshgrid(x, y)

    # Plot the 3D surface
    fig = plt.figure(figsize=(8, 6))
    ax = fig.add_subplot(111, projection='3d')

    surf = ax.plot_surface(X, Y, Z, cmap='viridis', edgecolor='none')
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')

    plt.title('Simulation Output')
    plt.colorbar(surf, shrink=0.5, aspect=10)
    plt.show()

if __name__ == "__main__":
    main()
