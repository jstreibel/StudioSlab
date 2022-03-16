#!/usr/bin/env python3

from navigator import Ui_MainWindow
from PyQt5.QtWidgets import QMainWindow, QApplication, qApp, \
                            QFileDialog, QLabel, QSlider, QAction, \
                            QShortcut
from PyQt5 import QtGui
from PyQt5.QtCore import QRegExp

import sys

from simulation import Simulation, getCWD

class SkyrmeExplorer(QMainWindow):
    def __init__(self, parent=None):
        super(SkyrmeExplorer, self).__init__(parent)

        ui = Ui_MainWindow()
        ui.setupUi(self)

        # Setting up by hand some qt-designer oddities
        ui.dckHistoryPlot.setVisible(False)

        # Hook everything up
        ui.action_Open.triggered.connect(self.OpenDir)
        ui.btnGenerate.clicked.connect(self.Update)

        # Slider limits
        self.sldn = 1000.
        ui.sldv.valueChanged.connect(self.sldFloatValues)
        ui.sldv.setMaximum(self.sldn)
        ui.sldV.valueChanged.connect(self.sldFloatValues)
        ui.sldV.setMaximum(self.sldn)
        ui.sldPhase.valueChanged.connect(self.sldFloatValues)
        ui.sldPhase.setMaximum(self.sldn)
        ui.sldSymmetry.setDisabled(True)

        # Radio button labels
        ui.rbtT.toggled.connect(self.changeSymmetryLabel)
        ui.rbtX.toggled.connect(self.changeSymmetryLabel)

        # History Mode options
        ui.chkMarker.stateChanged.connect(self.toggleHistoryMarker)
        ui.sldSymmetry.valueChanged.connect(ui.wgtHistoryPlot.update)
        ui.wgtHistoryPlot.setSlider(ui.sldSymmetry)
        ui.btnPlayHistory.clicked.connect(ui.wgtHistoryPlot.play)
        ui.btnPlayHistory.setDisabled(True)
        ui.spnAnimSpeed.valueChanged.connect(ui.wgtHistoryPlot.setAnimInterval)

        self.ui = ui

        self.slddx = self.sldn*1.e-2
        self._setupShortcuts()

        ui.wgtNavigatorPlot.readDir(getCWD())
        ui.wgtNavigatorPlot.plot()

        self._sims = {}

    def _setupShortcuts(self):
        seq = [('I', self.ui.sldv, self.slddx),  ('K', self.ui.sldv, -self.slddx),
               ('Ctrl+I', self.ui.sldv, self.slddx*.1),  ('Ctrl+K', self.ui.sldv, -self.slddx*.1),
               ('L', self.ui.sldV, self.slddx),  ('J', self.ui.sldV, -self.slddx),
               ('Ctrl+L', self.ui.sldV, self.slddx*.1),  ('Ctrl+J', self.ui.sldV, -self.slddx*.1),
               ('O', self.ui.sldPhase, self.slddx),  ('U', self.ui.sldPhase, -self.slddx),
               ('Ctrl+O', self.ui.sldPhase, self.slddx*.1),  ('Ctrl+U', self.ui.sldPhase, -self.slddx*.1)]
        self.shortcuts = {}
        for entry in seq:
            shortcut = QShortcut(QtGui.QKeySequence(entry[0]), self)
            self.shortcuts[shortcut] = entry
            shortcut.activated.connect(self._inc)

        QShortcut(QtGui.QKeySequence(' '), self).activated.connect(self._mark)

    def _mark(self):
        print("Mark.")

    def _inc(self):
        shortcut, sld, dx = self.shortcuts[self.sender()]
        sld.setValue(sld.value()+dx)

        self.Update()

    def getv(self):
        return self.ui.sldv.value() / self.sldn

    def getV(self):
        return self.ui.sldV.value() / self.sldn

    def getPhase(self):
        return self.ui.sldPhase.value() / self.sldn

    def OpenDir(self):
        dirName = QFileDialog.getExistingDirectory(self, 'Pick the simulations directory',
                                                   './', QFileDialog.ShowDirsOnly)

    def Update(self):
        v, V, ph = self.getv(), self.getV(), self.getPhase()
        override = self.ui.chkRegen.isChecked()

        if not (v, V, ph) in self._sims:
            sim = Simulation(4.25, 7, 4096, v=v, V=V, ph=ph, overrideExisting=override)
            self._sims[(v, V, ph)] = sim
        elif override:
            sim = Simulation(4.25, 7, 4096, v=v, V=V, ph=ph, overrideExisting=override)
        else:
            sim = self._sims[(v, V, ph)]

        if not sim.imgExists() and not sim.simExists():
            self.ui.wgtNavigatorPlot.addSim(self.getv(), self.getV(), self.getPhase())

        saveFigure = self.ui.chkSaveImage.isChecked()
        preferImage = self.ui.chkPreferImage.isChecked()
        regenImage = self.ui.chkRegenImg.isChecked()
        useLogScale = self.ui.chkLogScale.isChecked()
        logEps = float(self.ui.epsilonSpinBox.value())
        imgWidth = int(self.ui.widthSpinBox.value())
        self.ui.wgtMainMatplot.plot(sim, preferImage=preferImage, saveFigure=saveFigure, regenImage=regenImage, 
                                    useLogScale=useLogScale, logEps=logEps, imgWidth=imgWidth)

        if self.ui.wgtHistoryPlot.isVisible():
            self.ui.wgtHistoryPlot.plot(sim)
            self.setHistoryLimits(sim._alles['outresT'])
            self.ui.sldSymmetry.setEnabled(True)
            self.ui.btnPlayHistory.setEnabled(True)

    def sldFloatValues(self, value):
        # This only works using the convention slider -> sliderValue
        senderName = self.sender().objectName()
        self.sender().parent().findChildren(QLabel, QRegExp(senderName + "Value"))[0].setText(str(value / self.sldn))
        self.ui.wgtNavigatorPlot.updateLocation(self.getv(), self.getV(), self.getPhase())

    def setHistoryLimits(self, extreme):
        self.ui.sldSymmetry.setMaximum(extreme-1)

    def changeSymmetryLabel(self):
        self.ui.lblSymmetryMode.setText(self.sender().property('modeLabel'))

    def toggleHistoryMarker(self, state):
        if self.ui.chkMarker.isChecked():
            self.ui.wgtMainMatplot.showHistoryMarker()
        else:
            self.ui.wgtMainMatplot.hideHistoryMarker()


if __name__ == '__main__':
    app = QApplication(sys.argv)

    window = SkyrmeExplorer()

    window.show()

    sys.exit(app.exec_())
