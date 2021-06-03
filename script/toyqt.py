import sys
from PySide2 import QtCore, QtGui, QtWidgets


class ToyWindow(QtWidgets.QWidget):
	def __init__(self, parent=None):
		super().__init__(parent)
		self.button_shoot = QtWidgets.QPushButton("Shoot", self)
		self.button_shoot.clicked.connect(self.shoot)
		self.on_shoot = None

	def shoot(self):

		print('shoot from QPushButton')
		if self.on_shoot:
			self.on_shoot()

qtapp = None
qtwindow = None

def startup():
	global qtapp
	global qtwindow
	qtapp = QtWidgets.QApplication(sys.argv)
	window = ToyWindow()
	window.setGeometry(100, 100, 500, 500)
	window.show()
	qtwindow = window

def update():
	qtapp.processEvents()
