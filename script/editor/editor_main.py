

import toy

import PySide2
from PySide2 import QtWidgets, QtCore

from .qtui import ui_main_window

import toyentry


class GameWidget(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

    def resizeEvent(self, event: PySide2.QtGui.QResizeEvent) -> None:
        self.reposition_game_window()

    def moveEvent(self, event: PySide2.QtGui.QMoveEvent) -> None:
        self.reposition_game_window()

    def reposition_game_window(self):
        size = self.size()
        global_position = self.mapToGlobal(QtCore.QPoint(0, 0))
        x = global_position.x()
        y = global_position.y()
        width = size.width()
        height = size.height()
        toy.app.get_window().move_window(x, y, width,height)


class EditorMainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = ui_main_window.Ui_MainWindow()
        self.game_widget = None

        self.ui.setupUi(self)

        self.init_game_widget()
        self.bind_signals_and_slots()

    def moveEvent(self, event: PySide2.QtGui.QMoveEvent) -> None:
        self.game_widget.reposition_game_window()

    def closeEvent(self, event: PySide2.QtGui.QCloseEvent) -> None:
        event.ignore()
        toy.app.set_should_close()

    def init_game_widget(self):
        layout = QtWidgets.QVBoxLayout(self.ui.widgetGameArea)
        self.game_widget = GameWidget()
        layout.addWidget(self.game_widget)

    def bind_signals_and_slots(self):
        ui = self.ui
        ui.actionDebug.triggered.connect(self._on_action_debug)
        ui.actionAbout.triggered.connect(self._on_action_about)

    def show_status(self, msg):
        duration_ms = 5000
        self.ui.statusbar.showMessage(msg, duration_ms)

    def _on_action_about(self):
        QtWidgets.QMessageBox.about(self, "About Toy Editor", "Editor for toy engine")

    def _on_action_debug(self):
        toyentry.app.on_action_debug()


class Editor(object):
    def __init__(self):
        import sys
        self.app = QtWidgets.QApplication(sys.argv)
        self.main_window = EditorMainWindow()
        self.main_window.show()

        editor_hwnd = self.main_window.winId()
        toy.app.get_window().set_win32_hwnd_parent(editor_hwnd)
        toy.app.get_window().set_borderless(True)

    def update(self):
        self.app.processEvents()
