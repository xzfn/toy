
from unit_manager import UnitManager


class World:
	def __init__(self):
		self.frame_id = 0
		self.game_time = 0.0
		self.unit_manager = UnitManager(self)

	def tick(self, delta_time):
		self.frame_id += 1
		self.game_time += delta_time
		self.unit_manager.tick(delta_time)

	def render(self):
		self.unit_manager.render()
