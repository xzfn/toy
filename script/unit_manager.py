

class Unit:
	def __init__(self, world, unit_id, param):
		self.world = world
		self.name = param.get('name', 'actor')
		self.unit_id = unit_id

		self._is_destroyed = False

	def tick(self, delta_time):
		pass

	def render(self):
		pass

	def destroy(self):
		self._is_destroyed = True

	def is_destroyed(self):
		return self._is_destroyed


class UnitManager:
	def __init__(self, world):
		self.world = world

		self._incrementing_id = 0
		self._units = {}

	def generate_id(self):
		self._incrementing_id += 1
		return self._incrementing_id

	def create_unit(self, unit_class, param=None):
		if param is None:
			param = {}
		if 'unit_id' in param:
			unit_id = param['unit_id']
		else:
			unit_id = self.generate_id()
		unit = unit_class(self.world, unit_id, param)
		self._units[unit_id] = unit
		return unit

	def get_unit(self, unit_id):
		return self._units.get(unit_id, None)

	def get_units(self):
		return self._units.values()

	def get_units_type(self, typename):
		units = []
		for unit in self._units.values():
			if unit.is_destroyed():
				continue
			if type(unit).__name__ == typename:
				units.append(unit)
		return units

	def tick(self, delta_time):
		destroyed_units = []
		for unit in list(self._units.values()):
			if unit.is_destroyed():
				destroyed_units.append(unit.unit_id)
			else:
				unit.tick(delta_time)
		for unit_id in destroyed_units:
			del self._units[unit_id]

	def render(self):
		for unit in self._units.values():
			if not unit.is_destroyed():
				unit.render()
