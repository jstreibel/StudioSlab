import imgui
import numpy as np
from glumpy import gl
from glumpy.graphics.text import FontManager
from glumpy.transforms import Position, Trackball, Viewport
from glumpy.graphics.collections import GlyphCollection
from glumpy.graphics.collections import SegmentCollection

import imgui


class Grid:
    def reset(self):
        self.transform.theta = 0
        self.transform.phi = 0
        self.transform.zoom = 1 #16.5

    def __init__(self, windowEventDispatcher, baseTransform=None, title=""):
        self.viewport = Viewport()
        self.transform = Trackball(Position())

        self.windowEventDispatcher = windowEventDispatcher
        self.baseTransform = baseTransform

        self.xlims = -1, 1
        self.ylims = -1, 1
        self.z = 0

        self.title = title

        self.labels = None
        self.ticks = None

        self._regenerate()

        self.reset()

        windowEventDispatcher.attach(self)

        if baseTransform is not None:
            self.transform.theta = baseTransform.theta
            self.transform.phi = baseTransform.phi
            self.transform.zoom = baseTransform.zoom

        windowEventDispatcher.attach(self.labels["transform"])
        windowEventDispatcher.attach(self.labels["viewport"])

        self.doDrawGrid = False

    def _regenerate(self):
        if self.labels is None:
            self.labels = GlyphCollection(transform=self.transform, viewport=self.viewport)
            self.ticks = SegmentCollection(mode="agg", transform=self.transform, viewport=self.viewport,
                                           linewidth='local', color='local')
        else:
            while len(self.labels):
                del self.labels[0]

            while len(self.ticks):
                del self.ticks[0]

        self._generateLabels()

        self._generateFrame()

        self._generateGrids()

        self._generateMajorTicks()

        self._generateMinorTicks()

    def _generateLabels(self):
        labels = self.labels
        xmin, xmax = self.xlims
        z = self.z
        title = self.title

        regular = FontManager.get("OpenSans-Regular.ttf")
        bold = FontManager.get("OpenSans-Bold.ttf")
        n = 11
        scale = 0.001
        for i, y in enumerate(np.linspace(xmin, xmax, n)):
            text = "%.2f" % (i / 10.0)
            labels.append(text, regular,
                          origin=(1.05, y, z), scale=scale, direction=(1, 0, 0),
                          anchor_x="left", anchor_y="center")
            labels.append(text, regular, origin=(y, -1.05, z),
                          scale=scale, direction=(1, 0, 0),
                          anchor_x="center", anchor_y="top")

        labels.append(title, bold, origin=(0, 1.1, z),
                      scale=2 * scale, direction=(1, 0, 0),
                      anchor_x="center", anchor_y="center")

    def _generateFrame(self):
        xmin, xmax = self.xlims
        ymin, ymax = self.ylims
        z = self.z
        ticks = self.ticks

        P0 = [(xmin, ymin, z), (xmin, ymax, z), (xmax, ymax, z), (xmax, ymin, z)]
        P1 = [(xmin, ymax, z), (xmax, ymax, z), (xmax, ymin, z), (xmin, ymin, z)]
        ticks.append(P0, P1, linewidth=2)

    def _generateGrids(self):
        xmax, xmin = self.xlims
        ymax, ymin = self.ylims
        z = self.z
        ticks = self.ticks

        n = 11
        P0 = np.zeros((n - 2, 3))
        P1 = np.zeros((n - 2, 3))

        P0[:, 0] = np.linspace(xmin, xmax, n)[1:-1]
        P0[:, 1] = ymin
        P0[:, 2] = z
        P1[:, 0] = np.linspace(xmin, xmax, n)[1:-1]
        P1[:, 1] = ymax
        P1[:, 2] = z
        ticks.append(P0, P1, linewidth=1, color=(0, 0, 0, .25))

        P0 = np.zeros((n - 2, 3))
        P1 = np.zeros((n - 2, 3))
        P0[:, 0] = xmin
        P0[:, 1] = np.linspace(ymin, ymax, n)[1:-1]
        P0[:, 2] = z
        P1[:, 0] = xmax
        P1[:, 1] = np.linspace(ymin, ymax, n)[1:-1]
        P1[:, 2] = z
        ticks.append(P0, P1, linewidth=1, color=(0, 0, 0, .25))

    def _generateMajorTicks(self):
        xmax, xmin = self.xlims
        ymax, ymin = self.ylims
        z = self.z
        ticks = self.ticks

        n = 11
        P0 = np.zeros((n - 2, 3))
        P1 = np.zeros((n - 2, 3))
        P0[:, 0] = np.linspace(xmin, xmax, n)[1:-1]
        P0[:, 1] = ymin - 0.015
        P0[:, 2] = z
        P1[:, 0] = np.linspace(xmin, xmax, n)[1:-1]
        P1[:, 1] = ymin + 0.025 * (ymax - ymin)
        P1[:, 2] = z
        ticks.append(P0, P1, linewidth=1.5)
        P0[:, 1] = ymax + 0.015
        P1[:, 1] = ymax - 0.025 * (ymax - ymin)
        ticks.append(P0, P1, linewidth=1.5)

        P0 = np.zeros((n - 2, 3))
        P1 = np.zeros((n - 2, 3))
        P0[:, 0] = xmin - 0.015
        P0[:, 1] = np.linspace(ymin, ymax, n)[1:-1]
        P0[:, 2] = z
        P1[:, 0] = xmin + 0.025 * (xmax - xmin)
        P1[:, 1] = np.linspace(ymin, ymax, n)[1:-1]
        P1[:, 2] = z
        ticks.append(P0, P1, linewidth=1.5)
        P0[:, 0] = xmax + 0.015
        P1[:, 0] = xmax - 0.025 * (xmax - xmin)
        ticks.append(P0, P1, linewidth=1.5)

    def _generateMinorTicks(self):
        xmax, xmin = self.xlims
        ymax, ymin = self.ylims
        z = self.z
        ticks = self.ticks

        n = 111
        P0 = np.zeros((n - 2, 3))
        P1 = np.zeros((n - 2, 3))
        P0[:, 0] = np.linspace(xmin, xmax, n)[1:-1]
        P0[:, 1] = ymin
        P0[:, 2] = z
        P1[:, 0] = np.linspace(xmin, xmax, n)[1:-1]
        P1[:, 1] = ymin + 0.0125 * (ymax - ymin)
        P1[:, 2] = z
        ticks.append(P0, P1, linewidth=1)
        P0[:, 1] = ymax
        P1[:, 1] = ymax - 0.0125 * (ymax - ymin)
        ticks.append(P0, P1, linewidth=1)

        P0 = np.zeros((n - 2, 3))
        P1 = np.zeros((n - 2, 3))
        P0[:, 0] = xmin
        P0[:, 1] = np.linspace(ymin, ymax, n)[1:-1]
        P0[:, 2] = z
        P1[:, 0] = xmin + 0.0125 * (xmax - xmin)
        P1[:, 1] = np.linspace(ymin, ymax, n)[1:-1]
        P1[:, 2] = z
        ticks.append(P0, P1, linewidth=1)
        P0[:, 0] = xmax
        P1[:, 0] = xmax - 0.0125 * (xmax - xmin)
        ticks.append(P0, P1, linewidth=1)

    def draw(self):
        if not self.doDrawGrid:
            return

        w, h = self.WinSize
        gl.glViewport(0, 0, w, h)

        # self.window.clear()
        self.labels.draw()
        self.ticks.draw()

    def draw_menu(self):
        xmin, xmax = self.xlims

        imgui.text("Base grid")
        changed, (xmin, xmax) = imgui.slider_float2("x limits", xmin, xmax, -2, 2)
        if changed:
            self.xlims = xmin, xmax
            self._regenerate()

        pressed, value = imgui.checkbox("Draw base grid", self.doDrawGrid)
        if pressed:
            self.doDrawGrid = value

    def on_resize(self, w, h):
        self.WinSize = w, h
